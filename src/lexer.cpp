
#include "lexer.hpp"

Lexer::Lexer(const std::string& entrada) {
    this->entrada = entrada;
    pos = 0;
    linea = 1;
    columna = 1;
}
bool esBlanco(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
Token Lexer::siguienteToken() {
    // saltar blancos
    while (pos < entrada.size() && esBlanco(entrada[pos])) {
        if (entrada[pos] == '\n') { linea++; columna = 1; }
        else { columna++; }
        pos++;
    }

    if (pos >= entrada.size()) {
        return {TokenType::END_OF_FILE, "", linea, columna};
    }

    //  automata


    char c = entrada[pos];
    pos++;
    columna++;
    return {TokenType::ERROR, std::string(1, c), linea, columna};
}