#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <iostream>
#include <leveldb/db.h>
#include <leveldb/env.h>
#include <leveldb/status.h>
#include <libpq-fe.h>
#include "thirdParty/proto/data.pb.h"
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include "Element.h"
#include <limits>
#include "Query.h"

class DatabaseManager {
public:
    // Constructor
    DatabaseManager(const std::string& dbName);
    
    std::vector<Document>  readLevelDB_BD_SEARCH(const std::string& nombrebd, const std::vector<int>& conditions, Query query); // Declaración de la función

    // Métodos para leer desde PostgreSQL y escribir en LevelDB
    int readPostgreSQLwriteLevelDB_BD();
    int readPostgreSQLwriteLevelDB_Conf();

    // Métodos para leer desde LevelDB y mostrar los datos por pantalla
    std::vector<Element> readLevelDB_BD();
    int readLevelDB_Conf();
    
    

    // Getters and Setters
    std::string getDbName() const;
    void setDbName(const std::string& dbName);

    double getXMax() const;
    void setXMax(double xMax);

    double getXMin() const;
    void setXMin(double xMin);

    double getYMax() const;
    void setYMax(double yMax);

    double getYMin() const;
    void setYMin(double yMin);

    unsigned int getSnapMax() const;
    void setSnapMax(unsigned int snapMax);

    unsigned int getSnapMin() const;
    void setSnapMin(unsigned int snapMin);

    unsigned int getMaxString() const;
    void setMaxString(unsigned int maxString);

    unsigned int getMinString() const;
    void setMinString(unsigned int minString);

private:
    std::string dbName; // Nombre de la base de datos
    double xMax;
    double xMin;

    double yMax;
    double yMin;

    unsigned int snapMax = 1267698197;
    unsigned int snapMin = 1164448681;

    unsigned int maxString = std::numeric_limits<unsigned int>::max();
    unsigned int minString = 0;
};

#endif // DATABASE_MANAGER_H

