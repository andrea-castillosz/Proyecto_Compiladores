#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include "token.hpp"
using namespace std;

class Lexer {
public:
    Lexer(const string& entrada);
    Token siguienteToken();

private:
    string entrada;
    size_t pos;
    int linea;
    int columna;

    // utilidades de recorrido
    char verActual() const;
    char verSiguiente() const;
    char avanzar();
    bool coincide(char esperado);
    void saltarBlancos();

    Token construirToken(TokenType tipo, const std::string& lexema, int lineaInicio, int columnaInicio);

    // un metodo por automata documentado en el informe
    Token identificador(int lineaInicio, int columnaInicio);
    Token numero(int lineaInicio, int columnaInicio);
    Token cadena(int lineaInicio, int columnaInicio);
    Token caracter(int lineaInicio, int columnaInicio);
    void comentarioDeLinea();
    void comentarioDeBloque();

    bool esDigito(char c) const;
    bool esLetra(char c) const;
    bool esLetraODigito(char c) const;
    bool esEscapeValido(char c) const;
};

#endif