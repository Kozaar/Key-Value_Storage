#pragma once

#include <climits>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>

#include "../controller/controller.h"

namespace s21 {
class Interface {
  enum Command {
    SET,
    GET,
    EXISTS,
    DEL,
    UPDATE,
    KEYS,
    RENAME,
    TTL,
    FIND,
    SHOWALL,
    UPLOAD,
    EXPORT,
    HELP,
    RETURN,
    ERROR
  };

 public:
  Interface();
  void ShowMainMenu();

 private:
  void WaitingForInput();
  void ShowWrongInputAttention();
  void ShowHelpMenu();
  void StorageStart();
  void SetRegexMap();
  std::vector<std::string> SplitBySpace(const std::string &);
  bool IsValidCommand(std::string, std::string);
  Command GetCommandNum(const char *);
  void Set(const std::vector<std::string> &);
  void Get(const std::vector<std::string> &);
  void Exists(const std::vector<std::string> &);
  void Del(const std::vector<std::string> &);
  void Update(const std::vector<std::string> &);
  void Keys();
  void Rename(const std::vector<std::string> &);
  void Ttl(const std::vector<std::string> &);
  void Find(const std::vector<std::string> &);
  void Showall();
  void Upload(const std::vector<std::string> &);
  void Export(const std::vector<std::string> &);

  std::unique_ptr<Controller> storage;
  std::map<std::string, std::regex> regexMap;
};
}  // namespace s21