#include <iostream>
#include "PalabrasReservadas.hpp"
#include "lexer.hpp"
#include <fstream>
#include <sstream>

std::string leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if(!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << ruta << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

int main() {
    // probando buscarPalabraReservada con "let" y con "x"
    std::string palabra1 = "let";
    std::string palabra2 = "x";
    TokenType tipo1 = buscarPalabraReservada(palabra1);
    TokenType tipo2 = buscarPalabraReservada(palabra2);
    if (tipo1 == TokenType::LET) {
        std::cout << palabra1 << " es una palabra reservada." << std::endl;
    } else {
        std::cout << palabra1 << " no es una palabra reservada." << std::endl;
    }
    if (tipo2 == TokenType::IDENTIFICADOR) {
        std::cout << palabra2 << " es un identificador." << std::endl;
    } else {
        std::cout << palabra2 << " es una palabra reservada." << std::endl;
    }
    

    //prueba leerArchivo
    std::string rutaArchivo = "pruebas/prueba1.rs"; 
    std::string contenido = leerArchivo(rutaArchivo);
    if (!contenido.empty()) {
        std::cout << "Contenido del archivo:\n" << contenido << std::endl;
    } else {
        std::cout << "El archivo está vacío o no se pudo leer." << std::endl;
    }

    //prueba Lexer
    Lexer lexer(contenido);
    Token token;
    do {
        token = lexer.siguienteToken();
        std::cout << "Token: " << token.lexema << " - Tipo: " << tokenTypeToString(token.type) << std::endl;
    } while (token.type != TokenType::END_OF_FILE);

    return 0;
}

