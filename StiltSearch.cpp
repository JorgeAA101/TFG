#include "StiltSearch.h"

// Implementación de la función search_node
std::vector<STILTNode*>  StiltSearch::search_node(STILTNode* root, Query query, Range range, int depth, unsigned int path) const{
    if(root->length == 16)
    {
        int a = 5;
    }
    path += root->path;

    if(depth < 32){
        int dimension =  depth % 4;

        std::vector<STILTNode*> l;
        std::vector<STILTNode*> u;

        STILTNode *hojaD = new STILTNode();
        STILTNode *hojaI = new STILTNode();

        hojaI = root->getLeftChild();
        hojaD = root->getRightChild();

        std::vector<int> v;
        std::vector<int> v2;

        v = range.get(range, dimension);


        if(v.empty()){
            l = search_edge(hojaI,query, range, depth, path);
            u = search_edge(hojaD,query, range, depth, path);
        }

        else{
            v2 = v; //lower_half(v);
            v2[1] = v[0] + v[1];
            v2[1] = v2[1] / 2;

            if(checkRange(v2,query.get(query,dimension))){
                l = search_edge(hojaI,query, range, depth, path);
            }
            else{
                l = std::vector<STILTNode*>();
            }

            v2 = v;
            v2[0] = v[0] + v[1];
            v2[0] = v2[0] / 2;
            if(checkRange(v2,query.get(query,dimension))){
                u = search_edge(hojaD,query, range, depth, path);
            }
            else {
                u = std::vector<STILTNode*>();
            }
        }
        std::vector<STILTNode*> result;

        result.reserve(l.size() + u.size());
        result.insert(result.end(), l.begin(), l.end());
        result.insert(result.end(), u.begin(), u.end());

        return result;

    }
    else{
        std::vector<unsigned int> vDecoder(4,0);

        vDecoder = decoder(path);

        query.setMatch(MatchQuery(query.getXNorm(), vDecoder[0]));
        if(!query.getMatch()) return std::vector<STILTNode*>();

        query.setMatch(MatchQuery(query.getYNorm(), vDecoder[1]));
        if(!query.getMatch()) return std::vector<STILTNode*>();

        query.setMatch(MatchQuery(query.getTextoNorm(), vDecoder[2]));
        if(!query.getMatch()) return std::vector<STILTNode*>();

        query.setMatch(MatchQuery(query.getDateNorm(), vDecoder[3]));
        if(!query.getMatch()) return std::vector<STILTNode*>();

        return std::vector<STILTNode*>(1, root);
    }
}

bool StiltSearch::MatchQuery(std::vector<unsigned int> query, unsigned int nodeElement) const{
    if(query.size() == 0) return true;

    for (int i = 0; i < query.size(); ++i) {
        if(nodeElement == query[i]){
            return true;
        }
    }
    return false;
}


std::vector<STILTNode*> StiltSearch::search_edge(STILTNode* root, Query query, Range range, int depth, unsigned  int path) const{

if(root == nullptr) return std::vector<STILTNode*>();

    unsigned mask = 2147483648 >> depth;



    for(int i =0 ;i<root->length; i++){
        int dimension = (depth + i) %4;

        std::vector<int> v;

        v = range.get(range, dimension);
        if(!v.empty()){

            if((mask&root->path)==0){
                v[1] = v[0] + v[1];
                v[1] = v[1] / 2;

            }
            else{
                v[0] = v[0] + v[1];
                v[0] = v[0] / 2; //lower_half(v);
            }
            range = range.set(range,dimension, v);
        }
        mask = mask >> 1;
    }

    if(checkRange(range,query)){
         depth = (depth + root->length);
        return  search_node(root, query, range, depth, path);
    }


    return std::vector<STILTNode*>();
}


bool StiltSearch::checkRange(Range range, Query query) const{

    if(checkRange(range.getTexto(), query.getTextoNorm())){
        return true;
    }

    if(checkRange(range.getX(), query.getXNorm())){
        return true;
    }

    if(checkRange(range.getY(), query.getYNorm())){
        return true;
    }

    if(checkRange(range.getDate(), query.getDateNorm())){
        return true;
    }

    return false;
}

bool StiltSearch::checkRange(std::vector<int> range, std::vector<unsigned int> query) const{
    for (int i = 0; i < query.size() ;i++) {
        if(range[0] <= query[i] && range[1] >= query[i]){
            return true;
        }
    }

    return false;
}

std::vector<unsigned int> StiltSearch::decoder(unsigned int path) const{
// Número entero obtenido después de cruzar los números
//unsigned int path = 1576306063; // Ejemplo de número entero obtenido

    std::vector<unsigned int> vDecoder(4,0);


// Variables para almacenar los números originales
    unsigned int hora = 0;
    unsigned int coordenadaY = 0;
    unsigned int coordenadaX = 0;
    unsigned int Cadena = 0;

    unsigned int valor_final = 0;

// Recorrer los bits de la suma_total
    for (int i = 0; i < 32; ++i) {
// Obtener el bit en la posición i de la suma_total
        unsigned int bit = (path >> i) & 1;

// Colocar el bit en la posición correspondiente en las variables originales
        switch (i % 4) {
            case 3:
                coordenadaX |= bit << (i / 4);
                break;
            case 2:
                coordenadaY |= bit << (i / 4);
                break;
            case 1:
                Cadena |= bit << (i / 4);
                break;
            case 0:
                hora |= bit << (i / 4);
                break;
        }
    }
    vDecoder[0] = coordenadaX;
    vDecoder[1] = coordenadaY;
    vDecoder[2] = Cadena;
    vDecoder[3] =  hora;

    return vDecoder;
}
