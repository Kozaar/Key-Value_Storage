#ifndef SRC_MODEL_CONTROLLER_CONTROLLER_H_
#define SRC_MODEL_CONTROLLER_CONTROLLER_H_

#include <memory>
#include <optional>

#include "../model/hash_table/hash_table.h"
#include "../model/self_balancing_binary_search_tree/self_balancing_binary_search_tree.h"
#include "../types.h"

namespace s21 {

class Controller {
 public:
  Controller() = delete;
  Controller(const ContainerType type);
  ~Controller();

  Errors set(const std::string& key, const Value& value, int ttl = 0);
  std::optional<Value> get(const std::string& key);
  bool exists(const std::string& key);
  Errors del(const std::string& key);
  Errors update(const Key& key, const Value& value, const int ttl,
                const int paramsMask);
  Errors rename(const std::string& oldKey, const std::string& newKey);
  int Ttl(const std::string& key);
  int upload(const std::string& filename);
  int exportValues(const std::string& filename);

  const std::vector<std::string> keys();
  const std::vector<std::string> find(const Value& value, const int ttl,
                                      const int paramsMask);
  const std::vector<Value> showall();

  int GetSize();

 private:
  AbstractKeyValueStore* storage_;
};

}  //  namespace s21

#endif  //  SRC_MODEL_CONTROLLER_CONTROLLER_H_