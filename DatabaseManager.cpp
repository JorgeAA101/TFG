#include "DatabaseManager.h"
#include "Element.h"


// Constructor
DatabaseManager::DatabaseManager(const std::string& dbName) : dbName(dbName) {}


int DatabaseManager::readLevelDB_BD_SEARCH(const std::string& nombrebd, const std::vector<int>& conditions) {
    const char* conninfo = "host=localhost user=postgres password=1234 dbname=";
    std::string resultado = std::string(conninfo) + nombrebd;

    PGconn* connection = PQconnectdb(resultado.c_str());
    if (PQstatus(connection) != CONNECTION_OK) {
        std::cout << "Connection error: " << PQerrorMessage(connection) << std::endl;
        PQfinish(connection);
        return 1;
    }

    // Construir la consulta SQL
    std::ostringstream conditionStream;
    conditionStream << "WHERE o.id IN (";
    for (size_t i = 0; i < conditions.size(); ++i) {
        conditionStream << conditions[i];
        if (i < conditions.size() - 1) {
            conditionStream << ", ";
        }
    }
    conditionStream << ") ";

    std::string query = "SELECT o.id, st_x(o.geom) as x, st_y(o.geom) as y, o.osm_id, "
                        "EXTRACT(EPOCH FROM o.timestamp)::int AS timestamp, "
                        "EXTRACT(EPOCH FROM o.snapshot)::int AS snapshot, o.tags, s.palabra, s.num, o.totalWordFrequency "
                        "FROM osmdata o "
                        "JOIN stats s ON o.osm_id = s.osm_id AND o.snapshot = s.snapshot "
                        + conditionStream.str() +
                        "ORDER BY o.id";

    std::cout << "Executing query: " << query << std::endl;

    // Ejecutar la consulta SQL
    PGresult* res = PQexec(connection, query.c_str());

    // Verificar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error executing query: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        PQfinish(connection);
        return 1;
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }

    // Obtener el número de filas y columnas del resultado
    int num_rows = PQntuples(res);

    if (num_rows == 0) {
        std::cout << "Query returned no results." << std::endl;
        PQclear(res);
        PQfinish(connection);
        return 1;
    } else {
        std::cout << "Query returned " << num_rows << " rows." << std::endl;
    }

    // Recorrer y mostrar los resultados
    for (int i = 0; i < num_rows; ++i) {
        int id = std::strtol(PQgetvalue(res, i, 0), nullptr, 10);

        double x = std::strtod(PQgetvalue(res, i, 1), nullptr);
        double y = std::strtod(PQgetvalue(res, i, 2), nullptr);
        int osm_id = std::strtol(PQgetvalue(res, i, 3), nullptr, 10);
        int timestamp = std::strtol(PQgetvalue(res, i, 4), nullptr, 10);
        int snapshot = std::strtol(PQgetvalue(res, i, 5), nullptr, 10);
        std::string tags = PQgetvalue(res, i, 6);
        int totalWordFrequency = std::strtol(PQgetvalue(res, i, 9), nullptr, 10);

        std::cout << "ID: " << id << ", x: " << x << ", y: " << y
                  << ", OSM ID: " << osm_id << ", Timestamp: " << timestamp
                  << ", Snapshot: " << snapshot << ", Tags: " << tags
                  << ", Total Word Frequency: " << totalWordFrequency << std::endl;
    }

    // Liberar la memoria y cerrar la conexión
    PQclear(res);
    PQfinish(connection);

    return 0;
}




double DatabaseManager::getXMax() const {
    return xMax;
}

void DatabaseManager::setXMax(double xMax) {
    this->xMax = xMax;
}

double DatabaseManager::getXMin() const {
    return xMin;
}

void DatabaseManager::setXMin(double xMin) {
    this->xMin = xMin;
}

double DatabaseManager::getYMax() const {
    return yMax;
}

void DatabaseManager::setYMax(double yMax) {
    this->yMax = yMax;
}

double DatabaseManager::getYMin() const {
    return yMin;
}

void DatabaseManager::setYMin(double yMin) {
    this->yMin = yMin;
}

unsigned int DatabaseManager::getSnapMax() const {
    return snapMax;
}

void DatabaseManager::setSnapMax(unsigned int snapMax) {
    this->snapMax = snapMax;
}

unsigned int DatabaseManager::getSnapMin() const {
    return snapMin;
}

void DatabaseManager::setSnapMin(unsigned int snapMin) {
    this->snapMin = snapMin;
}

unsigned int DatabaseManager::getMaxString() const {
    return maxString;
}

void DatabaseManager::setMaxString(unsigned int maxString) {
    this->maxString = maxString;
}

unsigned int DatabaseManager::getMinString() const {
    return minString;
}

void DatabaseManager::setMinString(unsigned int minString) {
    this->minString = minString;
}


// Método para leer desde PostgreSQL y escribir en LevelDB
int DatabaseManager::readPostgreSQLwriteLevelDB_BD() {
    const char* conninfo = "host=localhost user=postgres password=1234 dbname=";
    std::string resultado = std::string(conninfo) + dbName;
    PGconn* connection = PQconnectdb(resultado.c_str());
    if (PQstatus(connection) != CONNECTION_OK) {
        std::cout << "Connection error: " << PQerrorMessage(connection) << std::endl;
        PQfinish(connection);
        return 1;
    }

    PGresult *res = PQexec(connection, "SELECT o.id, st_x(o.geom) as x, st_y(o.geom) as y, o.osm_id, EXTRACT(EPOCH FROM o.timestamp)::int AS timestamp, EXTRACT(EPOCH FROM o.snapshot)::int  AS snapshot, o.tags, s.palabra, s.num, o.totalWordFrequency FROM osmdata o JOIN stats s ON o.osm_id = s.osm_id AND o.snapshot = s.snapshot order by o.id limit 1000000;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error executing query: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        PQfinish(connection);
        return 1;
    }

    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
    if (!status.ok()) {
        std::cerr << "Failed to open LevelDB: " << status.ToString() << std::endl;
        return 1;
    }

    Document data;
    auto* stats = data.mutable_stats();

    int num_rows = PQntuples(res);
    int serial = -1;

    for (int i = 0; i < num_rows; ++i) {
        const char* valor = PQgetvalue(res, i, 0);
        int valorEntero = atoi(valor);

        if (valorEntero == serial) {
            (*stats)[PQgetvalue(res, i, 7)] = std::strtol(PQgetvalue(res, i, 8), nullptr, 10);
        } else {
            if (i != 0) {
                std::string serialized_data;
                data.SerializeToString(&serialized_data);
                status = db->Put(leveldb::WriteOptions(), std::to_string(data.id()).c_str(), serialized_data);
                if (!status.ok()) {
                    std::cerr << "Failed to store data in LevelDB: " << status.ToString() << std::endl;
                    delete db;
                    return 1;
                }
                data.clear_id();
                data.clear_x();
                data.clear_y();
                data.clear_osm_id();
                data.clear_timestamp();
                data.clear_snapshot();
                data.clear_tags();
                data.clear_stats();
                data.clear_totalwordfrequency();
            }
            serial = valorEntero;
            data.set_id(std::strtol(PQgetvalue(res, i, 0), nullptr, 10));
            data.set_x(std::strtod(PQgetvalue(res, i, 1), nullptr));
            data.set_y(std::strtod(PQgetvalue(res, i, 2), nullptr));
            data.set_osm_id(std::strtol(PQgetvalue(res, i, 3), nullptr, 10));
            data.set_timestamp(std::strtol(PQgetvalue(res, i, 4), nullptr, 10));
            data.set_snapshot(std::strtol(PQgetvalue(res, i, 5), nullptr, 10));
            data.set_tags(PQgetvalue(res, i, 6));
            (*stats)[PQgetvalue(res, i, 7)] = std::strtol(PQgetvalue(res, i, 8), nullptr, 10);
            data.set_totalwordfrequency(std::strtol(PQgetvalue(res, i, 9), nullptr, 10));
        }
    }

    std::string serialized_data;
    data.SerializeToString(&serialized_data);
    status = db->Put(leveldb::WriteOptions(), std::to_string(data.id()).c_str(), serialized_data);
    if (!status.ok()) {
        std::cerr << "Failed to store data in LevelDB: " << status.ToString() << std::endl;
        delete db;
        return 1;
    }

    PQclear(res);
    PQfinish(connection);
    delete db;

    return 0;
}

// Método para leer datos de configuración desde PostgreSQL y escribir en LevelDB
int DatabaseManager::readPostgreSQLwriteLevelDB_Conf() {
    const char* conninfo = "host=localhost user=postgres password=1234 dbname=";
    std::string resultado = std::string(conninfo) + dbName;

    PGconn* connection = PQconnectdb(resultado.c_str());
    if (PQstatus(connection) != CONNECTION_OK) {
        std::cout << "Connection error: " << PQerrorMessage(connection) << std::endl;
        PQfinish(connection);
        return 1;
    }

    PGresult *res = PQexec(connection, "select 'snapshot', min(EXTRACT(EPOCH FROM o.snapshot)::int) as minSnap, max(EXTRACT(EPOCH FROM o.snapshot)::int) as maxSnap, 'x', min(st_x(o.geom)) as minX, max(st_x(o.geom)) as maxX, 'y', min(st_y(o.geom)) as minY, max(st_y(o.geom)) as maxY from osmdata o limit 1000000;");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error executing query: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        PQfinish(connection);
        return 1;
    }

    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbName + "", &db);
    if (!status.ok()) {
        std::cerr << "Failed to open LevelDB: " << status.ToString() << std::endl;
        return 1;
    }

    Conf dataConf;

    for (int i = 0; i < 3; ++i) {
        dataConf.set_min(std::strtod(PQgetvalue(res, 0, 1 + (i * 3)), nullptr));
        dataConf.set_max(std::strtod(PQgetvalue(res, 0, 2 + (i * 3)), nullptr));
        std::string serialized_data_conf;
        dataConf.SerializeToString(&serialized_data_conf);
        status = db->Put(leveldb::WriteOptions(), PQgetvalue(res, 0, 0 + (i * 3)), serialized_data_conf);
        if (!status.ok()) {
            std::cerr << "Failed to store data in LevelDB: " << status.ToString() << std::endl;
            delete db;
            return 1;
        }
    }

    PQclear(res);
    PQfinish(connection);
    delete db;

    return 0;
}

std::vector<Element> DatabaseManager::readLevelDB_BD() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
    

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  
  std::vector<Element> elements;

    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        Document message;
        if (message.ParseFromString(it->value().ToString())) {
  //std::cout << "x: " << message.x() << ", y: " << message.y() << ", timestamp: " << message.timestamp() << ", tags: " << message.tags() << ", id: " << message.id() <<  std::endl;
                      const auto& stats = message.stats();
            for (const auto& entry : stats) {
              //std::cout << "x: " << message.x() << ", y: " << message.y() << ", snapshot: " << message.snapshot() << ", stats: " <<  entry.first << ", id: " << message.id() << std::endl;
              Element element(message.id(),message.x(), message.y(), message.snapshot(), entry.first);
              elements.push_back(element);
            }
            
        } else {
            std::cerr << "Error deserializing value from database" << std::endl;
        }
    }

    if (!it->status().ok()) {
        std::cerr << "An error occurred during the iteration: " << it->status().ToString() << std::endl;
    }
    delete it;
    delete db;

    return elements;
}

// Método para leer y mostrar datos de configuración desde LevelDB
int DatabaseManager::readLevelDB_Conf() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbName + "", &db);
    if (!status.ok()) {
        std::cerr << "Failed to open LevelDB: " << status.ToString() << std::endl;
        return 1;
    }

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

     for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string value = it->value().ToString();

        // Intenta deserializar el valor en un objeto Conf
        Conf conf;
        if (conf.ParseFromString(value)) {
            // Imprime los datos de Conf
            //std::cout << "_CONF -> KEY: " << it->key().ToString() << std::endl;
            if(it->key().ToString()=="snapshot"){
              setSnapMax(conf.max());
              setSnapMin(conf.min());
              //std::cout << "Estoy dentro del condicional SSSSSSnapshot"<< std::endl;
            }
            if(it->key().ToString()=="x"){
              //std::cout << "Estoy dentro del condicional XXXXXXXXXX"<< std::endl;
              setXMax(conf.max());
              setXMin(conf.min());
            }
            if(it->key().ToString()=="y"){
              //std::cout << "Estoy dentro del condicional YYYYYYYYYY"<< std::endl;
                
              setYMax(conf.max());
              setYMin(conf.min());
            }
            //std::cout << "_CONF -> Min"<<it->key().ToString()<<": " << conf.min() << std::endl;
            
            //std::cout << "_CONF -> Max"<<it->key().ToString()<<": "  << conf.max() << std::endl;
            
            
            //std::cout << "----------------------" << std::endl;
        } else {
            std::cerr << "Error deserializing value from database" << std::endl;
        }

    }

    if (!it->status().ok()) {
        std::cerr << "An error occurred during the iteration: " << it->status().ToString() << std::endl;
    }
    delete it;
    delete db;

    return 0;
}






