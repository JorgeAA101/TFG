#include "DatabaseManager.h"
#include "Element.h"


// Constructor
DatabaseManager::DatabaseManager(const std::string& dbName) : dbName(dbName) {}
std::vector<Document> DatabaseManager::readLevelDB_BD_SEARCH(const std::string& nombrebd, const std::vector<int>& ids,  Query query) {
  std::cout << std::fixed << std::setprecision(7);
    // Comprobar si los IDs llegan correctamente al método
    /*std::cout << "IDs recibidos: ";
    for (int id : ids) {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    */

    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, nombrebd, &db);
    if (!status.ok()) {
        std::cerr << "Failed to open LevelDB: " << status.ToString() << std::endl;
        return {};
    }

    std::vector<Document> resultados;

     
    for (int id : ids) {
    
        std::string value;
        status = db->Get(leveldb::ReadOptions(), std::to_string(id), &value);
        if (!status.ok()) {
            std::cerr << "Failed to get value for ID: " << id << " Error: " << status.ToString() << std::endl;
            continue;
        }

        Document message;
        if (message.ParseFromString(value)) {
            if(!query.x.empty() && std::find(query.x.begin(), query.x.end(), message.x()) == query.x.end() ){
               //std::cout << "La X no estaba en la query: " << message.x() << std::endl; 
               continue;
            }
            
            if(!query.y.empty() && std::find(query.y.begin(), query.y.end(), message.y()) == query.y.end() ){
              //std::cout << "La Y no estaba en la query: " << message.y() << std::endl;
              continue;
            }
            
            if(!query.date_.empty() && std::find(query.date_.begin(), query.date_.end(), message.snapshot()) == query.date_.end() ){
              //std::cout << "El snapshot no estaba en la query: " << message.snapshot() << std::endl;
              continue;
            }
            
            bool textFound = true;
            for (const auto& entry : message.stats()) {
              if(!query.text.empty() && std::find(query.text.begin(), query.text.end(), entry.first) == query.text.end() ){
                 continue;
              }
              textFound = true;
              break;
            }
            if(!textFound){
            //std::cout << "El mensaje no estaba en la query: " << std::endl;
            continue;
            }
            resultados.push_back(message);
            
            /*std::cout << "Insertado:" << std::endl;
            std::cout << "X: " << message.x() << std::endl;            
            std::cout << "Y: " << message.y() << std::endl;
            std::cout << "snap: " << message.snapshot() << std::endl;
            std::cout << "text: ";
            for (const auto& entry : message.stats()) {
              std::cout << " " << entry.first << " - " << entry.second << " | ";
            }
            std::cout << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            */
        } else {
            std::cerr << "Error deserializing value for ID: " << id << std::endl;
        }
    }

    delete db;
    return resultados;
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

    PGresult *res = PQexec(connection, "SELECT o.id, st_x(o.geom) as x, st_y(o.geom) as y, o.osm_id, EXTRACT(EPOCH FROM o.timestamp)::int AS timestamp, EXTRACT(EPOCH FROM o.snapshot)::int  AS snapshot, o.tags, s.palabra, s.num, o.totalWordFrequency FROM osmdata o JOIN stats s ON o.osm_id = s.osm_id AND o.snapshot = s.snapshot order by o.id");
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

    PGresult *res = PQexec(connection, "select 'snapshot', min(EXTRACT(EPOCH FROM o.snapshot)::int) as minSnap, max(EXTRACT(EPOCH FROM o.snapshot)::int) as maxSnap, 'x', min(st_x(o.geom)) as minX, max(st_x(o.geom)) as maxX, 'y', min(st_y(o.geom)) as minY, max(st_y(o.geom)) as maxY from osmdata o ");
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
              Element element(message.id(),message.x(), message.y(), message.timestamp(), entry.first);
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
            /*if(it->key().ToString()=="snapshot"){
              setSnapMax(conf.max());
              setSnapMin(conf.min());
              //std::cout << "Estoy dentro del condicional SSSSSSnapshot"<< std::endl;
            }*/
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






