#ifndef SRC_MODEL_TYPES_H_
#define SRC_MODEL_TYPES_H_

#include <iostream>
#include <string>

namespace s21 {
typedef std::string Key;

struct Value {
  std::string lastname;
  std::string name;
  int year;
  std::string city;
  int coins;

  void Print() {
    std::cout << lastname << " " << name << " " << year << " " << city << " "
              << coins << std::endl;
  }
};

enum ContainerType { hashTable, rbtree };

enum Errors {
  noErrors = 0,
  keyAlreadyExists = -1,
  keyNotFound = -2,
  hasNoTtl = -3,
  canNotOpenFile = -4,
  corruptedFile = -5,
  unknownError = -10
};

enum ValueParam {
  pLastname = 1 << 0,
  pName = 1 << 1,
  pYear = 1 << 2,
  pCity = 1 << 3,
  pCoins = 1 << 4,
  pTtl = 1 << 5
};

}  // namespace s21

#endif  //  SRC_MODEL_TYPES_H_