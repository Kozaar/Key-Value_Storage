#include "controller.h"

namespace s21 {

Controller::Controller(const ContainerType type) {
  if (type == hashTable) {
    storage_ = new HashTable();
  } else if (type == rbtree) {
    storage_ = new SelfBalancingBinarySearchTree();
  }
};

Controller::~Controller() { delete storage_; }

Errors Controller::set(const std::string& key, const Value& value, int ttl) {
  return storage_->set(key, value, ttl);
}

std::optional<Value> Controller::get(const std::string& key) {
  return storage_->get(key);
}

bool Controller::exists(const std::string& key) {
  return storage_->exists(key);
}

Errors Controller::del(const std::string& key) { return storage_->del(key); }

Errors Controller::update(const Key& key, const Value& value, const int ttl,
                          const int paramsMask) {
  return storage_->update(key, value, ttl, paramsMask);
}

Errors Controller::rename(const std::string& oldKey,
                          const std::string& newKey) {
  return storage_->rename(oldKey, newKey);
}

int Controller::Ttl(const std::string& key) { return storage_->Ttl(key); }

int Controller::upload(const std::string& filename) {
  return storage_->upload(filename);
}

int Controller::exportValues(const std::string& filename) {
  return storage_->exportValues(filename);
}

const std::vector<std::string> Controller::keys() { return storage_->keys(); }

const std::vector<std::string> Controller::find(const Value& value,
                                                const int ttl,
                                                const int paramsMask) {
  return storage_->find(value, ttl, paramsMask);
}

const std::vector<Value> Controller::showall() { return storage_->showall(); }

int Controller::GetSize() { return storage_->GetSize(); }

}  //  namespace s21
