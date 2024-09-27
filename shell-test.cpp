#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "shell.cpp" // Подключаем ваш основной файл

// Настройка тестов
class ShellTest : public ::testing::Test {
protected:
    std::string temp_dir;
    std::string log_path;
    std::string username;

    void SetUp() override {
        username = "test_user";
        temp_dir = "/tmp/shell_test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::filesystem::create_directory(temp_dir);
        log_path = temp_dir + "/log.json";
    }

    void TearDown() override {
        std::filesystem::remove_all(temp_dir); // Удаление временной директории
    }
};
