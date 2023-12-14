#ifndef SRC_MODEL_ABSTRACT_KEY_VALUE_STORE_ABSTRACT_KEY_VALUE_STORE_H_
#define SRC_MODEL_ABSTRACT_KEY_VALUE_STORE_ABSTRACT_KEY_VALUE_STORE_H_

#include <atomic>
#include <optional>
#include <string>
#include <vector>

#include "../../types.h"

namespace s21 {

class AbstractKeyValueStore {
 public:
  AbstractKeyValueStore() = default;
  virtual ~AbstractKeyValueStore() = default;

  virtual Errors set(const std::string& key, const Value& value,
                     int ttl = hasNoTtl) = 0;
  virtual std::optional<Value> get(const Key& key) = 0;
  virtual bool exists(const std::string& key) = 0;
  virtual Errors del(const std::string& key) = 0;
  virtual Errors update(const Key& key, const Value& value, const int ttl,
                        const int paramsMask) = 0;
  virtual Errors rename(const std::string& oldKey,
                        const std::string& newKey) = 0;
  virtual int Ttl(const std::string& key) = 0;
  virtual int upload(const std::string& filename) = 0;
  virtual int exportValues(const std::string& filename) = 0;

  virtual const std::vector<std::string> keys() = 0;
  virtual const std::vector<std::string> find(const Value& value, const int ttl,
                                              const int paramsMask) = 0;
  virtual const std::vector<Value> showall() = 0;

  int GetSize() { return countItems.load(); }

 protected:
  std::atomic<int> countItems{0};
};

}  // namespace s21

#endif  //  SRC_MODEL_ABSTRACT_KEY_VALUE_STORE_ABSTRACT_KEY_VALUE_STORE_H_