#include "parser.hpp"
#include <iostream>

/* excepcion interna usada solo para cuando consumir() o
primario() no encuentran lo que esperaban, se "corta" la regla actual
y se sube hasta declaracion()/bloque(), que atajan esto y llaman a
sincronizar() para seguir buscando más errores en vez de abortar todo.*/
namespace {
    struct ErrorSintactico {};
}

Parser::Parser(const std::vector<Token>& tokensEntrada, LogErrores& log): log(log) {
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
    log.agregar(TipoError::SINTACTICO,
                mensaje + " (se encontro '" + t.lexema + "')",
                t.linea, t.columna);
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
Nodo* Parser::parsePrograma() {
    Nodo* raiz = new Nodo("programa");
    while (!esFinDeTokens()) {
        try {
            raiz->agregar(declaracion());
        } catch (const ErrorSintactico&) {
            sincronizar();
        }
    }
    return raiz;
}

// declaracion declaracionFuncion | declaracionVariable | sentencia
Nodo* Parser::declaracion() {
    if (revisar(TokenType::FN)) return declaracionFuncion();
    if (revisar(TokenType::LET)) return declaracionVariable();
    return sentencia();
}

// declaracionFuncion "fn" identificador "(" parametros? ")" ("->" tipoDato)? bloque
Nodo* Parser::declaracionFuncion() {
    consumir(TokenType::FN, "se esperaba 'fn'");
    Token nombre = consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la funcion");
 
    Nodo* nodoFn = new Nodo("funcion", nombre.lexema);
 
    consumir(TokenType::LPAREN, "se esperaba '(' despues del nombre de la funcion");
    if (!revisar(TokenType::RPAREN)) {
        nodoFn->agregar(parametros());
    } else {
        nodoFn->agregar(new Nodo("parametros"));
    }
    consumir(TokenType::RPAREN, "se esperaba ')' despues de los parametros");
 
    if (coincide(TokenType::ARROW)) {
        nodoFn->agregar(tipoDato());
    }
 
    nodoFn->agregar(bloque());
    return nodoFn;
}

// parametros identificador ":" tipoDato ("," identificador ":" tipoDato)*
Nodo* Parser::parametros() {
    Nodo* nodoParams = new Nodo("parametros"); 
    Token nombre = consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre del parametro");
    consumir(TokenType::COLON, "se esperaba ':' despues del nombre del parametro");
    Nodo* param = new Nodo("parametro", nombre.lexema);
    param->agregar(tipoDato());
    nodoParams->agregar(param);
 
    while (coincide(TokenType::COMMA)) {
        Token otro = consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre del parametro");
        consumir(TokenType::COLON, "se esperaba ':' despues del nombre del parametro");
        Nodo* paramOtro = new Nodo("parametro", otro.lexema);
        paramOtro->agregar(tipoDato());
        nodoParams->agregar(paramOtro);
    }
 
    return nodoParams;
}

// tipoDato "i32" | "f64" | "bool" | "char" | "str"
Nodo* Parser::tipoDato() {
    if (revisar(TokenType::TIPO_I32) || revisar(TokenType::TIPO_F64) ||
        revisar(TokenType::TIPO_BOOL) || revisar(TokenType::TIPO_CHAR) ||
        revisar(TokenType::TIPO_STR)) {
        Token t = avanzar();
        return new Nodo("tipo", t.lexema);
    }
    error("se esperaba un tipo de dato (i32, f64, bool, char, str)");
    throw ErrorSintactico{};
}

// bloque "{" declaracion* "}"
Nodo* Parser::bloque() {
    consumir(TokenType::LBRACE, "se esperaba '{'");
    Nodo* nodoBloque = new Nodo("bloque"); 
    while (!revisar(TokenType::RBRACE) && !esFinDeTokens()) {
        try {
            nodoBloque->agregar(declaracion());
        } catch (const ErrorSintactico&) {
            sincronizar();
        }
    }
 
    consumir(TokenType::RBRACE, "se esperaba '}'");
    return nodoBloque;
}

// declaracionVariable "let" "mut"? IDENT (":" tipoDato)? "=" expresion ";"
Nodo* Parser::declaracionVariable() {
    consumir(TokenType::LET, "se esperaba 'let'");
    bool esMut = coincide(TokenType::MUT);
    Token nombre = consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la variable");
 
    Nodo* nodoLet = new Nodo(esMut ? "let_mut" : "let", nombre.lexema);
 
    if (coincide(TokenType::COLON)) {
        nodoLet->agregar(tipoDato());
    }
    if (coincide(TokenType::ASSIGN)) {
        nodoLet->agregar(expresion());
    }
    consumir(TokenType::SEMICOLON, "se esperaba ';' al final de la declaracion");
    return nodoLet;
}

// sentencia sentenciaIf, sentenciaWhile, sentenciaFor, sentenciaReturn, bloque, sentenciaExpresion
Nodo* Parser::sentencia() {
    if (revisar(TokenType::IF)) return sentenciaIf();
    if (revisar(TokenType::WHILE)) return sentenciaWhile();
    if (revisar(TokenType::FOR)) return sentenciaFor();
    if (revisar(TokenType::RETURN)) return sentenciaReturn();
    if (revisar(TokenType::LBRACE)) return bloque();
    return sentenciaExpresion();
}

// sentenciaExpresion expresion ";"
Nodo* Parser::sentenciaExpresion() {
    Nodo* expr = expresion();
    consumir(TokenType::SEMICOLON, "se esperaba ';' despues de la expresion");
    return expr;
}

// sentenciaIf "if" expresion bloque ("else" (sentenciaIf | bloque))?
Nodo* Parser::sentenciaIf() {
    consumir(TokenType::IF, "se esperaba 'if'");
    Nodo* nodoIf = new Nodo("si");
    nodoIf->agregar(expresion());
    nodoIf->agregar(bloque());
 
    if (coincide(TokenType::ELSE)) {
        if (revisar(TokenType::IF)) {
            nodoIf->agregar(sentenciaIf());
        } else {
            nodoIf->agregar(bloque());
        }
    }
    return nodoIf;
}

// sentenciaWhile "while" expresion bloque
Nodo* Parser::sentenciaWhile() {
    consumir(TokenType::WHILE, "se esperaba 'while'");
    Nodo* nodoWhile = new Nodo("mientras");
    nodoWhile->agregar(expresion());
    nodoWhile->agregar(bloque());
    return nodoWhile;
}

// sentenciaFor "for" identificador "in" expresion bloque
Nodo* Parser::sentenciaFor() {
    consumir(TokenType::FOR, "se esperaba 'for'");
    Token var = consumir(TokenType::IDENTIFICADOR, "se esperaba el nombre de la variable de iteracion");
    consumir(TokenType::IN, "se esperaba 'in'");
 
    Nodo* nodoFor = new Nodo("para", var.lexema);
    nodoFor->agregar(expresion());
    nodoFor->agregar(bloque());
    return nodoFor;
}

// sentenciaReturn "return" expresion? ";"
Nodo* Parser::sentenciaReturn() {
    consumir(TokenType::RETURN, "se esperaba 'return'");
    Nodo* nodoReturn = new Nodo("retornar");
    if (!revisar(TokenType::SEMICOLON)) {
        nodoReturn->agregar(expresion());
    }
    consumir(TokenType::SEMICOLON, "se esperaba ';' despues de return");
    return nodoReturn;
}

// expresiones (de menor a mayor precedencia)

// expresion asignacion
Nodo* Parser::expresion() { return asignacion(); }

// asignacion logicoOr ("=" asignacion)?
Nodo* Parser::asignacion() {
    Nodo* izq = logicoOr();
    if (coincide(TokenType::ASSIGN)) {
        Nodo* der = asignacion();
        Nodo* nodo = new Nodo("=");
        nodo->agregar(izq);
        nodo->agregar(der);
        return nodo;
    }
    return izq;
}

// logicoOr logicoAnd ("||" logicoAnd)*
Nodo* Parser::logicoOr() {
    Nodo* izq = logicoAnd();
    while (coincide(TokenType::OR)) {
        Nodo* der = logicoAnd();
        Nodo* nodo = new Nodo("||");
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// logicoAnd igualdad ("&&" igualdad)*
Nodo* Parser::logicoAnd() {
    Nodo* izq = igualdad();
    while (coincide(TokenType::AND)) {
        Nodo* der = igualdad();
        Nodo* nodo = new Nodo("&&");
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// igualdad comparacion (("==" | "!=") comparacion)*
Nodo* Parser::igualdad() {
    Nodo* izq = comparacion();
    while (revisar(TokenType::EQ) || revisar(TokenType::NEQ)) {
        Token op = avanzar();
        Nodo* der = comparacion();
        Nodo* nodo = new Nodo(op.lexema);
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// comparacion rango (("<" | "<=" | ">" | ">=") rango)*
Nodo* Parser::comparacion() {
    Nodo* izq = rango();
    while (revisar(TokenType::LT) || revisar(TokenType::LE) ||
           revisar(TokenType::GT) || revisar(TokenType::GE)) {
        Token op = avanzar();
        Nodo* der = rango();
        Nodo* nodo = new Nodo(op.lexema);
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// rango termino (".." termino)?
Nodo* Parser::rango() {
    Nodo* izq = termino();
    if (coincide(TokenType::RANGE)) {
        Nodo* der = termino();
        Nodo* nodo = new Nodo("..");
        nodo->agregar(izq);
        nodo->agregar(der);
        return nodo;
    }
    return izq;
}

// termino factor (("+" | "-") factor)*
Nodo* Parser::termino() {
    Nodo* izq = factor();
    while (revisar(TokenType::PLUS) || revisar(TokenType::MINUS)) {
        Token op = avanzar();
        Nodo* der = factor();
        Nodo* nodo = new Nodo(op.lexema);
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// factor unario (("*" | "/" | "%") unario)*
Nodo* Parser::factor() {
    Nodo* izq = unario();
    while (revisar(TokenType::TIMES) || revisar(TokenType::DIV) || revisar(TokenType::MOD)) {
        Token op = avanzar();
        Nodo* der = unario();
        Nodo* nodo = new Nodo(op.lexema);
        nodo->agregar(izq);
        nodo->agregar(der);
        izq = nodo;
    }
    return izq;
}

// unario ("!" | "-") unario | llamada
Nodo* Parser::unario() {
    if (revisar(TokenType::NOT) || revisar(TokenType::MINUS)) {
        Token op = avanzar();
        Nodo* operando = unario();
        Nodo* nodo = new Nodo("unario_" + op.lexema);
        nodo->agregar(operando);
        return nodo;
    }
    return llamada();
}

// llamada primario ("(" argumentos ")")*
Nodo* Parser::llamada() {
    Nodo* expr = primario();
    while (coincide(TokenType::LPAREN)) {
        Nodo* nodoLlamada = new Nodo("llamada");
        nodoLlamada->agregar(expr);
 
        if (!revisar(TokenType::RPAREN)) {
            nodoLlamada->agregar(expresion());
            while (coincide(TokenType::COMMA)) {
                nodoLlamada->agregar(expresion());
            }
        }
        consumir(TokenType::RPAREN, "se esperaba ')' despues de los argumentos");
        expr = nodoLlamada;
    }
    return expr;
}

// primario  integer, float, string, character, true, false, identificador, "(" expresion ")"
Nodo* Parser::primario() {
    if (revisar(TokenType::INTEGER) || revisar(TokenType::FLOAT) ||
        revisar(TokenType::STRING) || revisar(TokenType::CHARACTER) ||
        revisar(TokenType::TRUE) || revisar(TokenType::FALSE)) {
        Token t = avanzar();
        return new Nodo("literal", t.lexema);
    }
 
    if (revisar(TokenType::IDENTIFICADOR)) {
        Token t = avanzar();
        return new Nodo("identificador", t.lexema);
    }
 
    if (coincide(TokenType::LPAREN)) {
        Nodo* expr = expresion();
        consumir(TokenType::RPAREN, "se esperaba ')' para cerrar la expresion");
        return expr;
    }
 
    error("se esperaba una expresion");
    throw ErrorSintactico{};
}