#include "ttl_manager.h"

#include <chrono>

namespace s21 {

TtlManager& TtlManager::getInstance() {
  static TtlManager inst;
  inst.start();
  return inst;
}

void TtlManager::start() {
  std::lock_guard<std::mutex> lock(mainThreadMutex_);
  if (!mainThread_) {
    mainThread_ = new std::thread(&TtlManager::doWork, std::ref(*this));
  }
}

void TtlManager::stop() { stopFlag_ = true; }

void TtlManager::addNewContainer(AbstractKeyValueStore& container) {
  Dispatcher disp;
  disp.Activate(&container);
  std::pair<AbstractKeyValueStore*, Dispatcher> pair(&container, disp);
  std::lock_guard<std::mutex> lock(dispatcherMutex_);
  dispatchers_.insert((pair));
}

void TtlManager::deleteContainer(AbstractKeyValueStore& container) {
  std::lock_guard<std::mutex> lock(dispatcherMutex_);
  dispatchers_.erase(&container);
}

void TtlManager::addOrUpdateNode(AbstractKeyValueStore& container,
                                 const Key& key, int sec) {
  if (sec > 0) {
    std::lock_guard<std::mutex> lock(dispatcherMutex_);
    dispatchers_[&container].AddOrUpdateObservableValue(key, sec);
  } else {
    deleteNode(container, key);
  }
}

void TtlManager::deleteNode(AbstractKeyValueStore& container, const Key& key) {
  std::lock_guard<std::mutex> lock(dispatcherMutex_);
  dispatchers_[&container].DeleteKeyFromObserv(key);
}

TtlManager::TtlManager() : stopFlag_(false), mainThread_(nullptr) {}

TtlManager::~TtlManager() {
  stop();
  std::lock_guard<std::mutex> lock(mainThreadMutex_);
  if (mainThread_) {
    mainThread_->join();
  }
  delete mainThread_;
}

void TtlManager::doWork() {
  while (stopFlag_.load() == false) {
    auto start = std::chrono::high_resolution_clock::now();
    {
      std::lock_guard<std::mutex> lock(dispatcherMutex_);
      for (auto i = dispatchers_.begin(); i != dispatchers_.end(); ++i) {
        i->second.Update();
      }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto time = finish - start;
    if (time < std::chrono::seconds{1}) {
      std::this_thread::sleep_for(std::chrono::seconds{1} - time);
    }
  }
}

}  //  namespace s21
