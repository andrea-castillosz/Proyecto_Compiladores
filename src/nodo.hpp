#pragma once

#include <string>
#include <vector>

struct Nodo {
    std::string etiqueta;      // qué construcción es tipo si es "funcion", "let", "binario"
    std::string valor;         // el lexema cuando aplica
    std::vector<Nodo*> hijos;

    Nodo(const std::string& etiqueta, const std::string& valor = "")
        : etiqueta(etiqueta), valor(valor) {}

    void agregar(Nodo* hijo) {
        if (hijo != nullptr) hijos.push_back(hijo);
    }
};

void imprimirArbol(const Nodo* nodo, int nivel = 0);