#include <vector>
#include "Range.h"

// Constructor para inicializar las variables
Range::Range(std::vector<int> texto, std::vector<int> X,
             std::vector<int> Y, std::vector<int> date)
        : texto_(texto), X_(X), Y_(Y), date_(date) {}

// Métodos para obtener los valores
std::vector<int> Range::getTexto() const {
    return texto_;
}

std::vector<int> Range::getX() const {
    return X_;
}

std::vector<int> Range::getY() const {
    return Y_;
}

std::vector<int> Range::getDate() const {
    return date_;
}

// Métodos para establecer los valores
void Range::setTexto(std::vector<int> texto) {
    texto_ = texto;
}

void Range::setX(std::vector<int> X) {
    X_ = X;
}

void Range::setY(std::vector<int> Y) {
    Y_ = Y;
}

void Range::setDate(std::vector<int> date) {
    date_ = date;
}

std::vector<int> Range::get(Range range, int dimension){

    switch(dimension){
        case 0:
            return range.getX();
        case 1:
            return range.getY();
        case 2:
            return range.getTexto();
        case 3:
            return range.getDate();
        default:
            return {};
    }
}

Range Range::set(Range range, int dimension, std::vector<int> v){

    switch(dimension){
        case 0:
            range.setX(v);
            break;
        case 1:
            range.setY(v);
            break;
        case 2:
            range.setTexto(v);
            break;
        case 3:
            range.setDate(v);
            break;
    }

    return range;
}


