CXX = g++
CXXFLAGS = -std=c++17 -Wall
FUENTES = main.cpp lexer.cpp token.cpp parser.cpp nodo.cpp logerrores.cpp tablasimbolos.cpp
EJECUTABLE = compilador

$(EJECUTABLE): $(FUENTES)
	$(CXX) $(CXXFLAGS) $(FUENTES) -o $(EJECUTABLE)

clean:
	rm -f $(EJECUTABLE)