#include "TablaSimbolos.hpp"
#include <iostream>

int TablaSimbolos::insertar(const std::string& lexema, const std::string& tipo) {
    
    simbolos.push_back({lexema, tipo});

    return simbolos.size() - 1;
}

void TablaSimbolos::imprimir() const {

    std::cout << "Tabla de Símbolos:" << std::endl;

    for (size_t i = 0; i < simbolos.size(); ++i) {
        std::string tipoMostrado = simbolos[i].tipo.empty() ? "-" : simbolos[i].tipo;
        std::cout << "Índice: " << i << ", Lexema: " << simbolos[i].lexema << ", Tipo: " << tipoMostrado << std::endl;
    }
}

void TablaSimbolos::actualizarTipo(int indice, const std::string& tipo) {
    if (indice >= 0 && indice < static_cast<int>(simbolos.size())) {
        simbolos[indice].tipo = tipo;
    }
}