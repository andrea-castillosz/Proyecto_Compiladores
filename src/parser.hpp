#pragma once

#include <vector>
#include <string>
#include "Token.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    void parsePrograma();
    bool huboErrores() const;

private:
    std::vector<Token> tokens;
    size_t pos;
    bool errores;

    Token actual() const; //se supone que devuelve el token actual
    bool coincide(TokenType tipo);//si el token actual coincide con el tipo esperado, avanza y devuelve true, si no devuelve false
    Token consumir(TokenType tipo, const std::string& mensaje);//lo mismo que coincide pero si no coincide lanza un error
    void error(const std::string& mensaje);//marca que hubo un error y muestra el mensaje
};