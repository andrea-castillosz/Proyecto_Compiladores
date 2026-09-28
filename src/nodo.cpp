#include "nodo.hpp"
#include <iostream>

static void imprimirNodo(const Nodo* nodo, const std::string& prefijo, bool esUltimo, bool esRaiz) {
    if (nodo == nullptr) return;

    if (esRaiz) {
        std::cout << nodo->etiqueta;
    } else {
        std::cout << prefijo << (esUltimo ? "\u2514\u2500 " : "\u251C\u2500 ") << nodo->etiqueta;
    }

    if (!nodo->valor.empty()) {
        std::cout << " (" << nodo->valor << ")";
    }
    std::cout << std::endl;

    std::string prefijoHijos;
    if (!esRaiz) {
        prefijoHijos = prefijo + (esUltimo ? "   " : "\u2502  ");
    }

    for (size_t i = 0; i < nodo->hijos.size(); ++i) {
        bool ultimo = (i == nodo->hijos.size() - 1);
        imprimirNodo(nodo->hijos[i], prefijoHijos, ultimo, false);
    }
}

void imprimirArbol(const Nodo* nodo, int nivel) {
    imprimirNodo(nodo, "", true, true);
}