#include <iostream>
#include <sstream>
#include <mysql.h>
#include <mosquitto.h>

const char* broker = "localhost";
const int port = 1883;

MYSQL* mysql_conn = nullptr;

void on_connect(struct mosquitto* mosq, void* obj, int rc) {
    if (rc == 0) {
        std::cout << "Connected to broker" << std::endl;
    } else {
        std::cerr << "Failed to connect to broker" << std::endl;
    }
}

void save_to_database(int id, float temp_one, float temp_second, int oil) {
    try {
        mysql_conn = mysql_init(nullptr);
        if (!mysql_real_connect(mysql_conn, "localhost", "root", "Still456_", "mqtt", 0, nullptr, 0)) {
            std::cerr << "Failed to connect to database: Error: " << mysql_error(mysql_conn) << std::endl;
            return;
        }

        std::stringstream ss;
        ss << "INSERT INTO user (id, temp_one, temp_second, oil) VALUES ("
           << id << ", '"
           << temp_one << "', "
           << temp_second << ", "
           << oil << ")";

        std::string sql_query = ss.str();

        if (mysql_query(mysql_conn, sql_query.c_str())) {
            std::cerr << "Failed to insert data into table: Error: " << mysql_error(mysql_conn) << std::endl;
            mysql_close(mysql_conn);
            return;
        }

        std::cout << "Successfully inserted data into database" << std::endl;

        mysql_close(mysql_conn);
    } catch (std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

void message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    std::cout << "Received message: " << (char*)msg->payload << std::endl;

    std::string message((char*)msg->payload);
    std::istringstream iss(message);

    std::string id_str, temp_one_str, temp_second_str, oil_str;
    std::getline(iss, id_str, ';');
    std::getline(iss, temp_one_str, ';');
    std::getline(iss, temp_second_str, ';');
    std::getline(iss, oil_str, ';');

    int id = std::stoi(id_str.substr(id_str.find('=') + 1));
    float temp_one = std::stof(temp_one_str.substr(temp_one_str.find('=') + 1));
    float temp_second = std::stof(temp_second_str.substr(temp_second_str.find('=') + 1));
    int oil = std::stoi(oil_str.substr(oil_str.find('=') + 1));

    save_to_database(id, temp_one, temp_second, oil);
}

int main() {
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);
    if (!mosq) {
        std::cerr << "Error: Out of memory when creating Mosquitto client" << std::endl;
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);

    int rc = mosquitto_connect(mosq, broker, port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr << "Unable to connect to broker: " << mosquitto_strerror(rc) << std::endl;
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    const char* topic = "table_user";
    mosquitto_subscribe(mosq, nullptr, topic, 0);

    // Пример данных для вставки в базу данных и отправки на сервер Mosquitto
    int id = 2;
    float temp_one = 25.4;
    float temp_second = 38.3;
    int oil = 60;

    // Сохранение в базу данных и отправка на сервер Mosquitto при запуске
    save_to_database(id, temp_one, temp_second, oil);

    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
