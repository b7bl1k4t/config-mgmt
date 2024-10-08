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

// Функция для извлечения файлов из tar-архива в временную директорию
std::string extract_tar(const std::string& tar_path) {
    // Создание временной директории с меткой времени
    std::string temp_dir = "/tmp/shell_emulator_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::filesystem::create_directory(temp_dir);

    struct archive* archive_ptr = archive_read_new();
    struct archive* extract_ptr = archive_write_disk_new();
    struct archive_entry* entry;

    // Настройка параметров чтения
    archive_read_support_format_tar(archive_ptr);
    archive_read_support_filter_all(archive_ptr);

    if (archive_read_open_filename(archive_ptr, tar_path.c_str(), 10240) != ARCHIVE_OK) {
        std::cerr << "Error opening tar file: " << archive_error_string(archive_ptr) << std::endl;
        return "";
    }
    
    // Извлечение содержимого архива
    while (archive_read_next_header(archive_ptr, &entry) == ARCHIVE_OK) {
        const char* current_file = archive_entry_pathname(entry);
        
        // Полный путь для извлечения файла
        std::string full_path = temp_dir + "/" + current_file;
        
        // Создание необходимых директорий
        std::filesystem::create_directories(std::filesystem::path(full_path).parent_path());

        std::cout << "Extracting: " << full_path << std::endl;

        // Установка прав доступа и информации о файле
        archive_entry_set_pathname(entry, full_path.c_str());
        archive_write_disk_set_options(extract_ptr, ARCHIVE_EXTRACT_TIME);

        if (archive_write_header(extract_ptr, entry) == ARCHIVE_OK) {
            const void* buffer;
            size_t size;
            la_int64_t offset;

            // Чтение данных из архива
            while (archive_read_data_block(archive_ptr, &buffer, &size, &offset) == ARCHIVE_OK) {
                archive_write_data_block(extract_ptr, buffer, size, offset);
            }
            archive_write_finish_entry(extract_ptr);
        }
    }
     // Освобождение ресурсов
    archive_read_free(archive_ptr);
    archive_write_free(extract_ptr);
    
    return temp_dir; // Возвращаем путь к временной директории
}

// Функция для выполнения команды ls
void ls_command(const std::string& user, const std::string& log_path, const std::string& current_dir) {
    for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
        std::cout << entry.path().filename().string() << std::endl;
    }
    log_action(log_path, user, "ls");
}

// Функция для выполнения команды cd {директория} / сd {}
void cd_command(const std::string& dir, const std::string& user, const std::string& log_path, std::string& current_dir, const std::string& root_dir) {
    std::string new_path;

    // Обработка команды без аргументов
    if (dir.empty()) {
        new_path = root_dir; // Перейти в корень
    } else {
        // Формирование нового пути
        new_path = (std::filesystem::path(current_dir) / dir).string(); // Использование path для правильного формирования пути
    }

    // Проверка существования нового каталога и что он находится в пределах виртуальной файловой системы
    if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path) &&
        std::filesystem::equivalent(root_dir, new_path.substr(0, root_dir.length()))) {
        current_dir = new_path; // Обновляем текущий каталог
        log_action(log_path, user, "cd " + (dir.empty() ? "~" : dir));
    } else {
        std::cerr << "cd: " << new_path << ": No such file or directory" << std::endl;
    }
}

// Функция для выполнения команды echo {сообщение}
void echo_command(const std::string& message, const std::string& user, const std::string& log_path) {
    std::cout << message << std::endl;
    log_action(log_path, user, "echo " + message);
}

// Функция для выполнения команды rmdir  {директория}
void rmdir_command(const std::string& dir, const std::string& user, const std::string& log_path, const std::string& current_dir) {
    std::string full_path = current_dir + "/" + dir;
    if (std::filesystem::remove_all(full_path)) {
        log_action(log_path, user, "rmdir " + dir);
    } else {
        std::cerr << "rmdir: failed to remove " << full_path << std::endl;
    }
}

// Функция для выполнения команды uniq {файл}
void uniq_command(const std::string& file_path, const std::string& user, const std::string& log_path) {
    std::ifstream input(file_path);
    if (!input) {
        std::cerr << "uniq: cannot open " << file_path << std::endl;
        return;
    }

    std::string prev_line, line;
    while (std::getline(input, line)) {
        if (line != prev_line) {
            std::cout << line << std::endl;
        }
        prev_line = line;
    }
    log_action(log_path, user, "uniq " + file_path);
}

// Функция для выполнения команды exit {}
void exit_command(const std::string& user, const std::string& log_path) {
    log_action(log_path, user, "exit");
    exit(0);
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

    // Извлечение файлов из tar-архива в временную директорию
    std::string current_dir = extract_tar(tar_path);
    if (current_dir.empty()) {
        return 1;
    }
    
    std::string root_dir = current_dir; //создание корневой директории
    
    std::string command;
    while (true) {
        //вывод местоположения пользователя
        std::cout << username << "@emulator:" << current_dir.substr(39, current_dir.length()) << "$ ";  
        
        std::getline(std::cin, command);
        
        if (command.substr(0, 2) == "ls") {
            ls_command(username, log_path, current_dir);
        } else if (command.substr(0, 2) == "cd") {
            if (command.length() > 2){
                cd_command(command.substr(3), username, log_path, current_dir, root_dir);
            }
            else {
                cd_command("", username, log_path, current_dir, root_dir);
            }
        } else if (command.substr(0, 4) == "echo" && command.length() > 4) {
            echo_command(command.substr(5), username, log_path);
        } else if (command.substr(0, 5) == "rmdir" && command.length() > 5) {
            rmdir_command(command.substr(6), username, log_path, current_dir);
        } else if (command.substr(0, 4) == "uniq" && command.length() > 4) {
            uniq_command(current_dir + "/" + command.substr(5), username, log_path);
        } else if (command == "exit") {
            exit_command(username, log_path);
        } else {
            std::cerr << command << ": command not found" << std::endl;
        }
    }
    
    return 0;
}
