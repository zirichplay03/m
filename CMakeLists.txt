cmake_minimum_required(VERSION 3.28)
project(m)

set(CMAKE_CXX_STANDARD 17)

# Указываем пути к заголовочным файлам Mosquitto и MySQL Connector/C++
include_directories(
        lib/mosquitto/devel
        lib/mysql/MySQL-Server-8.0/include
)

# Указываем пути к каталогам с библиотеками Mosquitto и MySQL Connector/C++
link_directories(
        lib/mosquitto/cpp
        lib/mysql/MySQL-Server-8.0/lib
)

# Создаем исполняемый файл и добавляем исходные файлы
add_executable(m main.cpp)

# Связываем библиотеки с исполняемым файлом
target_link_libraries(m PRIVATE mosquitto)
target_link_libraries(m PRIVATE libmysql) # Используем mysqlcppconn8 для MySQL Connector/C++ 8.0

# Выводим информацию для отладки
message(STATUS "Include directories for Mosquitto: ${CMAKE_SOURCE_DIR}/lib/mosquitto/devel")
message(STATUS "Link directories for Mosquitto: ${CMAKE_SOURCE_DIR}/lib/mosquitto/cpp")
message(STATUS "Include directories for MySQL Connector/C++: ${CMAKE_SOURCE_DIR}/lib/mysql/MySQL-Server-8.0/include")
message(STATUS "Link directories for MySQL Connector/C++: ${CMAKE_SOURCE_DIR}/lib/mysql/MySQL-Server-8.0/lib")
