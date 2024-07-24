#include "Element.h"
#include <iostream>

// Constructor
Element::Element( int id, double x, double y, long timestamp, const std::string& tags)
    : id_(id),x_(x), y_(y), timestamp_(timestamp), tags_(tags) {}

// Getters
 int Element::getID() const {
    return id_;
}

double Element::getX() const {
    return x_;
}

double Element::getY() const {
    return y_;
}

long Element::getTimestamp() const {
    return timestamp_;
}

std::string Element::getTags() const {
    return tags_;
}

// Método para imprimir los detalles del elemento
void Element::printDetails() const {
    std::cout << "x: " << x_ << ", y: " << y_ << ", timestamp: " << timestamp_ << ", tags: " << tags_ << ", id_: " << id_ << std::endl; 
}



unsigned int Element::normalizeDate( int minEntero,  int maxEntero){
        //Index::convertirFechaHora(date_, "%d/%m/%Y %H:%M:%S")
        return Index::normalizarEntero(timestamp_, minEntero, maxEntero);
    
}

unsigned int Element::normalizeText( int minEntero,  int maxEntero){
        return Index::normalizarEntero(Index::keywordToBits(tags_,32),minEntero,maxEntero);
    
}

unsigned int Element::normalizeX( int minEntero,  int maxEntero){
   
        return Index::normalizarDouble(x_,minEntero,maxEntero);
    
}

unsigned int Element::normalizeY( int minEntero,  int maxEntero){
    
        return Index::normalizarDouble(y_,minEntero,maxEntero);
    
}


