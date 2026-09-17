#pragma once

#include <string>
#include <vector>

enum class TipoError { LEXICO, SINTACTICO };

struct ErrorCompilacion {
    TipoError tipo;
    std::string mensaje;
    int linea;
    int columna;
};

class LogErrores {
public:
    void agregar(TipoError tipo, const std::string& mensaje, int linea, int columna);
    bool vacio() const;
    void imprimir() const;

private:
    std::vector<ErrorCompilacion> errores;
};