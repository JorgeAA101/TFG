#ifndef C1_H
#define C1_H

#include <string>
#include <bitset>
#include <vector>

class Index {
public:
    Index(const std::string& inputString, size_t numBits);

    std::string bitsToKeyword(int bits, size_t target_length);
    static int charToBits(char c);
    static unsigned int keywordToBits(const std::string& s, size_t b);
    static std::string pad_with_null(const std::string& s, size_t target_length);
    // Función para normalizar coordenadas
    static void normalizarCoordenadas(std::vector<std::pair<double, double>>& coordenadas);
    static void normalizarHoras(std::vector<long int>& horas);
    static long int convertirFechaHora(std::string fechaHora, const std::string& format);
    static std::string enteroABinario(long int entero);

    void convertAndPrint();
    int getC1();

    static unsigned int normalizarEntero(long int entero, unsigned int minEntero, unsigned int maxEntero) ;
    static unsigned int normalizarDouble(double entero, double minEntero, double maxEntero);
    void normalizarTexto(std::vector<unsigned int>& texto);



    long int convertirFechaHora(std::string valor);
    long int normalizarLatitud(int valor);
    long int normalizarLongitud(int valor);
    long int normalizartexto(int valor);
    
    
    static unsigned int entrelazado(unsigned int variable,unsigned int variable_1, unsigned int variable_2, unsigned int variable_3) ; 


private:
    std::string inputString_;
    size_t numBits_;

    int maxLatitud = 139;
    int minLatitud = -119;
    int maxLongitud = 64;
    int minLongitud = 34;
    int maxTexto = 120;
    int minTexto = 30;
    int maxDate = 110;
    int minDate = 40;

};

#endif // C1_H
