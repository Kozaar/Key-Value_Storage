#include "dispatcher_base.h"

namespace s21 {

namespace {
std::mutex delKeyMutex_;
std::mutex storageMutex_;
}  // namespace

Dispatcher::Dispatcher() : storage_(nullptr) {}
//----------------------------------------------------------------
void Dispatcher::Activate(AbstractKeyValueStore* storage) {
  storage_ = storage;
}
//-------------------c---------------------------------------------
void Dispatcher::AddOrUpdateObservableValue(const std::string& value,
                                            const int sec) {
  {
    std::lock_guard<std::mutex> lock(storageMutex_);
    time_t currentTime = time(0);
    time_t timeToDelete = currentTime + sec;
    observableValues_.insert({value, timeToDelete});
  }
  Update();
}
//----------------------------------------------------------------
void Dispatcher::Update() {
  std::vector<std::string> needDeleteValues;
  {
    std::lock_guard<std::mutex> lock(storageMutex_);
    time_t currentTime = time(0);
    for (auto it = observableValues_.begin(); it != observableValues_.end();
         ++it) {
      if (it->second <= currentTime) {
        needDeleteValues.push_back(it->first);
      }
    }
  }
  DeleteValues(needDeleteValues);
}
//----------------------------------------------------------------
void Dispatcher::DeleteKeyFromObserv(const std::string& value) {
  {
    std::lock_guard<std::mutex> lock(delKeyMutex_);
    if (deletingKey_ && *deletingKey_ == value) {
      return;
    }
  }
  {
    std::lock_guard<std::mutex> lock(storageMutex_);
    for (auto it = observableValues_.begin(); it != observableValues_.end();
         ++it) {
      if (it->first == value) {
        it = observableValues_.erase(it);
        return;
      }
    }
  }
}
//----------------------------------------------------------------
void Dispatcher::DeleteValues(
    const std::vector<std::string>& needDeleteValues) {
  for (auto it = needDeleteValues.begin(); it != needDeleteValues.end(); it++) {
    {
      std::lock_guard<std::mutex> lock(delKeyMutex_);
      deletingKey_ = std::make_shared<std::string>(*it);
    }
    {
      std::lock_guard<std::mutex> lock(storageMutex_);
      if (storage_->del(*it) == noErrors) {
        observableValues_.erase(*it);
      }
    }
    {
      std::lock_guard<std::mutex> lock(delKeyMutex_);
      deletingKey_.reset();
    }
  }
}

}  //  namespace s21
