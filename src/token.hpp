#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
using namespace std; 

// cada categoria corresponde a un estado de aceptacion de alguno de los automatas
enum class TokenType {
    // identificador 
    IDENTIFICADOR,

    // palabras reservadas 
    LET, FN, IF, ELSE, WHILE, FOR, RETURN, TRUE, FALSE,MUT,IN,

    // tipos primitivos 
    TIPO_I32, TIPO_F64, TIPO_BOOL, TIPO_CHAR, TIPO_STR,

    // numeros 
    INTEGER, FLOAT,

    // cadena y caracter 
    STRING, CHARACTER,

    // operadores de un caracter 
    PLUS, MINUS, TIMES, DIV, MOD,
    ASSIGN, LT, GT, NOT, AMP, 

    // operadores de dos caracteres
    ARROW, // ->
    AND, // &&
    OR, // ||
    EQ, // ==
    NEQ, // !=
    LE, // <=
    GE, // >=
    RANGE, // ..

    // delimitadores
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACKET, RBRACKET, COMMA, SEMICOLON, COLON,

    // especiales
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    string lexema;
    int linea;
    int columna;
};

// nombre del tipo de token para imprimir
string tokenTypeToString(TokenType type);

#endif
