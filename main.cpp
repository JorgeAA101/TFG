#include "Nodo.h"
#include <iostream>
#include "Query.h"
#include "Range.h"
#include "Index.h"
#include "StiltSearch.h"
#include "Element.h"
#include "DatabaseManager.h"
#include <chrono>
void removeDuplicates(std::vector<int>& vec) {
    // Ordenar el vector
    std::sort(vec.begin(), vec.end());

    // Eliminar duplicados usando std::unique
    auto last = std::unique(vec.begin(), vec.end());

    // Reducir el tamaño del vector
    vec.erase(last, vec.end());
}

int main() {
std::string nombrebd = NOMBRE_BD; // Acceder al valor definido en la compilación

    DatabaseManager dbManager(nombrebd);
// Escribir datos en LevelDB desde PostgreSQL
    if (dbManager.readPostgreSQLwriteLevelDB_BD() != 0) {
        std::cerr << "Failed to read from PostgreSQL and write to LevelDB (BD)." << std::endl;
        return 1;
    }
    if (dbManager.readPostgreSQLwriteLevelDB_Conf() != 0) {
        std::cerr << "Failed to read from PostgreSQL and write to LevelDB (Conf)." << std::endl;
        return 1;
    }


    std::vector<Element> elements = dbManager.readLevelDB_BD();
   
   
    if (dbManager.readLevelDB_Conf() != 0) {
        std::cerr << "Failed to read from LevelDB (Conf)." << std::endl;
        return 1;
    }
                        
    unsigned int maxEnteroDate = dbManager.getSnapMax();
    unsigned int minEnteroDate = dbManager.getSnapMin();
    
    unsigned int maxEnteroText = dbManager.getMaxString();
    unsigned int minEnteroText = dbManager.getMinString();
    
    double maxEnteroX = dbManager.getXMax();
    double minEnteroX = dbManager.getXMin();
    double maxEnteroY = dbManager.getYMax();
    double minEnteroY = dbManager.getYMin();
    
    unsigned int normDate =0;
    unsigned int normText =0;
    unsigned int normX =0;
    unsigned int normY =0;
    
    STILTNode *root = new STILTNode();
    
    for(int i =0; i < elements.size() ; i++){
      normDate = elements[i].normalizeDate(minEnteroDate,maxEnteroDate);
      normText = elements[i].normalizeText(minEnteroText,maxEnteroText);
      normX = elements[i].normalizeX(minEnteroX,maxEnteroX);
      normY = elements[i].normalizeY(minEnteroY,maxEnteroY);
      
      /*
      elements[i].printDetails();
       std::cout << "Id: " << elements[i].id_<< std::endl;
       std::cout << "X: " << elements[i].getX() << "X: NORM " << normX<< std::endl;
        std::cout << "Y: " << elements[i].getY() << "Y: NORM " << normY << std::endl;
        std::cout << "Stats: " << elements[i].getTags() << " NormStats: " << normText << std::endl; 
        std::cout << " " << std::endl;
        */
      root->stilt_insert(Index::entrelazado(normX,normY, normText, normDate), elements[i].id_, root);
    }    
    
    std::vector<int> xMin = {0, 255};
    std::vector<int> xVacio = {};
    
    //Editar estas listas para los datos de la query con los datos sin normalizar
    std::vector<std::string> text = {"traffic","creat"};
    std::vector<double> x = {};
    std::vector<double> y = {};
    std::vector<unsigned int> date = {};

    //texto - x - y - date || poner xMin si hay datos en esa lista de la query, xVacio si no hay
    Range range (xMin, xVacio, xVacio, xVacio);

     unsigned int normText2 =0;

    Query query(text, x, y, date);    
    query.normalizeX(minEnteroX,maxEnteroX);
    query.normalizeY(minEnteroY,maxEnteroY);
    query.normalizeText(minEnteroText,maxEnteroText);
    //normText2 = query.normalizeText(minEnteroText,maxEnteroText);
    //std::cout << "normText2: " << normText2 << std::endl;
    query.normalizeDate(minEnteroDate,maxEnteroDate);

    
    StiltSearch s;
    std::vector<int> resultadoDistinct;
    
    //Iniciar cronometro
    auto start = std::chrono::high_resolution_clock::now();
    
    //Lanzar la busqueda
    std::vector<STILTNode*> resultado = s.search_node(root,query,range,0,0);
    //Insertar los IDs en una lista    
    for(int i = 0; i < resultado.size(); i++){
      resultadoDistinct.push_back(resultado[i]->id);

    }    
    //Eliminar IDs duplicados
    removeDuplicates(resultadoDistinct);
    
    //std::string nombrebd = "newyorkutf810dias";
    dbManager.readLevelDB_BD_SEARCH(nombrebd, resultadoDistinct);
    
    //Parar cronometro
    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
    std::cout << "elapsed: " << microseconds.count() << std::endl;
      
    for(int i = 0; i < resultadoDistinct.size(); i++){
      std::cout << "Id: " << resultadoDistinct[i] << std::endl;
    }
    
    return 0;
}
