#include "lexer.hpp"
#include "PalabrasReservadas.hpp"
#include "logerrores.hpp"

Lexer::Lexer(const std::string& entradaFuente, LogErrores& log) : log(log) {
    entrada = entradaFuente;
    pos = 0;
    linea = 1;
    columna = 1;
}
bool Lexer::esBlanco(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char Lexer::verActual() const {
    if (pos >= entrada.size()) return '\0';
    return entrada[pos];
}

char Lexer::verSiguiente() const {
    if (pos + 1 >= entrada.size()) return '\0';
    return entrada[pos + 1];
}

char Lexer::avanzar() {
    char c = entrada[pos];
    pos++;
    if (c == '\n') {
        linea++;
        columna = 1;
    } else {
        columna++;
    }
    return c;
}

bool Lexer::coincide(char esperado) {
    if (pos >= entrada.size() || entrada[pos] != esperado) return false;
    avanzar();
    return true;
}

void Lexer::saltarBlancos() {
    while (pos < entrada.size() && esBlanco(entrada[pos])) {
        avanzar();
    }
}

Token Lexer::construirToken(TokenType tipo, const std::string& lexema, int lineaInicio, int columnaInicio) {
    return Token{tipo, lexema, lineaInicio, columnaInicio};
}

bool Lexer::esDigito(char c) const { return c >= '0' && c <= '9'; }
bool Lexer::esLetra(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
bool Lexer::esLetraODigito(char c) const { return esLetra(c) || esDigito(c) || c == '_'; }
bool Lexer::esEscapeValido(char c) const {
    return c == 'n' || c == 't' || c == 'r' || c == '\\' || c == '"' || c == '\'' || c == '0';
}


Token Lexer::siguienteToken() {
    for (;;) {
        saltarBlancos();

        if (pos >= entrada.size()) {
            return construirToken(TokenType::END_OF_FILE, "", linea, columna);
        }

        int lineaInicio = linea;
        int columnaInicio = columna;
        size_t inicioLexema = pos;
        char c = avanzar();

        switch (c) {
            // delimitadores
            case '{': return construirToken(TokenType::LBRACE, "{", lineaInicio, columnaInicio);
            case '}': return construirToken(TokenType::RBRACE, "}", lineaInicio, columnaInicio);
            case '(': return construirToken(TokenType::LPAREN, "(", lineaInicio, columnaInicio);
            case ')': return construirToken(TokenType::RPAREN, ")", lineaInicio, columnaInicio);
            case '[': return construirToken(TokenType::LBRACKET, "[", lineaInicio, columnaInicio);
            case ']': return construirToken(TokenType::RBRACKET, "]", lineaInicio, columnaInicio);
            case ',': return construirToken(TokenType::COMMA, ",", lineaInicio, columnaInicio);
            case ';': return construirToken(TokenType::SEMICOLON, ";", lineaInicio, columnaInicio);
            case ':': return construirToken(TokenType::COLON, ":", lineaInicio, columnaInicio);

            // operadores de un car
            case '+': return construirToken(TokenType::PLUS, "+", lineaInicio, columnaInicio);
            case '*': return construirToken(TokenType::TIMES, "*", lineaInicio, columnaInicio);
            case '%': return construirToken(TokenType::MOD, "%", lineaInicio, columnaInicio);

            // operadores 
            case '-':
                if (coincide('>')) return construirToken(TokenType::ARROW, "->", lineaInicio, columnaInicio);
                return construirToken(TokenType::MINUS, "-", lineaInicio, columnaInicio);
            case '&':
                if (coincide('&')) return construirToken(TokenType::AND, "&&", lineaInicio, columnaInicio);
                return construirToken(TokenType::AMP, "&", lineaInicio, columnaInicio);
            case '|':
                if (coincide('|')) return construirToken(TokenType::OR, "||", lineaInicio, columnaInicio);
                return construirToken(TokenType::ERROR, "|", lineaInicio, columnaInicio);
            case '!':
                if (coincide('=')) return construirToken(TokenType::NEQ, "!=", lineaInicio, columnaInicio);
                return construirToken(TokenType::NOT, "!", lineaInicio, columnaInicio);
            case '<':
                if (coincide('=')) return construirToken(TokenType::LE, "<=", lineaInicio, columnaInicio);
                return construirToken(TokenType::LT, "<", lineaInicio, columnaInicio);
            case '>':
                if (coincide('=')) return construirToken(TokenType::GE, ">=", lineaInicio, columnaInicio);
                return construirToken(TokenType::GT, ">", lineaInicio, columnaInicio);
            case '=':
                if (coincide('=')) return construirToken(TokenType::EQ, "==", lineaInicio, columnaInicio);
                return construirToken(TokenType::ASSIGN, "=", lineaInicio, columnaInicio);
            case '.':
                if (coincide('.')) return construirToken(TokenType::RANGE, "..", lineaInicio, columnaInicio);
                return construirToken(TokenType::ERROR, ".", lineaInicio, columnaInicio);

            
            case '/':
                if (coincide('/')) {
                    comentarioDeLinea();
                    continue; 
                }
                if (coincide('*')) { // si no cerro bien devuelve un token de error si no cae en el continue
                    if (!comentarioDeBloque()) {
                        return construirToken(TokenType::ERROR, "/*", lineaInicio, columnaInicio);
                    }
                    continue;
                }
                return construirToken(TokenType::DIV, "/", lineaInicio, columnaInicio);

            case '"': return cadena(lineaInicio, columnaInicio);
            case '\'': return caracter(lineaInicio, columnaInicio);

            default:
                if (esDigito(c)) {
                    pos = inicioLexema; columna = columnaInicio; // reposiciona
                    return numero(lineaInicio, columnaInicio);
                }
                if (esLetra(c) || c == '_') {
                    pos = inicioLexema; columna = columnaInicio; 
                    return identificador(lineaInicio, columnaInicio);
                }
                return construirToken(TokenType::ERROR, std::string(1, c), lineaInicio, columnaInicio);
        }
    }
}

// ---------- IDENTIFICADOR ----------
Token Lexer::identificador(int lineaInicio, int columnaInicio) {
    size_t inicio = pos;
    avanzar(); // primer carácter: letra o '_' (ya validado por el llamador)
    while (esLetraODigito(verActual())) avanzar();

    std::string lexema = entrada.substr(inicio, pos - inicio);
    TokenType tipo = buscarPalabraReservada(lexema);
    return construirToken(tipo, lexema, lineaInicio, columnaInicio);
}

// ---------- NUMERO ----------
Token Lexer::numero(int lineaInicio, int columnaInicio) {
    size_t inicio = pos;
    while (esDigito(verActual())) avanzar();

    bool esFlotante = false;
    if (verActual() == '.' && esDigito(verSiguiente())) {
        esFlotante = true;
        avanzar(); 
        while (esDigito(verActual())) avanzar();
    }

    std::string lexema = entrada.substr(inicio, pos - inicio);
    return construirToken(esFlotante ? TokenType::FLOAT : TokenType::INTEGER, lexema, lineaInicio, columnaInicio);
}

// ---------- CADENA ----------
Token Lexer::cadena(int lineaInicio, int columnaInicio) {
    size_t inicio = pos; 

    while (verActual() != '"' && verActual() != '\n' && pos < entrada.size()) {
        if (verActual() == '\\') {
            avanzar();
            if (esEscapeValido(verActual())) {
                avanzar();
            } else {
                std::string lexema = entrada.substr(inicio - 1, pos - inicio + 1);
                return construirToken(TokenType::ERROR, lexema, lineaInicio, columnaInicio);
            }
        } else {
            avanzar();
        }
    }

    if (verActual() != '"') {
        std::string lexema = entrada.substr(inicio - 1, pos - inicio + 1);
        return construirToken(TokenType::ERROR, lexema, lineaInicio, columnaInicio);
    }

    std::string contenido = entrada.substr(inicio, pos - inicio);
    avanzar(); // consume la comilla de cierre
    return construirToken(TokenType::STRING, contenido, lineaInicio, columnaInicio);
}

// ---------- CARACTER ----------
Token Lexer::caracter(int lineaInicio, int columnaInicio) {
    size_t inicio = pos; 

    if (pos >= entrada.size() || verActual() == '\'' || verActual() == '\n') {
        std::string lexema = entrada.substr(inicio - 1, pos - inicio + 1);
        return construirToken(TokenType::ERROR, lexema, lineaInicio, columnaInicio);
    }

    if (verActual() == '\\') {
        avanzar();
        if (esEscapeValido(verActual())) {
            avanzar();
        } else {
            std::string lexema = entrada.substr(inicio - 1, pos - inicio + 1);
            return construirToken(TokenType::ERROR, lexema, lineaInicio, columnaInicio);
        }
    } else {
        avanzar();
    }

    if (verActual() != '\'') {
        std::string lexema = entrada.substr(inicio - 1, pos - inicio + 1);
        return construirToken(TokenType::ERROR, lexema, lineaInicio, columnaInicio);
    }

    std::string contenido = entrada.substr(inicio, pos - inicio);
    avanzar(); // consume la comilla de cierre
    return construirToken(TokenType::CHARACTER, contenido, lineaInicio, columnaInicio);
}

// ---------- comentario linea ----------
void Lexer::comentarioDeLinea() {
    while (verActual() != '\n' && pos < entrada.size()) avanzar();
    // el '\n' se deja para que lo consuma saltarBlancos() en la próxima vuelta
}

// ---------- comentario bloque ----------
bool Lexer::comentarioDeBloque() { //se cambio a bool para indicar si se cerro correctamente
    while (pos < entrada.size()) {
        if (verActual() == '*' && verSiguiente() == '/') {
            avanzar(); // '*'
            avanzar(); // '/'
            return true;
        }
        avanzar();
    }
    return false;
}