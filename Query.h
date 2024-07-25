#ifndef QUERY_H
#define QUERY_H

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <limits>

class Query {
public:
    Query();
    // Constructor para inicializar los campos
    Query(const std::vector<std::string> text, const std::vector<double> latitud,
          const std::vector<double> longitud, const std::vector<unsigned int> date);

    // Método que obtiene el máximo y el mínimo de una lista de enteros
    std::pair<int, int> findMinMax(const std::vector<int> numbers);

    // Métodos para obtener los valores de los campos
    std::vector<std::string> getText() const;
    std::vector<double> getX() const;
    std::vector<double> getY() const;
    std::vector<unsigned int> getDate() const;
    bool getMatch() const;

    std::vector<unsigned int> getDateNorm() const;
    std::vector<unsigned int> getXNorm() const;
    std::vector<unsigned int> getYNorm() const;
    std::vector<unsigned int> getTextoNorm() const;

    // Métodos para establecer los valores de los campos
    void setText(const std::vector<std::string>& text);
    void setX(const std::vector<double>& latitud);
    void setY(const std::vector<double>& longitud);
    void setDate(const std::vector<unsigned int>& date);
    void setTextoNorm(const std::vector<unsigned int>& textoNorm);
    void setXNorm(const std::vector<unsigned int>& latitudNorm);
    void setYNorm(const std::vector<unsigned int>& longitudNorm);
    void setDateNorm(const std::vector<unsigned int>& dateNorm);

    std::vector<unsigned int> get(Query query, int dimension);
    void setMatch(bool match);




    /*
    // Métodos adicionales para obtener rangos
    std::pair<int, int> getLatitudRange() const;
    std::pair<int, int> getLongitudRange() const;
    std::pair<int, int> getDateRange() const;
    std::pair<int, int> getTextMinMax() const;
     */

     void normalizeDate( unsigned int minEntero, unsigned int maxEntero);
     void normalizeText( unsigned int minEntero, unsigned int maxEntero);
     void normalizeX( double minEntero,  double maxEntero);
     void normalizeY( double minEntero,  double maxEntero);


    std::vector<std::string> text;
    std::vector<unsigned int> textNorm;
    std::vector<double> x;
    std::vector<unsigned int> xNorm;
    std::vector<double> y;
    std::vector<unsigned int> yNorm;
    std::vector<unsigned int> date_;
    std::vector<unsigned int> dateNorm;
    bool match;
};

#endif // QUERY_H
