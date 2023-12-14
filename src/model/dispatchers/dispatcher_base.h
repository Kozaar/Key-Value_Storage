// Класс диспетчеризации данных, имеющих время существования
#ifndef SRC_DISPATCHERS_DISPATCHER_BASE_H_
#define SRC_DISPATCHERS_DISPATCHER_BASE_H_

#include <ctime>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../abstract_key_value_store/abstract_key_value_store.h"

namespace s21 {

class Dispatcher {
 public:
  Dispatcher();
  ~Dispatcher() = default;

  void Activate(AbstractKeyValueStore* storage);
  void AddOrUpdateObservableValue(const std::string& value, const int sec);
  void Update();
  void DeleteKeyFromObserv(const std::string& value);

 private:
  std::unordered_map<std::string, time_t> observableValues_;
  AbstractKeyValueStore* storage_;
  std::shared_ptr<std::string> deletingKey_;

  void DeleteValues(const std::vector<std::string>& needDeleteValues);
};
}  //  namespace s21

#endif  //  SRC_DISPATCHERS_DISPATCHER_BASE_H_