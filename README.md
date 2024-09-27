Ниже представлен пример файла `README.md` для вашего проекта с учетом функциональности программы.

---

# Shell Emulator

## Описание

Этот проект представляет собой простой эмулятор командной оболочки, реализующий базовые команды, такие как `ls`, `cd`, `echo`, `rmdir`, `uniq` и `exit`. Оболочка работает с файловой системой, которая извлекается из tar-архива, и ведет журнал действий пользователя в JSON-файл.

## Функциональность

- **ls** — выводит список файлов и директорий в текущем каталоге.
- **cd [directory]** — изменяет текущий каталог на указанный. Если каталог не указан, переходит в корневую директорию.
- **echo [message]** — выводит сообщение в стандартный вывод.
- **rmdir [directory]** — удаляет указанную директорию.
- **uniq [file]** — выводит уникальные строки из указанного файла.
- **exit** — завершает работу оболочки.

## Требования

Для сборки проекта необходимы следующие библиотеки и инструменты:
- Компилятор `g++`
- Библиотека `libarchive` для работы с tar-архивами.
- Библиотека `jsoncpp` для работы с JSON-логированием.
- Linux/Unix-подобная система для работы с командами файловой системы.

## Сборка проекта

1. Убедитесь, что все зависимости установлены. Для установки на Ubuntu выполните:

   ```bash
   sudo apt update
   sudo apt install g++ libarchive-dev libjsoncpp-dev
   ```

2. Скомпилируйте программу с помощью следующей команды:

   ```bash
   g++ -o shell shell.cpp -larchive -ljsoncpp -lpthread
   ```

3. Убедитесь, что файл успешно скомпилирован и готов к использованию.

## Использование

Запустите программу, передав имя пользователя, путь к tar-архиву и файл для логирования:

```bash
./shell <username> <path_to_tar_archive> <path_to_log_file>
```

### Пример

```bash
./shell user archive.tar log.json
```

После запуска программа будет ожидать команды, которые можно вводить как в обычной командной оболочке.

### Примеры команд:

- Просмотр файлов и папок:

   ```bash
   ls
   ```

- Переход в другой каталог:

   ```bash
   cd subdir
   ```

- Вывод сообщения:

   ```bash
   echo Hello, World!
   ```

- Удаление директории:

   ```bash
   rmdir subdir
   ```

- Уникальные строки из файла:

   ```bash
   uniq file.txt
   ```

- Завершение работы:

   ```bash
   exit
   ```

## Логирование

Программа сохраняет действия пользователя в лог-файл в формате JSON. В журнале указываются имя пользователя и команда, которую он выполнил.

Пример записи в лог-файл:

```json
[
  {
    "user": "user",
    "action": "ls"
  },
  {
    "user": "user",
    "action": "cd subdir"
  }
]
```
