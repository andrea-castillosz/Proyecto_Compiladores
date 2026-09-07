#include "Token.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFICADOR: return "IDENTIFICADOR";
        case TokenType::LET:           return "LET";
        case TokenType::FN:            return "FN";
        case TokenType::IF:            return "IF";
        case TokenType::ELSE:          return "ELSE";
        case TokenType::WHILE:         return "WHILE";
        case TokenType::FOR:           return "FOR";
        case TokenType::RETURN:        return "RETURN";
        case TokenType::TRUE:          return "TRUE";
        case TokenType::FALSE:         return "FALSE";
        case TokenType::MUT:           return "MUT";
        case TokenType::IN:            return "IN";
        case TokenType::TIPO_I32:      return "TIPO_I32";
        case TokenType::TIPO_F64:      return "TIPO_F64";

        case TokenType::TIPO_BOOL:     return "TIPO_BOOL";
        case TokenType::TIPO_CHAR:     return "TIPO_CHAR";
        case TokenType::TIPO_STR:      return "TIPO_STR";
        case TokenType::INTEGER:       return "INTEGER";
        case TokenType::FLOAT:         return "FLOAT";
        case TokenType::STRING:        return "STRING";
        case TokenType::CHARACTER:     return "CHARACTER";
        case TokenType::PLUS:          return "PLUS";
        case TokenType::MINUS:         return "MINUS";
        case TokenType::TIMES:         return "TIMES";
        case TokenType::DIV:           return "DIV";
        case TokenType::MOD:           return "MOD";
        
        case TokenType::ASSIGN:        return "ASSIGN";
        case TokenType::LT:            return "LT";
        case TokenType::GT:            return "GT";
        case TokenType::NOT:           return "NOT";
        case TokenType::AMP:           return "AMP";
        case TokenType::ARROW:         return "ARROW";
        case TokenType::AND:           return "AND";
        case TokenType::OR:            return "OR";
        case TokenType::EQ:            return "EQ";
        case TokenType::NEQ:           return "NEQ";
        case TokenType::LE:            return "LE";
        case TokenType::GE:            return "GE";
        case TokenType::RANGE:         return "RANGE";
        case TokenType::LBRACE:        return "LBRACE";
        case TokenType::RBRACE:        return "RBRACE";
        case TokenType::LPAREN:        return "LPAREN";
        case TokenType::RPAREN:        return "RPAREN";
        case TokenType::LBRACKET:      return "LBRACKET";
        case TokenType::RBRACKET:      return "RBRACKET";
        case TokenType::COMMA:         return "COMMA";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::COLON:         return "COLON";
        case TokenType::END_OF_FILE:   return "END_OF_FILE";
        case TokenType::ERROR:         return "ERROR";
        
        default:                       return "DESCONOCIDO";
    }
}