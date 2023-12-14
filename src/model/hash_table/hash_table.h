#ifndef SRC_HASH_TABLE_HASH_TABLE_H_
#define SRC_HASH_TABLE_HASH_TABLE_H_

#include <memory>
#include <mutex>

#include "../abstract_key_value_store/abstract_key_value_store.h"
#include "../dispatchers/dispatcher_base.h"

namespace s21 {
class HashTable : public AbstractKeyValueStore {
 public:
  struct Item {
    Key ItemKey;
    Value ItemValue;
    time_t TimeToDel;
    std::shared_ptr<Item> NextItem;

    Item(Key key, Value value, time_t timeToDel)
        : ItemKey(key),
          ItemValue(value),
          TimeToDel(timeToDel),
          NextItem(nullptr) {}
  };

  typedef size_t HashKey;

  HashTable();
  ~HashTable() override;

  Errors set(const std::string& key, const Value& value,
             int ttl = hasNoTtl) override;
  std::optional<Value> get(const std::string& key) override;
  bool exists(const std::string& key) override;
  Errors del(const std::string& key) override;
  Errors update(const Key& key, const Value& value, const int ttl,
                const int paramsMask) override;
  Errors rename(const std::string& oldKey, const std::string& newKey) override;
  int Ttl(const std::string& key) override;
  int upload(const std::string& filename) override;
  int exportValues(const std::string& filename) override;

  const std::vector<std::string> keys() override;
  const std::vector<std::string> find(const Value& value, const int ttl,
                                      const int paramsMask) override;
  const std::vector<Value> showall() override;

  int GetSize() { return countItems.load(); }

 private:
  std::vector<std::shared_ptr<Item>> m_storage;
  std::mutex m_nodeMutex;

  HashKey HashFunction(const Key& key) const;
  const std::shared_ptr<Item> FindItem(const Key& key);
};
}  //  namespace s21

#endif  //  SRC_HASH_TABLE_HASH_TABLE_H_