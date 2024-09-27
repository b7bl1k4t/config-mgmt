#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <jsoncpp/json/json.h> // Для работы с JSON
#include <filesystem>
#include <archive.h>
#include <archive_entry.h>
#include <unistd.h> // Для работы с текущим каталогом
#include <cstdlib>
#include <chrono>
#include <iomanip>

// Функция для логирования действий
void log_action(const std::string& log_path, const std::string& user, const std::string& action) {
    Json::Value log_entry;
    log_entry["user"] = user;
    log_entry["action"] = action;
    
    std::ifstream log_file_in(log_path);
    Json::Value log_data;
    
    if (log_file_in) {
        log_file_in >> log_data;
        log_file_in.close();
    }
    
    log_data.append(log_entry);

    std::ofstream log_file_out(log_path);
    log_file_out << log_data;
    log_file_out.close();
}

// Основная функция
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <username> <path to tar file> <log file>" << std::endl;
        return 1;
    }

    std::string username = argv[1];
    std::string tar_path = argv[2];
    std::string log_path = argv[3];

    return 0;
}