#ifndef SRC_MODEL_SELF_BALANCING_BINARY_SEARCH_THREE_SELF_BALANCING_BINARY_SEARCH_THREE_H_
#define SRC_MODEL_SELF_BALANCING_BINARY_SEARCH_THREE_SELF_BALANCING_BINARY_SEARCH_THREE_H_

#include <mutex>

#include "../abstract_key_value_store/abstract_key_value_store.h"
#include "../dispatchers/dispatcher_base.h"

namespace s21 {
class SelfBalancingBinarySearchTree : public AbstractKeyValueStore {
 private:
  enum Colors { red = 0, black };
  struct Node {
    Key key;
    Value val;
    time_t timeToDel;
    Node* parent;
    Node* leftChild;
    Node* rightChild;
    Colors color;

    bool for_print;
  };

 public:
  SelfBalancingBinarySearchTree();
  ~SelfBalancingBinarySearchTree();

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

 private:
  Node* root;
  std::mutex nodeMutex;

  void clearTree();
  bool findPlaceForNewNode(Node* newNode);
  Node* grandParent(const Node& n);
  Node* uncle(const Node& n);
  Node* sibling(const Node* n) const;
  void rotateLeft(Node* n);
  void rotateRight(Node* n);
  void insertCase1(Node* n);
  void insertCase2(Node* n);
  void insertCase3(Node* n);
  void insertCase4(Node* n);
  void insertCase5(Node* n);
  Node* findNode(const std::string& key) const;

  Errors deleteCase1(Node* n);
  Errors deleteCase2(Node* n);
  Errors deleteCase3(Node* n);
  Errors deleteCase4(Node* n);
  Errors deleteCase5(Node* n);
  Errors deleteCase6(Node* n);

  Node* findMin(Node* n) const;
  Node* nextElem(Node* n) const;
};

}  //  namespace s21

#endif  //  SRC_MODEL_SELF_BALANCING_BINARY_SEARCH_THREE_SELF_BALANCING_BINARY_SEARCH_THREE_H_