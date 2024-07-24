#include "Index.h"
#include <iostream>
#include <vector>
#include <bitset>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <vector>


#include <cmath>
#include <iomanip>
#include <algorithm>

// Constructor
Index::Index(const std::string& inputString, size_t numBits)
        : inputString_(inputString), numBits_(numBits) {}

// Función para convertir bits a una cadena según las reglas dadas
std::string Index::bitsToKeyword(int bits, size_t target_length) {
    std::string result;

    // Recorrer los bits y convertirlos de vuelta a caracteres
    for (size_t i = 0; i < target_length; ++i) {
        int charBits = (bits >> ((target_length - 1 - i) * 5)) & 0x1F;

        if (charBits == 0) {
            // Detener la conversión cuando se encuentra un carácter nulo
            break;
        }

        char c;
        if (charBits >= 1 && charBits <= 26) {
            c = 'a' + charBits - 1;
        } else if (charBits == 27) {
            c = '?';
        } else if (charBits == 28) {
            c = '"';
        } else if (charBits == 29) {
            c = '\'';
        } else if (charBits == 30) {
            c = '^';
        } else {
            c = '?'; // Un carácter indefinido para el caso predeterminado
        }

        result += c;
    }

    return result;
}

// Función para convertir un carácter a bits según las reglas dadas
int Index::charToBits(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    } else if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 1;
    } else {
        switch (c) {
            case '\0': return 0;
            case '?': return 27;
            case '"': return 28;
            case '\'': return 29;
            case '^': return 30;
            default: return 31;
        }
    }
}

// Función para convertir una cadena a bits según las reglas dadas
unsigned int Index::keywordToBits(const std::string& s, size_t b) {
    // Asegurar que la cadena tenga la longitud suficiente
    std::string padded = pad_with_null(s, (b / 5) + 1);

    int bits = 0;

    // Agregar caracteres completos a los bits
    for (size_t i = 0; i < b / 5; ++i) {
        bits = (bits << 5) | charToBits(padded[i]);
    }

    // Agregar un carácter parcial si es necesario
    size_t rem = b % 5;
    if (rem != 0) {
        bits = (bits << rem) | (charToBits(padded[b / 5]) >> (5 - rem));
    }

    // Devolver el resultado final
    return bits;
}

// Función para rellenar la cadena con caracteres nulos
std::string Index::pad_with_null(const std::string& s, size_t target_length) {
    std::string padded = s;
    while (padded.length() < target_length) {
        padded += '\0';
    }
    return padded;
}

// Método público para realizar la conversión y mostrar resultados
void Index::convertAndPrint() {
    std::cout << "Cadena original: " << inputString_ << std::endl;

    int resultBits = keywordToBits(inputString_, numBits_);

    // Imprimir el resultado en binario
    std::cout << "Resultado en binario: " << std::bitset<32>(resultBits) << std::endl;
    std::cout << "Resultado entero: " << resultBits<< std::endl;

    // Convertir los bits de vuelta a cadena y mostrar el resultado
    std::string convertedString = bitsToKeyword(resultBits, numBits_);
    std::cout << "Cadena convertida: " << convertedString << std::endl;
}

int Index::getC1() {
    return keywordToBits(inputString_, numBits_);
}

void Index::normalizarTexto(std::vector<unsigned int>& texto) {
    if (texto.empty()) {
        throw std::invalid_argument("La lista de longitudes está vacía");
    }

    unsigned int minLon = *std::min_element(texto.begin(), texto.end());
    unsigned int maxLon = *std::max_element(texto.begin(), texto.end());

    if (minLon == maxLon) {
        // Si todos los valores son iguales, no se puede normalizar
        for (auto& lon : texto) {
            lon = 0; // o algún valor constante, según tu necesidad
        }
    } else {
        // Normalizar las longitudes
        for (auto& lon : texto) {
            lon = static_cast<unsigned int>(std::round(255.0 * (lon - minLon) / (maxLon - minLon)));
        }
    }
}




/*********** COORDENADAS **************/

// Función para normalizar coordenadas
void Index::normalizarCoordenadas(std::vector<std::pair<double, double>>& coordenadas) {
    double minLat = DBL_MAX;
    double maxLat = DBL_MIN;
    double minLon = DBL_MAX;
    double maxLon = DBL_MIN;

    // Encontrar los valores mínimos y máximos de latitud y longitud
    for (const auto& coord : coordenadas) {
        minLat = std::min(minLat, coord.first);
        maxLat = std::max(maxLat, coord.first);
        minLon = std::min(minLon, coord.second);
        maxLon = std::max(maxLon, coord.second);
    }

    // Normalizar las coordenadas y redondear a enteros
    for (auto& coord : coordenadas) {
        coord.first = std::round(255 * (coord.first - minLat) / (maxLat - minLat));
        coord.second = std::round(255 * (coord.second - minLon) / (maxLon - minLon));
    }
}

/*
void Index::normalizarLatitud(std::vector<unsigned int>& longitudes) {
    if (longitudes.empty()) {
        throw std::invalid_argument("La lista de longitudes está vacía");
    }

    unsigned int minLon = *std::min_element(longitudes.begin(), longitudes.end());
    unsigned int maxLon = *std::max_element(longitudes.begin(), longitudes.end());

    if (minLon == maxLon) {
        // Si todos los valores son iguales, no se puede normalizar
        for (auto& lon : longitudes) {
            lon = 0; // o algún valor constante, según tu necesidad
        }
    } else {
        // Normalizar las longitudes
        for (auto& lon : longitudes) {
            lon = static_cast<unsigned int>(std::round(255.0 * (lon - minLon) / (maxLon - minLon)));
        }
    }
}
*/


unsigned int Index::normalizarEntero(long int entero, unsigned int minEntero, unsigned int maxEntero) {
        return (unsigned int)(std::round(255.0 * (entero - minEntero) / (maxEntero - minEntero)));
}

unsigned int Index::normalizarDouble(double entero, double minEntero, double maxEntero) {
        return (unsigned int)(std::round(255.0 * (entero - minEntero) / (maxEntero - minEntero)));
}

/***************** Horas *********************/

void Index::normalizarHoras(std::vector<long int>& horas) {
    long int minHora = *std::min_element(horas.begin(), horas.end());
    long int maxHora = *std::max_element(horas.begin(), horas.end());

    for (auto& hora : horas) {
        hora = std::round(255 * (hora - minHora) / (maxHora - minHora));
    }

}

long int Index::convertirFechaHora(std::string fechaHora, const std::string& format) {
    std::tm tmFechaHora = {};
    std::istringstream ss(fechaHora);
    ss >> std::get_time(&tmFechaHora, format.c_str());
    std::time_t timestamp = std::mktime(&tmFechaHora);
    return timestamp;
}

std::string Index::enteroABinario(long int entero) {
    std::string binario;
        while (entero) {
            binario.insert(binario.begin(), '0' + entero % 2);
            entero /= 2;
        }
        return binario.empty() ? "0" : binario;
    }


long int Index::convertirFechaHora(std::string valor) {
    int hora = Index::convertirFechaHora(valor, "%d/%m/%Y %H:%M:%S"); //comprobar formato de fechas
    return 255 * (hora - minDate) / (maxDate - minDate);

}
long int Index::normalizarLatitud(int valor) {
    return 255 * (valor - minLatitud) / (maxLatitud - minLatitud);
}

long int Index::normalizarLongitud(int valor) {
    return 255 * (valor - minLongitud) / (maxLongitud - minLongitud);
}
long int Index::normalizartexto(int valor) {
    return maxTexto;
}

unsigned int Index::entrelazado(unsigned int variable,unsigned int variable_1, unsigned int variable_2, unsigned int variable_3) {
    unsigned int resultado = 0;
    unsigned int resultado_1 = 0;
    unsigned int resultado_2 = 0;
    unsigned int resultado_3 = 0;

    // Recorrer los 8 bits de la variable
    for (int i = 7; i >= 0; --i) {
        // Construir la máscara para el bit en la posición i
        unsigned int mascara = 1 << i;

        // Verificar si el bit en la posición i está establecido en cada variable
        unsigned int establecido = variable & mascara;
        if(establecido != 0){
            int indice = i * 4 + 3;
            resultado += pow(2, indice);
            //std::cout << "Bit en la posición var_0 " << indice << ": " << resultado << std::endl;
        }

        establecido = variable_1 & mascara;
        if(establecido != 0){
            int indice = i * 4 + 2;
            resultado_1 += pow(2, indice);
            //std::cout << "Bit en la posición var_1 " << indice << ": " << resultado_1 << std::endl;
        }

        establecido = variable_2 & mascara;
        if(establecido != 0){
            int indice = i * 4 + 1;
            resultado_2 += pow(2, indice);
            //std::cout << "Bit en la posición var_2 " << indice << ": " << resultado_2 << std::endl;
        }

        establecido = variable_3 & mascara;
        if(establecido != 0){
            int indice = i * 4;
            resultado_3 += pow(2, indice);
            //std::cout << "Bit en la posición var_3 " << indice << ": " << resultado_3 << std::endl;
        }
    
}
    // Suma total de todas las variables resultado
    unsigned int suma_total = resultado + resultado_1 + resultado_2 + resultado_3;
    //std::cout << "Suma total de todos los resultados: " << suma_total << std::endl;
//std::cout << "Suma total de todos los resultados: " << resultado << std::endl;
//std::cout << "Suma total de todos los resultados: " << resultado_1 << std::endl;
//std::cout << "Suma total de todos los resultados: " << resultado_2 << std::endl;
//std::cout << "Suma total de todos los resultados: " << resultado_3 << std::endl;
//std::cout << "-----------------" << std::endl;
    return suma_total;
    }
