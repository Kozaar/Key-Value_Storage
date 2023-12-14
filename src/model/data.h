#ifndef SRC_MODEL_DATA_H_
#define SRC_MODEL_DATA_H_

#include <string>
#include <vector>

#include "../types.h"

namespace s21 {
class Data {
 public:
  Data() = default;

  static std::vector<std::pair<Key, Value>> loadData(
      const std::string& fileName);
  static int saveData(const std::string& fileName,
                      const std::vector<std::pair<Key, Value>>& values);

 private:
  static std::pair<Key, Value> parseOneStr(std::string& str);
  static std::vector<std::string> tokenizer(std::string& str, char token);
  static std::string eraseOneWord(std::vector<std::string>& elems);
};

}  //  namespace s21

#endif  //  SRC_MODEL_DATA_H_