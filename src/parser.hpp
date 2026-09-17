#pragma once

#include <vector>
#include <string>
#include "token.hpp"
#include "nodo.hpp"
#include "logerrores.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens,LogErrores& log);
    void parsePrograma();
    bool huboErrores() const;

private:
    LogErrores& log;
    std::vector<Token> tokens;
    size_t pos;
    bool errores;

    // utilidades de recorrido
    Token actual() const;
    Token anterior() const;
    bool esFinDeTokens() const;
    bool revisar(TokenType tipo) const; // mira el token actual sin consumir
    bool coincide(TokenType tipo); // si coincide, avanza y devuelve true
    Token avanzar(); // consume el token actual y lo devuelve
    Token consumir(TokenType tipo, const std::string& mensaje); // como coincide, pero lanza error si no coincide
    void error(const std::string& mensaje);
    void sincronizar(); // modo pánico: avanza hasta el proximo punto seguro tras un error

    // reglas de la gramatica (una función por regla)
    void declaracion();
    void declaracionFuncion();
    void parametros();
    void tipoDato();
    void bloque();
    void declaracionVariable();

    void sentencia();
    void sentenciaExpresion();
    void sentenciaIf();
    void sentenciaWhile();
    void sentenciaFor();
    void sentenciaReturn();

    // expresiones, de menor a mayor precedencia
    void expresion();
    void asignacion();
    void logicoOr();
    void logicoAnd();
    void igualdad();
    void comparacion();
    void rango();
    void termino();
    void factor();
    void unario();
    void llamada();
    void primario();
};