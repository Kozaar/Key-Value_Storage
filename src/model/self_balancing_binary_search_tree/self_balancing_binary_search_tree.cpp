#include "self_balancing_binary_search_tree.h"

#include <cstring>
#include <queue>

#include "../data.h"
#include "../dispatchers/ttl_manager.h"

namespace s21 {

SelfBalancingBinarySearchTree::SelfBalancingBinarySearchTree() {
  root = nullptr;
  TtlManager::getInstance().addNewContainer(*this);
}

SelfBalancingBinarySearchTree::~SelfBalancingBinarySearchTree() {
  TtlManager::getInstance().deleteContainer(*this);
  clearTree();
}

Errors SelfBalancingBinarySearchTree::set(const std::string &key,
                                          const Value &value, int ttl) {
  Node *node = new Node;
  {
    std::lock_guard<std::mutex> lock(nodeMutex);
    node->key = key;
    node->val = value;
    node->timeToDel =
        ttl > 0 ? (time(nullptr) + ttl) : static_cast<int>(hasNoTtl);
    node->color = red;
    node->leftChild = nullptr;
    node->rightChild = nullptr;
    node->parent = nullptr;
    if (!findPlaceForNewNode(node)) {
      delete node;
      return keyAlreadyExists;
    }
    insertCase1(node);
    ++countItems;
  }
  if (ttl > 0) {
    TtlManager::getInstance().addOrUpdateNode(*this, key, ttl);
  }
  return noErrors;
}

std::optional<Value> SelfBalancingBinarySearchTree::get(
    const std::string &key) {
  std::lock_guard<std::mutex> lock(nodeMutex);
  Node *n = findNode(key);
  if (n) {
    return n->val;
  } else {
    return std::nullopt;
  }
}

bool SelfBalancingBinarySearchTree::exists(const std::string &key) {
  std::lock_guard<std::mutex> lock(nodeMutex);
  if (findNode(key) != nullptr) {
    return true;
  }
  return false;
}

Errors SelfBalancingBinarySearchTree::del(const std::string &key) {
  Node *n = nullptr;
  bool hasTtl = false;
  {
    std::lock_guard<std::mutex> lock(nodeMutex);
    n = findNode(key);
    if (!n) {
      return keyNotFound;
    }
    if (n->timeToDel > time(nullptr)) {
      hasTtl = true;
    }
  }
  if (hasTtl) {
    TtlManager::getInstance().deleteNode(*this, key);
  }
  std::lock_guard<std::mutex> lock(nodeMutex);
  Node *replacedNode = nullptr;
  if (n->leftChild && n->rightChild) {
    replacedNode = n->leftChild;
    while (replacedNode->rightChild) {
      replacedNode = replacedNode->rightChild;
    }
  }
  Node *child = n->leftChild ? n->leftChild : n->rightChild;

  if (replacedNode) {
    child = replacedNode->leftChild ? replacedNode->leftChild
                                    : replacedNode->rightChild;
    if (child) {
      child->parent = replacedNode->parent;
    }
    if (replacedNode->color == black) {
      if (child && child->color == red) {
        child->color = black;
      } else {
        deleteCase1(replacedNode);
      }
    }
    n->key = replacedNode->key;
    n->val = replacedNode->val;
    n = replacedNode;
  } else {
    if (child) {
      child->parent = n->parent;
    }
    if (n->color == black) {
      if (child && child->color == red) {
        child->color = black;
        if (n == root) {
          root = child;
        }
      } else {
        deleteCase1(n);
      }
    }
  }

  if (n->parent && n == n->parent->leftChild) {
    n->parent->leftChild = child;
  } else if (n->parent && n == n->parent->rightChild) {
    n->parent->rightChild = child;
  } else if (n->parent) {
    return unknownError;
  }
  delete n;
  --countItems;
  return noErrors;
}

Errors SelfBalancingBinarySearchTree::update(const Key &key, const Value &value,
                                             const int ttl,
                                             const int paramsMask) {
  Node *n = nullptr;
  bool needUpdateDispatcher = false;
  {
    std::lock_guard<std::mutex> lock(nodeMutex);
    n = findNode(key);
    if (!n) {
      return keyNotFound;
    }
    if (paramsMask & pLastname) {
      n->val.lastname = value.lastname;
    }
    if (paramsMask & pName) {
      n->val.name = value.name;
    }
    if (paramsMask & pYear) {
      n->val.year = value.year;
    }
    if (paramsMask & pCity) {
      n->val.city = value.city;
    }
    if (paramsMask & pCoins) {
      n->val.coins = value.coins;
    }
    if (paramsMask & pTtl) {
      n->timeToDel = ttl > 0 ? (time(nullptr) + ttl) : 0;
      needUpdateDispatcher = true;
    }
  }
  if (needUpdateDispatcher) {
    TtlManager::getInstance().addOrUpdateNode(*this, key, ttl);
  }
  return noErrors;
}

Errors SelfBalancingBinarySearchTree::rename(const std::string &oldKey,
                                             const std::string &newKey) {
  Node *n = nullptr;
  {
    std::lock_guard<std::mutex> lock(nodeMutex);
    n = findNode(oldKey);
    if (!n) {
      return keyNotFound;
    }
  }
  Errors res = set(newKey, n->val, n->timeToDel);
  if (res != noErrors) {
    return res;
  }
  return del(n->key);
}

int SelfBalancingBinarySearchTree::Ttl(const std::string &key) {
  std::lock_guard<std::mutex> lock(nodeMutex);
  Node *n = findNode(key);
  if (!n) {
    return keyNotFound;
  }
  return n->timeToDel > 0 ? (n->timeToDel - time(nullptr))
                          : static_cast<int>(hasNoTtl);
}

const std::vector<std::string> SelfBalancingBinarySearchTree::keys() {
  std::vector<std::string> res;
  std::lock_guard<std::mutex> lock(nodeMutex);
  Node *it = findMin(root);
  while (it) {
    res.push_back(it->key);
    it = nextElem(it);
  }
  return res;
}

int SelfBalancingBinarySearchTree::upload(const std::string &filename) {
  std::vector<std::pair<Key, Value>> values;
  try {
    values = Data::loadData(filename);
  } catch (const std::exception &e) {
    if (strstr(e.what(), "not open")) {
      return canNotOpenFile;
    }
    if (strstr(e.what(), "Corrupted")) {
      return corruptedFile;
    }
    return unknownError;
  }
  for (size_t i = 0; i < values.size(); ++i) {
    set(values[i].first, values[i].second);
  }
  return values.size();
}

int SelfBalancingBinarySearchTree::exportValues(const std::string &filename) {
  std::lock_guard<std::mutex> lock(nodeMutex);
  Node *it = findMin(root);
  std::vector<std::pair<Key, Value>> values;
  if (it) {
    do {
      values.push_back(std::pair<Key, Value>(it->key, it->val));
      it = nextElem(it);
    } while (it);
  }
  return Data::saveData(filename, values);
}

const std::vector<std::string> SelfBalancingBinarySearchTree::find(
    const Value &value, const int ttl, const int paramsMask) {
  std::vector<std::string> res;
  std::lock_guard<std::mutex> lock(nodeMutex);
  if (!root) {
    return res;
  }
  Node *it = findMin(root);
  while (it) {
    if ((!(paramsMask & pLastname) || it->val.lastname == value.lastname) &&
        (!(paramsMask & pName) || it->val.name == value.name) &&
        (!(paramsMask & pYear) || it->val.year == value.year) &&
        (!(paramsMask & pCity) || it->val.city == value.city) &&
        (!(paramsMask & pCoins) || it->val.coins == value.coins) &&
        (!(paramsMask & pTtl) || it->timeToDel == (time(nullptr) + ttl))) {
      res.push_back(it->key);
    }
    it = nextElem(it);
  }
  return res;
}

const std::vector<Value> SelfBalancingBinarySearchTree::showall() {
  std::vector<Value> res;
  std::lock_guard<std::mutex> lock(nodeMutex);
  if (!root) {
    return res;
  }
  Node *it = findMin(root);
  while (it) {
    res.push_back(it->val);
    it = nextElem(it);
  }
  return res;
}

void SelfBalancingBinarySearchTree::clearTree() {
  Key curRoot;
  while (countItems.load()) {
    {
      std::lock_guard<std::mutex> lock(nodeMutex);
      curRoot = root->key;
    }
    del(curRoot);
  }
}

bool SelfBalancingBinarySearchTree::findPlaceForNewNode(Node *newNode) {
  Node *curRoot = root;
  if (!curRoot) {
    root = newNode;
    return true;
  }
  do {
    if (newNode->key == curRoot->key) {
      return false;
    }
    newNode->parent = curRoot;
    if (newNode->key < curRoot->key) {
      curRoot = curRoot->leftChild;
    } else {
      curRoot = curRoot->rightChild;
    }
  } while (curRoot);
  curRoot = newNode->parent;
  if (newNode->key < curRoot->key) {
    curRoot->leftChild = newNode;
  } else {
    curRoot->rightChild = newNode;
  }
  return true;
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::grandParent(
    const Node &n) {
  if (n.parent != nullptr) {
    return n.parent->parent;
  } else {
    return nullptr;
  }
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::uncle(
    const Node &n) {
  Node *gp = grandParent(n);
  if (gp == nullptr) {
    return nullptr;
  }
  if (n.parent == gp->leftChild) {
    return gp->rightChild;
  } else {
    return gp->leftChild;
  }
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::sibling(
    const Node *n) const {
  if (!n || !n->parent) {
    return nullptr;
  }
  if (n == n->parent->leftChild) {
    return n->parent->rightChild;
  } else {
    return n->parent->leftChild;
  }
}

void SelfBalancingBinarySearchTree::rotateLeft(Node *n) {
  Node *newRoot = n->rightChild;

  newRoot->parent = n->parent;
  if (!n->parent) {
    root = newRoot;
  }
  if (n->parent != nullptr) {
    if (n->parent->leftChild == n) {
      n->parent->leftChild = newRoot;
    } else {
      n->parent->rightChild = newRoot;
    }
  }

  n->rightChild = newRoot->leftChild;
  if (newRoot->leftChild != nullptr) {
    newRoot->leftChild->parent = n;
  }

  n->parent = newRoot;
  newRoot->leftChild = n;
}

void SelfBalancingBinarySearchTree::rotateRight(Node *n) {
  Node *newRoot = n->leftChild;

  newRoot->parent = n->parent;
  if (!n->parent) {
    root = newRoot;
  }
  if (n->parent != nullptr) {
    if (n->parent->leftChild == n) {
      n->parent->leftChild = newRoot;
    } else {
      n->parent->rightChild = newRoot;
    }
  }

  n->leftChild = newRoot->rightChild;
  if (newRoot->rightChild != nullptr) {
    newRoot->rightChild->parent = n;
  }

  n->parent = newRoot;
  newRoot->rightChild = n;
}

void SelfBalancingBinarySearchTree::insertCase1(Node *n) {
  if (n->parent == nullptr) {
    n->color = black;
  } else {
    insertCase2(n);
  }
}

void SelfBalancingBinarySearchTree::insertCase2(Node *n) {
  if (n->parent->color == black) {
    return;
  } else {
    insertCase3(n);
  }
}

void SelfBalancingBinarySearchTree::insertCase3(Node *n) {
  Node *u = uncle((*n));

  if ((u != nullptr) && (u->color == red)) {
    n->parent->color = black;
    u->color = black;
    Node *g = grandParent((*n));
    g->color = red;
    insertCase1(g);
  } else {
    insertCase4(n);
  }
}

void SelfBalancingBinarySearchTree::insertCase4(Node *n) {
  Node *gp = grandParent((*n));

  if ((n == n->parent->rightChild) && (n->parent == gp->leftChild)) {
    rotateLeft(n->parent);
    n = n->leftChild;
  } else if ((n == n->parent->leftChild) && (n->parent == gp->rightChild)) {
    rotateRight(n->parent);
    n = n->rightChild;
  }
  insertCase5(n);
}

void SelfBalancingBinarySearchTree::insertCase5(Node *n) {
  Node *g = grandParent(*n);

  n->parent->color = black;
  g->color = red;
  if ((n == n->parent->leftChild) && (n->parent == g->leftChild)) {
    rotateRight(g);
  } else {
    rotateLeft(g);
  }
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::findNode(
    const std::string &key) const {
  Node *cur = root;
  while (cur && cur->key != key) {
    if (cur->key > key) {
      cur = cur->leftChild;
    } else {
      cur = cur->rightChild;
    }
  }
  if (cur) {
    return cur;
  } else {
    return nullptr;
  }
}

Errors SelfBalancingBinarySearchTree::deleteCase1(Node *n) {
  if (n->parent) {
    return deleteCase2(n);
  }
  if (countItems == 1) {
    root = nullptr;
  }
  return noErrors;
}

Errors SelfBalancingBinarySearchTree::deleteCase2(Node *n) {
  Node *s = sibling(n);

  if (s && s->color == red) {
    n->parent->color = red;
    s->color = black;
    if (n == n->parent->leftChild) {
      rotateLeft(n->parent);
    } else {
      rotateRight(n->parent);
    }
  }
  return deleteCase3(n);
}

Errors SelfBalancingBinarySearchTree::deleteCase3(Node *n) {
  Node *s = sibling(n);

  if ((n->parent->color == black) && s && (s->color == black) &&
      (!s->leftChild || s->leftChild->color == black) &&
      (!s->rightChild || s->rightChild->color == black)) {
    s->color = red;
    return deleteCase1(n->parent);
  } else
    return deleteCase4(n);
}

Errors SelfBalancingBinarySearchTree::deleteCase4(Node *n) {
  Node *s = sibling(n);

  if ((n->parent->color == red) && s && (s->color == black) &&
      (!s->leftChild || s->leftChild->color == black) &&
      (!s->rightChild || s->rightChild->color == black)) {
    s->color = red;
    n->parent->color = black;
  } else {
    return deleteCase5(n);
  }
  return noErrors;
}

Errors SelfBalancingBinarySearchTree::deleteCase5(Node *n) {
  Node *s = sibling(n);

  if (s && s->color == black) {
    if ((n == n->parent->leftChild) &&
        (s->rightChild == nullptr || s->rightChild->color == black) &&
        (s->leftChild && s->leftChild->color == red)) {
      s->color = red;
      s->leftChild->color = black;
      rotateRight(s);
    } else if ((n == n->parent->rightChild) &&
               (!s->leftChild || s->leftChild->color == black) &&
               (s->rightChild && s->rightChild->color == red)) {
      s->color = red;
      s->rightChild->color = black;
      rotateLeft(s);
    }
  }
  return deleteCase6(n);
}

Errors SelfBalancingBinarySearchTree::deleteCase6(Node *n) {
  Node *s = sibling(n);

  if (s) {
    s->color = n->parent->color;
    n->parent->color = black;

    if (n == n->parent->leftChild && s->rightChild) {
      s->rightChild->color = black;
      rotateLeft(n->parent);
    } else if (n == n->parent->rightChild && s->leftChild) {
      s->leftChild->color = black;
      rotateRight(n->parent);
    } else {
      return unknownError;
    }
  } else {
    return unknownError;
  }
  return noErrors;
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::findMin(
    Node *n) const {
  Node *it = n;
  if (it) {
    while (it->leftChild) {
      it = it->leftChild;
    }
  }
  return it;
}

SelfBalancingBinarySearchTree::Node *SelfBalancingBinarySearchTree::nextElem(
    Node *n) const {
  Node *it = n;
  const Node *last = root;
  while (last->rightChild) {
    last = last->rightChild;
  }

  if (it->rightChild) {
    it = it->rightChild;
    while (it->leftChild) {
      it = it->leftChild;
    }
  } else if (it->parent && it != last) {
    Node *old_node = it;
    it = it->parent;
    while (it->parent && it->rightChild == old_node) {
      old_node = it;
      it = it->parent;
    }
  } else if (it == last) {
    it = nullptr;
  } else {
    return nullptr;
  }
  return it;
}

}  //  namespace s21
