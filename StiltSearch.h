#ifndef STILTSEARCH_H
#define STILTSEARCH_H

#include "Nodo.h"
#include "Nodo.h"
#include "Query.h"
#include "Range.h"
#include <iostream>

class StiltSearch{
public:
    std::vector<STILTNode*> search_node(STILTNode* root, Query query, Range range, int depth, unsigned int path) const;
    std::vector<STILTNode*> search_edge(STILTNode* root, Query query, Range range, int depth, unsigned int path) const;
    bool checkRange(Range range, Query query) const;
    bool checkRange(std::vector<int> range, std::vector<unsigned int> query) const;
    std::vector<unsigned int> decoder(unsigned int path) const;
    bool MatchQuery(std::vector<unsigned int> query, unsigned int nodeElement) const;
};



#endif
