#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

#include "../model/self_balancing_binary_search_tree/self_balancing_binary_search_tree.h"
#include "../types.h"

void fillTree(s21::SelfBalancingBinarySearchTree& tree) {
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  tree.set("10", v);
  tree.set("20", v);
  tree.set("30", v);
  tree.set("17", v);
  tree.set("16", v);
  tree.set("18", v);
  tree.set("31", v);
  tree.set("32", v);
  tree.set("33", v);
  tree.set("34", v);
  tree.set("21", v);
  tree.set("22", v);
  tree.set("35", v);
  tree.set("44", v);
  tree.set("55", v);
}

TEST(rbtree, set_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  ASSERT_EQ(tree.set("10", v), s21::keyAlreadyExists);
  ASSERT_EQ(tree.set("11", v), s21::noErrors);
  ASSERT_EQ(tree.set("12", v), s21::noErrors);

  std::vector<std::string> keys = tree.keys();
  std::vector<std::string> expKeys{"10", "11", "12", "16", "17", "18",
                                   "20", "21", "22", "30", "31", "32",
                                   "33", "34", "35", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    ASSERT_STREQ(expKeys[i].c_str(), keys[i].c_str());
  }
}

TEST(rbtree, get_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  ASSERT_EQ(tree.set("12", v), s21::noErrors);

  std::optional<s21::Value> opt = tree.get("12");
  ASSERT_TRUE(opt.has_value());
  s21::Value val = opt.value();

  ASSERT_STREQ(val.lastname.c_str(), v.lastname.c_str());
  ASSERT_STREQ(val.name.c_str(), v.name.c_str());
  ASSERT_EQ(val.year, v.year);
  ASSERT_STREQ(val.city.c_str(), v.city.c_str());
  ASSERT_EQ(val.coins, v.coins);

  opt.reset();
  opt = tree.get("120");
  ASSERT_FALSE(opt.has_value());
}

TEST(rbtree, exists_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  ASSERT_TRUE(tree.exists("10"));
  ASSERT_FALSE(tree.exists("120"));
}

TEST(rbtree, del_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  ASSERT_EQ(tree.del("16"), s21::noErrors);
  ASSERT_EQ(tree.del("34"), s21::noErrors);
  ASSERT_EQ(tree.del("35"), s21::noErrors);
  ASSERT_EQ(tree.del("35"), s21::keyNotFound);

  std::vector<std::string> keys = tree.keys();
  std::vector<std::string> expKeys{"10", "17", "18", "20", "21", "22",
                                   "30", "31", "32", "33", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    ASSERT_STREQ(expKeys[i].c_str(), keys[i].c_str());
  }
}

TEST(rbtree, update_test) {
  s21::SelfBalancingBinarySearchTree tree;
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  tree.set("10", v);
  tree.set("20", v);
  tree.set("30", v);

  s21::Value newVal;
  newVal.city = "Nsk";
  newVal.coins = 300;
  newVal.lastname = "Ivanov";
  newVal.name = "Petr";
  newVal.year = v.year;

  int bitmask = (s21::pLastname | s21::pName | s21::pCity | s21::pCoins);

  ASSERT_EQ(tree.update(s21::Key("30"), newVal, 0, bitmask), s21::noErrors);
  ASSERT_EQ(tree.update("35", newVal, 0, bitmask), s21::keyNotFound);

  std::vector<s21::Value> values = tree.showall();
  std::vector<s21::Value> expValues{v, v, newVal};
  ASSERT_EQ(values.size(), expValues.size());
  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_STREQ(expValues[i].lastname.c_str(), values[i].lastname.c_str());
    ASSERT_STREQ(expValues[i].name.c_str(), values[i].name.c_str());
    ASSERT_EQ(expValues[i].year, values[i].year);
    ASSERT_STREQ(expValues[i].city.c_str(), values[i].city.c_str());
    ASSERT_EQ(expValues[i].coins, values[i].coins);
  }
}

TEST(rbtree, rename_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  ASSERT_EQ(tree.rename("10", "11"), s21::noErrors);
  ASSERT_EQ(tree.rename("11", "16"), s21::keyAlreadyExists);
  ASSERT_EQ(tree.rename("12", "100"), s21::keyNotFound);

  std::vector<std::string> keys = tree.keys();
  std::vector<std::string> expKeys{"11", "16", "17", "18", "20",
                                   "21", "22", "30", "31", "32",
                                   "33", "34", "35", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    ASSERT_STREQ(expKeys[i].c_str(), keys[i].c_str());
  }
}

TEST(rbtree, ttl_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  ASSERT_EQ(tree.Ttl("10"), s21::hasNoTtl);

  int ttl = 10;
  int bitmask = s21::pTtl;
  ASSERT_EQ(tree.update(s21::Key("10"), s21::Value(), ttl, bitmask),
            s21::noErrors);

  ASSERT_GT(tree.Ttl("10"), 0);

  ASSERT_EQ(tree.Ttl("100"), s21::keyNotFound);
}

TEST(rbtree, upload_good_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  int numKeyInFile = 3;
  ASSERT_EQ(tree.upload("examples/ex1.txt"), numKeyInFile);

  std::vector<std::string> keys = tree.keys();
  std::vector<std::string> expKeys{
      "10", "16", "17", "18", "20", "21", "22",   "30",   "31",
      "32", "33", "34", "35", "44", "55", "key1", "key2", "key300500"};
  ASSERT_EQ(keys.size(), expKeys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    ASSERT_STREQ(expKeys[i].c_str(), keys[i].c_str());
  }
}

TEST(rbtree, upload_bad_filename_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  std::vector<std::string> oldKeys = tree.keys();
  std::vector<s21::Value> oldValues = tree.showall();

  ASSERT_EQ(tree.upload("examples/bad.file"), s21::canNotOpenFile);

  std::vector<std::string> newKeys = tree.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = tree.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(rbtree, upload_corrupted_file_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  std::vector<std::string> oldKeys = tree.keys();
  std::vector<s21::Value> oldValues = tree.showall();

  ASSERT_EQ(tree.upload("examples/corrupted.txt"), s21::corruptedFile);

  std::vector<std::string> newKeys = tree.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = tree.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(rbtree, upload_empty_file_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  std::vector<std::string> oldKeys = tree.keys();
  std::vector<s21::Value> oldValues = tree.showall();

  ASSERT_EQ(tree.upload("examples/empty.txt"), s21::corruptedFile);

  std::vector<std::string> newKeys = tree.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = tree.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(rbtree, export_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  std::vector<std::string> oldKeys = tree.keys();
  std::vector<s21::Value> oldValues = tree.showall();

  ASSERT_EQ(tree.exportValues("examples/test.txt"), oldKeys.size());

  s21::SelfBalancingBinarySearchTree newTree;
  newTree.upload("examples/test.txt");

  std::vector<std::string> newKeys = newTree.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = newTree.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(rbtree, find_test) {
  s21::SelfBalancingBinarySearchTree tree;
  fillTree(tree);

  s21::Value v;
  v.city = "asd";
  v.coins = 234;
  v.lastname = "zxc";
  v.name = "vbn";
  v.year = 213;

  ASSERT_EQ(tree.set("11", v), s21::noErrors);
  ASSERT_EQ(tree.set("12", v), s21::noErrors);
  ASSERT_EQ(tree.set("13", v), s21::noErrors);

  int bitmask = (s21::pLastname | s21::pName | s21::pYear | s21::pCoins);

  std::vector<std::string> foundKeys = tree.find(v, 0, bitmask);
  std::vector<std::string> expKeys{"11", "12", "13"};
  ASSERT_EQ(foundKeys.size(), expKeys.size());
  for (size_t i = 0; i < foundKeys.size(); ++i) {
    ASSERT_STREQ(expKeys[i].c_str(), foundKeys[i].c_str());
  }
}