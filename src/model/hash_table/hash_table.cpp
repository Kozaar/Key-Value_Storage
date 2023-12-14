#include "hash_table.h"

#include <climits>
#include <cstring>

#include "../data.h"
#include "../dispatchers/ttl_manager.h"

namespace s21 {

namespace {
constexpr size_t VectorSize = UCHAR_MAX;
}

using HashKey = HashTable::HashKey;

HashTable::HashTable() : m_storage(VectorSize, nullptr) {
  TtlManager::getInstance().addNewContainer(*this);
}
//----------------------------------------------------------------
HashTable::~HashTable() { TtlManager::getInstance().deleteContainer(*this); }
//----------------------------------------------------------------
HashKey HashTable::HashFunction(const Key& key) const {
  unsigned char sum = 0;
  for (auto it = key.begin(); it != key.end(); it++) sum += *it;
  return static_cast<HashKey>(sum);
}
//----------------------------------------------------------------
const std::shared_ptr<HashTable::Item> HashTable::FindItem(const Key& key) {
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  auto it = m_storage[HashFunction(key)];
  while (it != nullptr && key != it->ItemKey) it = it->NextItem;
  return it;
}
//----------------------------------------------------------------
Errors HashTable::set(const std::string& key, const Value& value, int ttl) {
  if (FindItem(key) != nullptr) return keyAlreadyExists;
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  const auto idx = HashFunction(key);
  Item newItem(key, value,
               ttl > 0 ? time(nullptr) + ttl : static_cast<time_t>(ttl));
  auto it = m_storage[idx];
  if (it == nullptr) {
    m_storage[idx] = std::make_shared<Item>(newItem);
  } else {
    while (it->NextItem != nullptr) it = it->NextItem;
    it->NextItem = std::make_shared<Item>(newItem);
  }

  if (ttl > 0) TtlManager::getInstance().addOrUpdateNode(*this, key, ttl);
  ++countItems;
  return noErrors;
}
//----------------------------------------------------------------
std::optional<Value> HashTable::get(const std::string& key) {
  auto Item = FindItem(key);
  if (Item != nullptr)
    return Item->ItemValue;
  else
    return std::nullopt;
}
//----------------------------------------------------------------
bool HashTable::exists(const std::string& key) {
  return FindItem(key) != nullptr;
}
//----------------------------------------------------------------
Errors HashTable::del(const std::string& key) {
  if (FindItem(key) == nullptr) return keyNotFound;
  bool needDeleteFromTtlManager = false;
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  auto idx = HashFunction(key);
  auto it = m_storage[idx];
  if (it->ItemKey == key) {
    m_storage[idx] = it->NextItem;
    needDeleteFromTtlManager = it->TimeToDel < 0;
  } else {
    while (it->NextItem->ItemKey != key) it = it->NextItem;
    needDeleteFromTtlManager = it->NextItem->TimeToDel < 0;
    it->NextItem = it->NextItem->NextItem;
  }
  --countItems;
  if (needDeleteFromTtlManager)
    TtlManager::getInstance().deleteNode(*this, key);
  return noErrors;
}
//----------------------------------------------------------------
Errors HashTable::update(const Key& key, const Value& value, const int ttl,
                         const int paramsMask) {
  auto it = FindItem(key);
  if (it == nullptr) return keyNotFound;
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  it->ItemValue.lastname =
      paramsMask & pLastname ? value.lastname : it->ItemValue.lastname;
  it->ItemValue.name = paramsMask & pName ? value.name : it->ItemValue.name;
  it->ItemValue.year = paramsMask & pYear ? value.year : it->ItemValue.year;
  it->ItemValue.city = paramsMask & pCity ? value.city : it->ItemValue.city;
  it->ItemValue.coins = paramsMask & pCoins ? value.coins : it->ItemValue.coins;

  if (paramsMask & pTtl) {
    it->TimeToDel = ttl > 0 ? (time(nullptr) + ttl) : 0;
    TtlManager::getInstance().addOrUpdateNode(*this, key, ttl);
  }
  return noErrors;
}
//----------------------------------------------------------------
Errors HashTable::rename(const std::string& oldKey, const std::string& newKey) {
  auto item = FindItem(oldKey);
  if (item == nullptr) return keyNotFound;
  Value value = item->ItemValue;
  auto ttl = item->TimeToDel;
  auto messageSet = set(newKey, value, ttl);
  if (messageSet != noErrors) return messageSet;
  return del(oldKey);
}
//----------------------------------------------------------------
int HashTable::Ttl(const std::string& key) {
  auto item = FindItem(key);
  if (item == nullptr) return keyNotFound;
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  return item->TimeToDel > 0 ? (item->TimeToDel - time(nullptr))
                             : static_cast<int>(hasNoTtl);
}
//----------------------------------------------------------------
int HashTable::upload(const std::string& filename) {
  try {
    std::vector<std::pair<Key, Value>> values(Data::loadData(filename));
    auto sizeBeforeUpload = countItems.load();
    for (size_t i = 0; i < values.size(); ++i)
      set(values[i].first, values[i].second);
    return countItems.load() - sizeBeforeUpload;
  } catch (const std::exception& e) {
    if (strstr(e.what(), "not open")) return canNotOpenFile;
    if (strstr(e.what(), "Corrupted")) return corruptedFile;
    return unknownError;
  }
}
//----------------------------------------------------------------
int HashTable::exportValues(const std::string& filename) {
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  std::vector<std::pair<Key, Value>> values;
  for (size_t idx = 0; idx < m_storage.size(); ++idx) {
    auto it = m_storage[idx];
    while (it != nullptr) {
      values.push_back(std::pair<Key, Value>(it->ItemKey, it->ItemValue));
      it = it->NextItem;
    }
  }
  return Data::saveData(filename, values);
}
//----------------------------------------------------------------
const std::vector<std::string> HashTable::keys() {
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  std::vector<std::string> allKeys;
  for (size_t idx = 0; idx < m_storage.size(); ++idx) {
    auto it = m_storage[idx];
    while (it != nullptr) {
      allKeys.push_back(it->ItemKey);
      it = it->NextItem;
    }
  }
  return allKeys;
}
//----------------------------------------------------------------
const std::vector<std::string> HashTable::find(const Value& value,
                                               const int ttl,
                                               const int paramsMask) {
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  std::vector<std::string> neededKeys;
  for (size_t idx = 0; idx < m_storage.size(); ++idx) {
    auto it = m_storage[idx];
    while (it != nullptr) {
      if ((!(paramsMask & pLastname) ||
           it->ItemValue.lastname == value.lastname) &&
          (!(paramsMask & pName) || it->ItemValue.name == value.name) &&
          (!(paramsMask & pYear) || it->ItemValue.year == value.year) &&
          (!(paramsMask & pCity) || it->ItemValue.city == value.city) &&
          (!(paramsMask & pCoins) || it->ItemValue.coins == value.coins) &&
          (!(paramsMask & pTtl) || it->TimeToDel == (time(nullptr) + ttl)))

        neededKeys.push_back(it->ItemKey);
      it = it->NextItem;
    }
  }
  return neededKeys;
}
//----------------------------------------------------------------
const std::vector<Value> HashTable::showall() {
  std::lock_guard<std::mutex> lock(m_nodeMutex);
  std::vector<Value> allValues;
  for (size_t idx = 0; idx < m_storage.size(); ++idx) {
    auto it = m_storage[idx];
    while (it != nullptr) {
      allValues.push_back(it->ItemValue);
      it = it->NextItem;
    }
  }
  return allValues;
}

}  //  namespace s21
