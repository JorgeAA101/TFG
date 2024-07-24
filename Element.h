#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include "Index.h"


class Element {
public:
    // Constructor
    Element( int id_, double x, double y, long timestamp, const std::string& tags);

    // Getters
     int getID() const;
    double getX() const;
    double getY() const;
    long getTimestamp() const;
    std::string getTags() const;

    // Método para imprimir los detalles del elemento
    void printDetails() const;
    
    
    
unsigned int normalizeDate( int minEntero,  int maxEntero);


unsigned int normalizeText( int minEntero,  int maxEntero);


unsigned int normalizeX( int minEntero,  int maxEntero);


unsigned int normalizeY( int minEntero,  int maxEntero);

    

     int id_;
    double x_;
    double y_;
    long timestamp_;
    std::string tags_;
};

#endif // ELEMENT_H

