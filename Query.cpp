#include "Query.h"
#include <limits>
#include "Range.h"
#include "Index.h"

// Constructor para inicializar los campos
Query::Query()
        : match(false) {}

Query::Query(const std::vector<std::string> text, const std::vector<double> x,
             const std::vector<double> y, const std::vector<unsigned int> date)
        : text(text), x(x), y(y), date_(date) {

}

// Método que obtiene el máximo y el mínimo de una lista de enteros
std::pair<int, int> Query::findMinMax(const std::vector<int> numbers) {
    if (numbers.empty()) {
        throw std::invalid_argument("La lista de enteros está vacía");
    }

    int minVal = std::numeric_limits<int>::max();
    int maxVal = std::numeric_limits<int>::min();

    for (int number : numbers) {
        if (number < minVal) {
            minVal = number;
        }
        if (number > maxVal) {
            maxVal = number;
        }
    }

    return std::make_pair(minVal, maxVal);
}

// Métodos para obtener los valores de los campos
std::vector<std::string> Query::getText() const {
    return text;
}

std::vector<double> Query::getX() const {
    return x;
}

std::vector<double> Query::getY() const {
    return y;
}

std::vector<unsigned int> Query::getDate() const {
    return date_;
}

std::vector<unsigned int> Query::getDateNorm() const {
    return dateNorm;
}

std::vector<unsigned int> Query::getXNorm() const {
    return xNorm;
}

std::vector<unsigned int> Query::getYNorm() const {
    return yNorm;
}
std::vector<unsigned int> Query::getTextoNorm() const {
    return textNorm;
}
bool Query::getMatch() const {
    return match;
}


std::vector<unsigned int> Query::get(Query query, int dimension){

    switch(dimension){
        case 0:
            return query.getXNorm();
        case 1:
            return query.getYNorm();
        case 2:
            return query.getTextoNorm();
        case 3:
            return query.getDateNorm();
        default:
            return  {};
    }
}




// Métodos para establecer los valores de los campos
void Query::setText(const std::vector<std::string>& t) {
    text = t;
}

void Query::setX(const std::vector<double>& l) {
    x = l;
}

void Query::setY(const std::vector<double>& l) {
    y = l;
}

void Query::setDate(const std::vector<unsigned int>& date) {
    date_ = date;
}

void Query::setTextoNorm(const std::vector<unsigned int>& tn) {
    textNorm = tn;
}

void Query::setXNorm(const std::vector<unsigned int>& xN) {
    xNorm = xN;
}

void Query::setYNorm(const std::vector<unsigned int>& yN){
    yNorm = yN;
}

void Query::setDateNorm(const std::vector<unsigned int>& dateN) {
    dateNorm = dateN;
}
void Query::setMatch(bool m) {
    match = m;
}

void Query::normalizeDate(unsigned int minEntero, unsigned int maxEntero){
    for(int i =0 ;i < date_.size(); i++){
        dateNorm.push_back(Index::normalizarEntero(date_[i], minEntero, maxEntero));
    }
}

void Query::normalizeText( unsigned int minEntero,  unsigned int maxEntero){
    for(int i =0 ;i < text.size(); i++){

        textNorm.push_back(Index::normalizarEntero(Index::keywordToBits(text[i],32),minEntero,maxEntero));
    }
}

void Query::normalizeX( double minEntero,  double maxEntero){
    for(int i =0 ;i < x.size(); i++){
        xNorm.push_back(Index::normalizarDouble(x[i],minEntero,maxEntero));
    }
}

void Query::normalizeY( double minEntero, double maxEntero){
    for(int i =0 ;i < y.size(); i++){
        yNorm.push_back(Index::normalizarDouble(y[i],minEntero,maxEntero));
    }
}



/*
// Métodos adicionales para obtener rangos
std::pair<int, int> Query::getLatitudRange() const {
    return findMinMax(std::vector<int>(latitud_.begin(), latitud_.end()));
}

std::pair<int, int> Query::getLongitudRange() const {
    return findMinMax(std::vector<int>(longitud_.begin(), longitud_.end()));
}

std::pair<int, int> Query::getDateRange() const {
    // Aquí deberías definir cómo calcular el rango para fechas si aplica.
    // Este es solo un ejemplo placeholder.
    std::vector<int> dateNumbers; // Asume que puedes convertir las fechas a enteros de alguna forma.
    return findMinMax(dateNumbers);
}

std::pair<int, int> Query::getTextMinMax() const {
    // Normalizar lista de textos, por ejemplo, por longitud de cadena o algún criterio aplicable
    std::vector<int> textLengths;
    for (const auto& text : text_) {
        textLengths.push_back(static_cast<int>(text.length()));
    }
    return findMinMax(textLengths);
}
*/
