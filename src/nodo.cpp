#include "nodo.hpp"
#include <iostream>

void imprimirArbol(const Nodo* nodo, int nivel) {
    if (nodo == nullptr) return;

    // 1. indentación: dos espacios por nivel
    for (int i = 0; i < nivel; ++i) {
        std::cout << "  ";
    }
    // 2. imprimir etiqueta, y valor entre paréntesis si no está vacío
    std::cout << nodo->etiqueta;
    if (!nodo->valor.empty()) {
        std::cout << " (" << nodo->valor << ")";
    }
    std::cout << std::endl;
    // 3. recorrer hijos con nivel + 1
    for (const auto& hijo : nodo->hijos) {
        imprimirArbol(hijo, nivel + 1);
    }
}