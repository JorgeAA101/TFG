#ifndef RANGE_H
#define RANGE_H

#include <iostream>
#include <utility> // Para std::pair

class Range {
public:
    // Constructor para inicializar las variables
    Range(std::vector<int> texto, std::vector<int> X,
          std::vector<int> Y, std::vector<int> date);

    // Métodos para obtener los valores
    std::vector<int> getTexto() const;
    std::vector<int> getX() const;
    std::vector<int> getY() const;
    std::vector<int> getDate() const;


    // Métodos para establecer los valores
    void setTexto(std::vector<int> texto);
    void setX(std::vector<int> X);
    void setY(std::vector<int> Y);
    void setDate(std::vector<int> date);


    std::vector<int> get(Range range, int dimension);
    Range set(Range range, int dimension, std::vector<int> v);



private:
    std::vector<int> texto_;
    std::vector<int> X_;
    std::vector<int> Y_;
    std::vector<int> date_;
};




#endif // RANGE_H
