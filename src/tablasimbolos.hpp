#pragma once

#include <string>
#include <vector>

struct Simbolo {
    std::string lexema;
    std::string tipo;   // vacio si no se declaro explícitamente
};

class TablaSimbolos {
public:
    int insertar(const std::string& lexema, const std::string& tipo);
    void imprimir() const;

private:
    std::vector<Simbolo> simbolos;
};