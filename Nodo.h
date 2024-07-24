#ifndef NODO_H
#define NODO_H

#include <iostream>
#include <vector>

class STILTNode {
public:
    int id;
    unsigned int path;
    STILTNode* leftChild;
    STILTNode* rightChild;

public:
    STILTNode(int nodeId = 0, int nodePath = 0);

    // Getters
    int getId() const;
    int getPath() const;
    STILTNode* getLeftChild() const;
    STILTNode* getRightChild() const;

    /*array keys*/

    // Setters
    void setId(int newId);
    void setPath(int newPath);

    // Métodos adicionales
    void stilt_insert(int key, int id,STILTNode* root);
    void print_all_nodes() const;
    void print_nodes(const STILTNode* node, int entero) const;
    int countLeadingZeros(unsigned int num);
    unsigned int calcularRaiz(unsigned int tam, unsigned int nodoPath);
    unsigned int calcularNodo(unsigned int tam, unsigned int nodoPath);
    unsigned int getBit(unsigned int num, int posicion);
    STILTNode* pick_edge2(const STILTNode* node, unsigned int path, int acumulador);
    bool desplazarDerecha(unsigned int raiz ,unsigned int path, int saltos);

    int length;
};

// Funciones auxiliares
unsigned int obtenerParteComun(unsigned int num1, unsigned int num2);
unsigned int obtenerNOParteComun(unsigned int num1, unsigned int num2);

#endif // NODO_H
