#include "Nodo.h"
#include <iostream>
#include <cmath>
#include <climits>
#include <iostream>
#include <bitset>

// Implementación del constructor
STILTNode::STILTNode(int nodeId, int nodePath) : id(nodeId), path(nodePath), leftChild(nullptr), rightChild(nullptr) {}

int STILTNode::getPath() const {
    return path;
}

STILTNode* STILTNode::getLeftChild() const {
    return leftChild;
}

STILTNode* STILTNode::getRightChild() const {
    return rightChild;
}

void STILTNode::setPath(int newPath) {
    path = newPath;
}

// Implementación de stilt_insert
void STILTNode::stilt_insert(int key, int id, STILTNode* node) {
    STILTNode* edge = node->pick_edge2(node,key, 0);

    int path = key;
    //if(path == 1509196050)
    //    std::cout << "Hola " <<  std::endl;
    unsigned int acumulador = 0;
    int leadingZeros = 0;
    // std::cout << " leadingZeros ->                -> " << edge << ", Path:    " << node->id << std::endl;

    do {

        if (edge == nullptr) {
            /* Esto seríoa put edge */
            STILTNode *hoja = new STILTNode();
            hoja->setPath(path);
            hoja->length = 32 - leadingZeros;
            hoja->id.push_back(id);
            // Imprimir información sobre el nodo actual
            // std::cout << "Node ID: " << node->id << ", Path: " << path << " Tamano: " << hoja->length << std::endl;

            if(getBit(path, hoja->length-1)==1){
                node->rightChild = hoja;
            }
            else{
                node->leftChild= hoja;
            }

            break;
        }
        //else if(comprobarCamino2(path, edge->path))
        if(desplazarDerecha(edge->getPath() ,path,(acumulador + edge->length)))
        {
            acumulador = acumulador + edge->length;
            path = path - edge->path;
            if(path == 0){
              edge->id.push_back(id);
              return;
            }
            else{
              edge = edge->pick_edge2(edge, path, acumulador);
            }                  
        }
        else
        {
            /* edge */
            leadingZeros = countLeadingZeros(edge->getPath() ^ path);
            int oldLength = edge->length;
            edge->length = leadingZeros-acumulador;

            STILTNode* hojaD = new STILTNode();

            hojaD->leftChild = edge->leftChild;
            hojaD->rightChild = edge->rightChild;
            hojaD->path = calcularNodo( leadingZeros,edge->path);
            hojaD->length = oldLength - edge->length;
            hojaD->id = edge->id;
            edge->id.clear();
            //std::cout << "Node xxxcxID: " << path << ", Path: " << hojaD->path << " Tamano: " << 32- (leadingZeros+1) << std::endl;

            if(getBit(hojaD->path, 32- (leadingZeros+1))==1){
                //std::cout << "Node xxxDxID: " << path << ", Path: " << hojaD->path << " Tamano: " << 32- (leadingZeros+1) << std::endl;

                edge->rightChild = hojaD;
                edge->leftChild = nullptr;

                edge->path = calcularRaiz(leadingZeros, edge->getPath());

                path = calcularNodo( leadingZeros,path);

                //std::cout << "RAIZ xxxIID: " << node->id << ", Path: " <<  edge->path<< " Tamano: " << 32- (leadingZeros+1) << std::endl;
                //std::cout << "1IF xxxIID: " << node->id << ", Path: " << path << " Tamano: " << 32- (leadingZeros+1) << std::endl;

                node = edge;
                edge = edge->leftChild;
            }
            else{
                edge->leftChild= hojaD;
                edge->rightChild = nullptr;
                //std::cout << "Node xxxIID: " << node->id << ", Path: " << hojaD->path << " Tamano: " << 32- (leadingZeros+1) << std::endl;

                edge->path = calcularRaiz(leadingZeros, edge->getPath());
                path = calcularNodo( leadingZeros,path);

                //std::cout << "RAIZ xxxIID: " << node->id << ", Path: " <<  edge->path<< " Tamano: " << 32- (leadingZeros+1) << std::endl;
                //std::cout << "1IF xxxIID: " << node->id << ", Path: " << path << " Tamano: " << 32- (leadingZeros+1) << std::endl;
                node = edge;
                edge = edge->rightChild;
            }

            /* Añadir el tamaño */

            //edge->path = calcularRaiz(leadingZeros, edge->getPath());
            //path = calcularNodo( leadingZeros,path);
            //node = edge;
            //edge = pick_edge2( edge, path,  acumulador);
        }
    }while (path);
}
//STILTNode* STILTNode::split(const STILTNode* node, unsigned int path){



// Implementación de la función print_nodes
void STILTNode::print_nodes(const STILTNode* node, int entero) const {
    if(entero == -1){
        std::cout << "raiz"<< std::endl ;
    }
    else if(entero == 0){
        std::cout << "izq"<< std::endl ;
    }
    else{
        std::cout << "der"<< std::endl ;
    }
    if (node) {

        // Imprimir información sobre el nodo actual
        std::cout << "Node ID: " << node->id[0] << ", Path: " << node->path << " Tamano: " << node->length << " binario:" << std::bitset<32>(node->path).to_string() << std::endl;

        // Llamar recursivamente a la función para imprimir los nodos hijos
        print_nodes(node->leftChild, 0);
        print_nodes(node->rightChild, 1);
    }
    else{
        std::cout << "  --Nodo vacio--" << std::endl;
    }
}

// Implementación de print_all_nodes
void STILTNode::print_all_nodes() const {
    std::cout << "Impresion Arbol " << std::endl;

    print_nodes(this, -1);
}


// Función para contar los ceros principales en un entero sin signo
int STILTNode::countLeadingZeros(unsigned int num) {
    /*int count = 0;
    while ((num & 0x80000000) == 0 && count < 32) {
        ++count;
        num <<= 1;
    }
    return count;*/
    // Initialize count to 0
    int count = 0;

    // Bucle
    for (int i = 31; i >= 0; --i) {
        // Cuento los ceros
        if (((num >> i) & 1) == 0) {

            ++count;
        } else {
            // Rompo el lazo cuando llego al primer 1
            break;
        }
    }

    return count;
}

unsigned int STILTNode::calcularRaiz(unsigned int tam, unsigned int nodoPath){
    nodoPath = nodoPath >> (32 - tam);
    return nodoPath << (32 - tam);
}

unsigned int STILTNode::calcularNodo(unsigned int tam, unsigned int nodoPath) {

    unsigned int mask = 0xFFFFFFFF;
    mask = mask >> tam;
    unsigned int rand =nodoPath & mask;
    return nodoPath & mask;
}



// Implementación de pick_edge
STILTNode* STILTNode::pick_edge2(const STILTNode* node, unsigned int path, int acumulador){

    if(getBit(path, 32- (acumulador+1))==1){
        return node->rightChild;
    }
    else{
        return node->leftChild;
    }

}

unsigned int STILTNode::getBit(unsigned int num, int posicion) {
    // Máscara para aislar el bit en la posición especificada
    unsigned int mascara = 1u << posicion;

    // Realiza una operación AND entre el número y la máscara para aislar el bit en la posición especificada
    unsigned int bit = (num & mascara) >> posicion;

    return bit;
}

bool STILTNode::desplazarDerecha(unsigned int raiz ,unsigned int path, int saltos) {
    saltos = 32-saltos;
    unsigned int desp = path >> saltos;
    unsigned int raiz2 = raiz >> saltos;
    return  desp == raiz2;
}


