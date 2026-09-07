
#include <string>
#include "token.hpp"
using namespace std;
class Lexer {
public:
    Lexer(const std::string& entrada);
    Token siguienteToken();

private:
    std::string entrada;
    size_t pos;
    int linea;
    int columna;
};
