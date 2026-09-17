#pragma once

#include <vector>
#include <string>
#include "token.hpp"
#include "nodo.hpp"
#include "logerrores.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens,LogErrores& log);
    Nodo* parsePrograma();
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
    Nodo* declaracion();
    Nodo* declaracionFuncion();
    Nodo* parametros();
    Nodo* tipoDato();
    Nodo* bloque();
    Nodo* declaracionVariable();

    Nodo* sentencia();
    Nodo* sentenciaExpresion();
    Nodo* sentenciaIf();
    Nodo* sentenciaWhile();
    Nodo* sentenciaFor();
    Nodo* sentenciaReturn();
 
    // expresiones, de menor a mayor precedencia
    Nodo* expresion();
    Nodo* asignacion();
    Nodo* logicoOr();
    Nodo* logicoAnd();
    Nodo* igualdad();
    Nodo* comparacion();
    Nodo* rango();
    Nodo* termino();
    Nodo* factor();
    Nodo* unario();
    Nodo* llamada();
    Nodo* primario();
};