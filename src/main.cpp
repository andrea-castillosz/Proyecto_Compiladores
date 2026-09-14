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

int main(int argc, char* argv[]) {
     if (argc < 2) {
        std::cerr << "Uso: ./compilador <archivo.rs>" << std::endl;
        return 1;
    }
    std::string rutaArchivo = argv[1];
    
    //prueba leerArchivo
   
    std::string contenido = leerArchivo(rutaArchivo);
    if (!contenido.empty()) {
      //  std::cout << "Contenido del archivo:\n" << contenido << std::endl;
    } else {
       // std::cout << "El archivo está vacío o no se pudo leer." << std::endl;
    }

    //prueba Lexer
    Lexer lexer(contenido);
    Token token;
    do {
        token = lexer.siguienteToken();
        
        if (token.type == TokenType::ERROR) {
           std::cout << "Token: " << token.lexema
          << " - Tipo: " << tokenTypeToString(token.type)
          << " - Linea: " << token.linea
          << " - Col: " << token.columna << std::endl;
        }else{
            std::cout << "Token: " << token.lexema << " - Tipo: " << tokenTypeToString(token.type) << std::endl;
        }
    } while (token.type != TokenType::END_OF_FILE);

    return 0;
}

