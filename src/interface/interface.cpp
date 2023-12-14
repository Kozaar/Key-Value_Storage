#include "interface.h"

namespace s21 {
Interface::Interface() { SetRegexMap(); }

void Interface::SetRegexMap() {
  std::string key = (R"(\s\w+)");
  std::string values = (R"(\s[\S]+\s[\S]+\s\d+\s[\S]+\s\d+)");
  std::string values_dash =
      (R"(\s[\S|-]+\s[\S|-]+\s[\S|-]+\s[\S|-]+\s[\S|-]+)");
  std::string ex = (R"((\sEX\s\d+)?)");
  std::string end = (R"(\s*?$)");

  regexMap["SET"] =
      std::regex(R"(^SET)" + key + values + ex + end, std::regex::icase);
  regexMap["GET"] = std::regex(R"(^GET)" + key + end, std::regex::icase);
  regexMap["EXISTS"] = std::regex(R"(^EXISTS)" + key + end, std::regex::icase);
  regexMap["DEL"] = std::regex(R"(^DEL)" + key + end, std::regex::icase);
  regexMap["UPDATE"] = std::regex(R"(^UPDATE)" + key + values_dash + ex + end,
                                  std::regex::icase);
  regexMap["KEYS"] = std::regex(R"(^KEYS)" + end, std::regex::icase);
  regexMap["RENAME"] =
      std::regex(R"(^RENAME)" + key + key + end, std::regex::icase);
  regexMap["TTL"] = std::regex(R"(^TTL)" + key + end, std::regex::icase);
  regexMap["FIND"] =
      std::regex(R"(^FIND)" + values_dash + ex + end, std::regex::icase);
  regexMap["SHOWALL"] = std::regex(R"(^SHOWALL)" + end, std::regex::icase);
  regexMap["UPLOAD"] = std::regex(R"(^UPLOAD\s.*$)", std::regex::icase);
  regexMap["EXPORT"] = std::regex(R"(^EXPORT\s.*)", std::regex::icase);
  regexMap["HELP"] = std::regex(R"(^HELP)" + end, std::regex::icase);
  regexMap["RETURN"] = std::regex(R"(^RETURN)" + end, std::regex::icase);
}

void Interface::ShowMainMenu() {
  while (true) {
    std::cout << "~~Main menu~~\n"
              << "Выберите тип хранилища:\n"
              << "\t1 - Хеш-таблица\n"
              << "\t2 - Самобалансирующееся бинарное дерево поиска\n"
              << "\t0 - Выход\n";

    int input = -1;
    std::cin >> input;

    if (std::cin.fail()) {
      ShowWrongInputAttention();
      continue;
    }

    switch (input) {
      case 1:
        storage = std::make_unique<Controller>(ContainerType::hashTable);
        StorageStart();
        break;
      case 2:
        storage = std::make_unique<Controller>(ContainerType::rbtree);
        StorageStart();
        break;
      case 0:
        std::cout << "bye-bye\n";
        return;
      default:
        ShowWrongInputAttention();
    }
  }
}

void Interface::ShowWrongInputAttention() {
  std::cout << "Некорректный ввод\n";
  std::cin.clear();
  WaitingForInput();
}

void Interface::WaitingForInput() {
  std::cout << "Для продолжения нажмите Enter\n";
  std::cin.sync();
  std::cin.ignore(INT_MAX, '\n');
  std::cout << "Введите команду:\n";
}

void Interface::StorageStart() {
  std::cout << "~~Storage~~\n"
            << "\tЗапрос к хранилищу - <Команда запроса> <Аргумент1> "
               "<Аргумент2>...\n"
            << "\tHELP - для вывода справочной информации о командах запросов\n"
            << "\tRETURN - для возврата в главное меню\n";
  while (true) {
    WaitingForInput();
    std::string str = "";
    std::getline(std::cin, str);
    if (std::cin.fail() or str.empty()) {
      ShowWrongInputAttention();
      continue;
    }

    std::vector<std::string> args = SplitBySpace(str);
    std::string command = args.front();

    if (!IsValidCommand(str, command)) {
      std::cout << "Введенна некорректная команда!\n" << std::endl;
      continue;
    }

    Command commandNum = GetCommandNum(command.c_str());
    switch (commandNum) {
      case Command::SET:
        Set(args);
        break;
      case Command::GET:
        Get(args);
        break;
      case Command::EXISTS:
        Exists(args);
        break;
      case Command::DEL:
        Del(args);
        break;
      case Command::UPDATE:
        Update(args);
        break;
      case Command::KEYS:
        Keys();
        break;
      case Command::RENAME:
        Rename(args);
        break;
      case Command::TTL:
        Ttl(args);
        break;
      case Command::FIND:
        Find(args);
        break;
      case Command::SHOWALL:
        Showall();
        break;
      case Command::UPLOAD:
        Upload(args);
        break;
      case Command::EXPORT:
        Export(args);
        break;
      case Command::HELP:
        ShowHelpMenu();
        break;
      case Command::RETURN:
        return;
      default:
        ShowWrongInputAttention();
        continue;
    }
  }
}

std::vector<std::string> Interface::SplitBySpace(const std::string& str) {
  std::string tmp;
  std::stringstream ss(str);
  std::vector<std::string> v;
  while (getline(ss, tmp, ' ')) v.push_back(tmp);
  return v;
}

bool Interface::IsValidCommand(std::string str, std::string name) {
  std::transform(name.begin(), name.end(), name.begin(), ::toupper);
  return regex_match(str, regexMap[name]);
}

Interface::Command Interface::GetCommandNum(const char* commandName) {
  if (strcasecmp(commandName, "SET") == 0) return Command::SET;
  if (strcasecmp(commandName, "GET") == 0) return Command::GET;
  if (strcasecmp(commandName, "EXISTS") == 0) return Command::EXISTS;
  if (strcasecmp(commandName, "DEL") == 0) return Command::DEL;
  if (strcasecmp(commandName, "UPDATE") == 0) return Command::UPDATE;
  if (strcasecmp(commandName, "KEYS") == 0) return Command::KEYS;
  if (strcasecmp(commandName, "RENAME") == 0) return Command::RENAME;
  if (strcasecmp(commandName, "TTL") == 0) return Command::TTL;
  if (strcasecmp(commandName, "FIND") == 0) return Command::FIND;
  if (strcasecmp(commandName, "SHOWALL") == 0) return Command::SHOWALL;
  if (strcasecmp(commandName, "UPLOAD") == 0) return Command::UPLOAD;
  if (strcasecmp(commandName, "EXPORT") == 0) return Command::EXPORT;
  if (strcasecmp(commandName, "HELP") == 0) return Command::HELP;
  if (strcasecmp(commandName, "RETURN") == 0) return Command::RETURN;
  return Command::ERROR;
}

void Interface::Set(const std::vector<std::string>& commandArgs) {
  Value values;
  int ex = 0;
  Key key = commandArgs.at(1);
  values.lastname = commandArgs.at(2);
  values.name = commandArgs.at(3);
  values.year = std::stoi(commandArgs.at(4));
  values.city = commandArgs.at(5);
  values.coins = std::stoi(commandArgs.at(6));
  if (commandArgs.size() > 8) ex = std::stoi(commandArgs.at(8));

  if (storage->set(key, values, ex) == noErrors)
    std::cout << "OK\n";
  else
    std::cout << "ERROR: key already exists\n";
}

void Interface::Get(const std::vector<std::string>& commandArgs) {
  Key key = commandArgs.at(1);

  auto findedValues = storage->get(key);
  if (findedValues.has_value())
    findedValues.value().Print();
  else
    std::cout << "(null)\n";
}

void Interface::Exists(const std::vector<std::string>& commandArgs) {
  Key key = commandArgs.at(1);
  if (storage->exists(key))
    std::cout << "true\n";
  else
    std::cout << "false\n";
}

void Interface::Del(const std::vector<std::string>& commandArgs) {
  Key key = commandArgs.at(1);

  if (storage->del(key) == noErrors)
    std::cout << "true\n";
  else
    std::cout << "false\n";
}

void Interface::Update(const std::vector<std::string>& commandArgs) {
  Value values;
  int ex = 0;
  Key key = commandArgs.at(1);
  values.lastname = commandArgs.at(2);
  values.name = commandArgs.at(3);
  values.year = commandArgs.at(4) == "-" ? 0 : std::stoi(commandArgs.at(4));
  values.city = commandArgs.at(5);
  values.coins = commandArgs.at(6) == "-" ? 0 : std::stoi(commandArgs.at(6));
  int mask = 0;
  for (size_t i = 2; i < 7; i++)
    if (commandArgs.at(i) != "-") mask |= (1 << (i - 2));
  if (commandArgs.size() > 8) {
    ex = std::stoi(commandArgs.at(8));
    mask |= 5;
  }
  if (storage->update(key, values, ex, mask) == noErrors)
    std::cout << "OK\n";
  else
    std::cout << "ERROR\n";
}

void Interface::Keys() {
  auto findedValues = storage->keys();
  if (!findedValues.empty())
    for (size_t i = 0; i < findedValues.size(); i++)
      std::cout << i + 1 << ") " << findedValues.at(i) << std::endl;
  else
    std::cout << "(null)\n";
}

void Interface::Rename(const std::vector<std::string>& commandArgs) {
  Key key1 = commandArgs.at(1);
  Key key2 = commandArgs.at(2);

  if (storage->rename(key1, key2) == noErrors)
    std::cout << "true\n";
  else
    std::cout << "false\n";
}

void Interface::Showall() {
  auto findedValues = storage->showall();
  if (!findedValues.empty()) {
    printf("%s | %-10s| %-11s| %-5s|   %-8s| %-10s|\n", "№", "Last name",
           "First name", "Year", "City", "Number of coins");
    for (size_t i = 0; i < findedValues.size(); i++) {
      Value i_values = findedValues.at(i);
      printf("%-2zu  %-12s %-11s %-7d %-10s %-5d\n", i + 1,
             i_values.lastname.c_str(), i_values.name.c_str(), i_values.year,
             i_values.city.c_str(), i_values.coins);
    }
  } else
    std::cout << "(null)\n";
}

void Interface::Ttl(const std::vector<std::string>& commandArgs) {
  Key key = commandArgs.at(1);
  int ttl = storage->Ttl(key);
  if (ttl == keyNotFound)
    std::cout << "(null)\n";
  else
    std::cout << ttl << std::endl;
}

void Interface::Find(const std::vector<std::string>& commandArgs) {
  Value values;
  int ex = 0;
  values.lastname = commandArgs.at(1);
  values.name = commandArgs.at(2);
  values.year = commandArgs.at(3) == "-" ? 0 : std::stoi(commandArgs.at(3));
  values.city = commandArgs.at(4);
  values.coins = commandArgs.at(5) == "-" ? 0 : std::stoi(commandArgs.at(5));
  int mask = 0;
  for (size_t i = 1; i < 6; i++)
    if (commandArgs.at(i) != "-") mask |= (1 << (i - 1));
  if (commandArgs.size() > 7) {
    ex = std::stoi(commandArgs.at(7));
    mask |= 5;
  }

  auto findedValues = storage->find(values, ex, mask);

  if (!findedValues.empty())
    for (size_t i = 0; i < findedValues.size(); i++)
      std::cout << i + 1 << ") " << findedValues.at(i) << std::endl;
  else
    std::cout << "(null)\n";
}

void Interface::Upload(const std::vector<std::string>& commandArgs) {
  int rowCount = storage->upload(commandArgs.at(1));
  if (rowCount == canNotOpenFile)
    std::cout << "Ошибка: Невозможно открыть файл\n";
  else if (rowCount == corruptedFile)
    std::cout << "Ошибка: Файл поврежден\n";
  else if (rowCount == unknownError)
    std::cout << "Ошибка\n";
  else if (rowCount > 0)
    std::cout << "OK " << rowCount << "\n";
}

void Interface::Export(const std::vector<std::string>& commandArgs) {
  int rowCount = storage->exportValues(commandArgs.at(1));
  if (rowCount == canNotOpenFile)
    std::cout << "Ошибка: Невозможно открыть файл\n";
  else if (rowCount > 0)
    std::cout << "OK " << rowCount << "\n";
}

void Interface::ShowHelpMenu() {
  std::cout << "Команды необходимо вводить в представленном формате:\n\n"

            << "\t<Ключ> - должен состоять из букв латинского алфавита, может "
               "содержать цифры\n"
            << "\t<Фамилия>, <Имя>, <Город> - должны состоять из любых "
               "непробельных символов\n"
            << "\t<Число текущих коинов>, <Время в секундах>, <Город> - должны "
               "состоять из цифр\n"
            << "\tЕсли в первом или последнем символе входной строки "
               "содержится символ \",\n"
            << "\tто он символ будет удален, и в хранилище будет обрезанная "
               "строка\n\n"

            << "\tSET <ключ> <Фамилия> <Имя> <Год рождения> <Город> <Число "
               "текущих коинов> EX <время в \n"
            << "\tсекундах>(необязательное поле)\n"
            << "\tКоманда используется для установки ключа и его значения.\n\n"

            << "\tGET <ключ>\n"
            << "\tКоманда используется для получения значения, связанного с "
               "ключом. Если такой записи нет, \n"
            << "\tто будет возвращён (null)\n\n"

            << "\tEXISTS <ключ>\n"
            << "\tЭта команда проверяет, существует ли запись с данным ключом. "
               "Она возвращает true если \n"
            << "\tобъект существует или false если нет\n\n"

            << "\tDEL <ключ>\n"
            << "\tКоманда удаляет ключ и соответствующее значение, после чего "
               "возвращает true, если запись\n"
            << "\tуспешно удалена, в противном случае - false\n\n"

            << "\tUPDATE <ключ> <Фамилия> <Имя> <Год рождения> <Город> <Число "
               "текущих коинов>\n"
            << "\tКоманда обновляет значение по соответствующему ключу, если "
               "такой ключ существует\n"
            << "\tЕсли же какое-то поле менять не планируется, то на его месте "
               "ставится прочерк '-'\n\n"

            << "\tKEYS\n"
            << "\tВозвращает все ключи, которые есть в хранилище\n\n"

            << "\tRENAME <ключ> <ключ>\n"
            << "\tКоманда используется для переименования ключей\n\n"

            << "\tTTL <ключ>\n"
            << "\tКогда ключ установлен с истечением срока действия, эту "
               "команду можно использовать для\n"
            << "\tпросмотра оставшегося времени.\n"
            << "\tЕсли записи с заданным ключом не существует, то возвращается "
               "(null)\n\n"

            << "\tFIND <ключ> <Фамилия> <Имя> <Год рождения> <Город> <Число "
               "текущих коинов>\n"
            << "\tЭта команда используется для восстановления ключа (или "
               "ключей) по заданному значению.\n"
            << "\tЕсли же по каким-то полям не будет выполняться поиск, то на "
               "их месте ставится прочерк -\n\n"

            << "\tSHOWALL\n"
            << "\tКоманда для получения всех записей, которые содержатся в "
               "key-value хранилище на текущий "
            << "момент\n\n"

            << "\tUPLOAD\n"
            << "\tДанная команда используется для загрузки данных из файла. "
               "Файл содержит список \n"
            << "\tзагружаемых данных в формате\n\n"

            << "\tEXPORT\n"
            << "\tДанная команда используется для выгрузки данныхв файл\n\n";
}

}  // namespace s21