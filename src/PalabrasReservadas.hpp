#ifndef PALABRAS_RESERVADAS_HPP
#define PALABRAS_RESERVADAS_HPP

#include <string>
#include <unordered_map>
#include "Token.hpp"

inline TokenType buscarPalabraReservada(const std::string& lexema) {
    static const std::unordered_map<std::string, TokenType> tabla = {
        {"let", TokenType::LET},
        {"fn", TokenType::FN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"mut", TokenType::MUT},
        {"in", TokenType::IN},
        {"i32",  TokenType::TIPO_I32},
        {"f64",  TokenType::TIPO_F64},
        {"bool", TokenType::TIPO_BOOL},
        {"char", TokenType::TIPO_CHAR},
        {"str",  TokenType::TIPO_STR}
    };

  
    auto it = tabla.find(lexema);
    if (it != tabla.end()) {
        return it->second;
    }
    return TokenType::IDENTIFICADOR;
}

#endif