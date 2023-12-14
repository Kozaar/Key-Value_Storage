#ifndef SRC_DISPATCHERS_TTL_MANAGER_H_
#define SRC_DISPATCHERS_TTL_MANAGER_H_

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "../../types.h"
#include "../abstract_key_value_store/abstract_key_value_store.h"
#include "dispatcher_base.h"

namespace s21 {

class TtlManager {
 public:
  TtlManager(TtlManager& other) = delete;
  TtlManager(TtlManager&& other) = delete;

  TtlManager& operator=(TtlManager& other) = delete;
  TtlManager& operator=(TtlManager&& other) = delete;

  static TtlManager& getInstance();
  void stop();
  void addNewContainer(AbstractKeyValueStore& container);
  void deleteContainer(AbstractKeyValueStore& container);
  void addOrUpdateNode(AbstractKeyValueStore& container, const Key& key,
                       int sec);
  void deleteNode(AbstractKeyValueStore& container, const Key& key);

 private:
  std::atomic_bool stopFlag_;
  std::mutex dispatcherMutex_;
  std::mutex mainThreadMutex_;
  std::thread* mainThread_;
  std::unordered_map<AbstractKeyValueStore*, Dispatcher> dispatchers_;

  TtlManager();
  ~TtlManager();
  void start();
  void doWork();
};

}  //  namespace s21

#endif  //  SRC_DISPATCHERS_TTL_MANAGER_H_