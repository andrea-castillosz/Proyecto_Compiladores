
#include "logerrores.hpp"
#include <iostream>



void LogErrores::agregar(TipoError tipo,const std::string& mensaje, int linea, int columna) {
    errores.push_back({tipo,mensaje, linea, columna});
}

bool LogErrores::vacio() const {
    return errores.empty();
}

void LogErrores::imprimir() const {
    for (const auto& error : errores) {
    std::string etiqueta = (error.tipo == TipoError::LEXICO) ? "lexico" : "sintactico";
    std::cerr << "[linea " << error.linea << ", columna " << error.columna
              << "] Error " << etiqueta << ": " << error.mensaje << std::endl;
    }
}