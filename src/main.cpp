#include <iostream>
#include "PalabrasReservadas.hpp"
#include "lexer.hpp"
#include <fstream>
#include <sstream>
#include <vector>

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
    std::vector<Token> tokens;
    Lexer lexer(contenido);
    Token token;
    do {
        token = lexer.siguienteToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);//nvm ahora vamos a guardar los tokens en un vector para que sea mas facil para el parser
    for (const auto& t : tokens) {
        std::cout << "Token: " << tokenTypeToString(t.type) << ", Lexema: '" << t.lexema << "', Linea: " << t.linea << ", Columna: " << t.columna << std::endl;
    }

    return 0;
}

