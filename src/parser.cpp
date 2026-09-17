#include "parser.hpp"
#include <iostream>

/* excepcion interna usada solo para cuando consumir() o
primario() no encuentran lo que esperaban, se "corta" la regla actual
y se sube hasta declaracion()/bloque(), que atajan esto y llaman a
sincronizar() para seguir buscando más errores en vez de abortar todo.*/
namespace {
    struct ErrorSintactico {};
}

Parser::Parser(const std::vector<Token>& tokensEntrada) {
    tokens = tokensEntrada;
    pos = 0;
    errores = false;
}

bool Parser::huboErrores() const { return errores; }

// utilidades de recorrido

Token Parser::actual() const { return tokens[pos]; }
Token Parser::anterior() const { return tokens[pos - 1]; }

bool Parser::esFinDeTokens() const {
    return actual().type == TokenType::END_OF_FILE;
}

bool Parser::revisar(TokenType tipo) const {
    if (esFinDeTokens()) return false;
    return actual().type == tipo;
}

Token Parser::avanzar() {
    if (!esFinDeTokens()) pos++;
    return anterior();
}

bool Parser::coincide(TokenType tipo) {
    if (revisar(tipo)) {
        avanzar();
        return true;
    }
    return false;
}

Token Parser::consumir(TokenType tipo, const std::string& mensaje) {
    if (revisar(tipo)) return avanzar();
    error(mensaje);
    throw ErrorSintactico{};
}

void Parser::error(const std::string& mensaje) {
    errores = true;
    Token t = actual();
    std::cerr << "[linea " << t.linea << ", columna " << t.columna << "] Error sintactico: "
              << mensaje << " (se encontro '" << t.lexema << "')" << std::endl;
}

void Parser::sincronizar() {
    avanzar();
    while (!esFinDeTokens()) {
        if (anterior().type == TokenType::SEMICOLON) return;
        switch (actual().type) {
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        avanzar();
    }
}

// punto de entrada
// programa declaracion*
void Parser::parsePrograma() {
    while (!esFinDeTokens()) {
        try {
            declaracion();
        } catch (const ErrorSintactico&) {
            sincronizar();
        }
    }
}

// declaracion declaracionFuncion | declaracionVariable | sentencia
void Parser::declaracion() {
    if (revisar(TokenType::FN)) { declaracionFuncion(); return; }
    if (revisar(TokenType::LET)) { declaracionVariable(); return; }
    sentencia();
}

// declaracionFuncion "fn" identificador "(" parametros? ")" ("->" tipoDato)? bloque
void Parser::declaracionFuncion() {
    consumir(TokenType::FN, "se esperaba 'fn'");
    consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la funcion");
    consumir(TokenType::LPAREN, "se esperaba '(' despues del nombre de la funcion");
    if (!revisar(TokenType::RPAREN)) {
        parametros();
    }
    consumir(TokenType::RPAREN, "se esperaba ')' despues de los parametros");
    if (coincide(TokenType::ARROW)) {
        tipoDato();
    }
    bloque();
}

// parametros identificador ":" tipoDato ("," identificador ":" tipoDato)*
void Parser::parametros() {
    consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre del parametro");
    consumir(TokenType::COLON, "se esperaba ':' despues del nombre del parametro");
    tipoDato();
    while (coincide(TokenType::COMMA)) {
        consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre del parametro");
        consumir(TokenType::COLON, "se esperaba ':' despues del nombre del parametro");
        tipoDato();
    }
}

// tipoDato "i32" | "f64" | "bool" | "char" | "str"
void Parser::tipoDato() {
    if (coincide(TokenType::TIPO_I32)) return;
    if (coincide(TokenType::TIPO_F64)) return;
    if (coincide(TokenType::TIPO_BOOL)) return;
    if (coincide(TokenType::TIPO_CHAR)) return;
    if (coincide(TokenType::TIPO_STR)) return;
    error("se esperaba un tipo de dato (i32, f64, bool, char, str)");
    throw ErrorSintactico{};
}

// bloque "{" declaracion* "}"
void Parser::bloque() {
    consumir(TokenType::LBRACE, "se esperaba '{'");
    while (!revisar(TokenType::RBRACE) && !esFinDeTokens()) {
        try {
            declaracion();
        } catch (const ErrorSintactico&) {
            sincronizar();
        }
    }
    consumir(TokenType::RBRACE, "se esperaba '}'");
}

// declaracionVariable "let" "mut"? IDENT (":" tipoDato)? "=" expresion ";"
void Parser::declaracionVariable() {
    consumir(TokenType::LET, "se esperaba 'let'");
    coincide(TokenType::MUT); // opcional
    consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la variable");
    if (coincide(TokenType::COLON)) {
        tipoDato();
    }
    if (coincide(TokenType::ASSIGN)) {
    expresion();
    }
    consumir(TokenType::SEMICOLON, "se esperaba ';' al final de la declaracion");
}

// sentencia sentenciaIf, sentenciaWhile, sentenciaFor, sentenciaReturn, bloque, sentenciaExpresion
void Parser::sentencia() {
    if (revisar(TokenType::IF)) { sentenciaIf(); return; }
    if (revisar(TokenType::WHILE)) { sentenciaWhile(); return; }
    if (revisar(TokenType::FOR)) { sentenciaFor(); return; }
    if (revisar(TokenType::RETURN)) { sentenciaReturn(); return; }
    if (revisar(TokenType::LBRACE)) { bloque(); return; }
    sentenciaExpresion();
}

// sentenciaExpresion expresion ";"
void Parser::sentenciaExpresion() {
    expresion();
    consumir(TokenType::SEMICOLON, "se esperaba ';' despues de la expresion");
}

// sentenciaIf "if" expresion bloque ("else" (sentenciaIf | bloque))?
void Parser::sentenciaIf() {
    consumir(TokenType::IF, "se esperaba 'if'");
    expresion();
    bloque();
    if (coincide(TokenType::ELSE)) {
        if (revisar(TokenType::IF)) {
            sentenciaIf();
        } else {
            bloque();
        }
    }
}

// sentenciaWhile "while" expresion bloque
void Parser::sentenciaWhile() {
    consumir(TokenType::WHILE, "se esperaba 'while'");
    expresion();
    bloque();
}

// sentenciaFor "for" identificador "in" expresion bloque
void Parser::sentenciaFor() {
    consumir(TokenType::FOR, "se esperaba 'for'");
    consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la variable de iteracion");
    consumir(TokenType::IN, "se esperaba 'in'");
    expresion();
    bloque();
}

// sentenciaReturn "return" expresion? ";"
void Parser::sentenciaReturn() {
    consumir(TokenType::RETURN, "se esperaba 'return'");
    if (!revisar(TokenType::SEMICOLON)) {
        expresion();
    }
    consumir(TokenType::SEMICOLON, "se esperaba ';' despues de return");
}

// expresiones (de menor a mayor precedencia)

// expresion asignacion
void Parser::expresion() { asignacion(); }

// asignacion logicoOr ("=" asignacion)?
void Parser::asignacion() {
    logicoOr();
    if (coincide(TokenType::ASSIGN)) {
        asignacion();
    }
}

// logicoOr logicoAnd ("||" logicoAnd)*
void Parser::logicoOr() {
    logicoAnd();
    while (coincide(TokenType::OR)) {
        logicoAnd();
    }
}

// logicoAnd igualdad ("&&" igualdad)*
void Parser::logicoAnd() {
    igualdad();
    while (coincide(TokenType::AND)) {
        igualdad();
    }
}

// igualdad comparacion (("==" | "!=") comparacion)*
void Parser::igualdad() {
    comparacion();
    while (revisar(TokenType::EQ) || revisar(TokenType::NEQ)) {
        avanzar();
        comparacion();
    }
}

// comparacion rango (("<" | "<=" | ">" | ">=") rango)*
void Parser::comparacion() {
    rango();
    while (revisar(TokenType::LT) || revisar(TokenType::LE) ||
           revisar(TokenType::GT) || revisar(TokenType::GE)) {
        avanzar();
        rango();
    }
}

// rango termino (".." termino)?
void Parser::rango() {
    termino();
    if (coincide(TokenType::RANGE)) {
        termino();
    }
}

// termino factor (("+" | "-") factor)*
void Parser::termino() {
    factor();
    while (revisar(TokenType::PLUS) || revisar(TokenType::MINUS)) {
        avanzar();
        factor();
    }
}

// factor unario (("*" | "/" | "%") unario)*
void Parser::factor() {
    unario();
    while (revisar(TokenType::TIMES) || revisar(TokenType::DIV) || revisar(TokenType::MOD)) {
        avanzar();
        unario();
    }
}

// unario ("!" | "-") unario | llamada
void Parser::unario() {
    if (revisar(TokenType::NOT) || revisar(TokenType::MINUS)) {
        avanzar();
        unario();
        return;
    }
    llamada();
}

// llamada primario ("(" argumentos ")")*
void Parser::llamada() {
    primario();
    while (coincide(TokenType::LPAREN)) {
        if (!revisar(TokenType::RPAREN)) {
            expresion();
            while (coincide(TokenType::COMMA)) {
                expresion();
            }
        }
        consumir(TokenType::RPAREN, "se esperaba ')' despues de los argumentos");
    }
}

// primario  integer, float, string, character, true, false, identificador, "(" expresion ")"
void Parser::primario() {
    if (coincide(TokenType::INTEGER)) return;
    if (coincide(TokenType::FLOAT)) return;
    if (coincide(TokenType::STRING)) return;
    if (coincide(TokenType::CHARACTER)) return;
    if (coincide(TokenType::TRUE)) return;
    if (coincide(TokenType::FALSE)) return;
    if (coincide(TokenType::IDENTIFICADOR)) return;
    if (coincide(TokenType::LPAREN)) {
        expresion();
        consumir(TokenType::RPAREN, "se esperaba ')' para cerrar la expresion");
        return;
    }
    error("se esperaba una expresion");
    throw ErrorSintactico{};
}