#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

#include "../model/hash_table/hash_table.h"
#include "../types.h"

void fillhashtable(s21::HashTable& hashtable) {
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  hashtable.set("10", v);
  hashtable.set("20", v);
  hashtable.set("30", v);
  hashtable.set("17", v);
  hashtable.set("16", v);
  hashtable.set("18", v);
  hashtable.set("31", v);
  hashtable.set("32", v);
  hashtable.set("33", v);
  hashtable.set("34", v);
  hashtable.set("21", v);
  hashtable.set("22", v);
  hashtable.set("35", v);
  hashtable.set("44", v);
  hashtable.set("55", v);
}

TEST(hashtable, set_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  ASSERT_EQ(hashtable.set("10", v), s21::keyAlreadyExists);
  ASSERT_EQ(hashtable.set("11", v), s21::noErrors);
  ASSERT_EQ(hashtable.set("12", v), s21::noErrors);

  std::vector<std::string> keys = hashtable.keys();
  std::vector<std::string> expKeys{"10", "11", "12", "16", "17", "18",
                                   "20", "21", "22", "30", "31", "32",
                                   "33", "34", "35", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
}

TEST(hashtable, get_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  ASSERT_EQ(hashtable.set("12", v), s21::noErrors);

  std::optional<s21::Value> opt = hashtable.get("12");
  ASSERT_TRUE(opt.has_value());
  s21::Value val = opt.value();

  ASSERT_STREQ(val.lastname.c_str(), v.lastname.c_str());
  ASSERT_STREQ(val.name.c_str(), v.name.c_str());
  ASSERT_EQ(val.year, v.year);
  ASSERT_STREQ(val.city.c_str(), v.city.c_str());
  ASSERT_EQ(val.coins, v.coins);

  opt.reset();
  opt = hashtable.get("120");
  ASSERT_FALSE(opt.has_value());
}

TEST(hashtable, exists_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  ASSERT_TRUE(hashtable.exists("10"));
  ASSERT_FALSE(hashtable.exists("120"));
}

TEST(hashtable, del_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  ASSERT_EQ(hashtable.del("16"), s21::noErrors);
  ASSERT_EQ(hashtable.del("34"), s21::noErrors);
  ASSERT_EQ(hashtable.del("35"), s21::noErrors);
  ASSERT_EQ(hashtable.del("35"), s21::keyNotFound);

  std::vector<std::string> keys = hashtable.keys();
  std::vector<std::string> expKeys{"10", "17", "18", "20", "21", "22",
                                   "30", "31", "32", "33", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
}

TEST(hashtable, update_test) {
  s21::HashTable hashtable;
  s21::Value v;
  v.city = "qwe";
  v.coins = 123;
  v.lastname = "asd";
  v.name = "zxc";
  v.year = 1236;
  hashtable.set("10", v);
  hashtable.set("20", v);
  hashtable.set("30", v);

  s21::Value newVal;
  newVal.city = "Nsk";
  newVal.coins = 300;
  newVal.lastname = "Ivanov";
  newVal.name = "Petr";
  newVal.year = v.year;

  int bitmask = (s21::pLastname | s21::pName | s21::pCity | s21::pCoins);

  ASSERT_EQ(hashtable.update(s21::Key("30"), newVal, 0, bitmask),
            s21::noErrors);
  ASSERT_EQ(hashtable.update("35", newVal, 0, bitmask), s21::keyNotFound);

  std::vector<s21::Value> values = hashtable.showall();
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

TEST(hashtable, rename_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  ASSERT_EQ(hashtable.rename("10", "11"), s21::noErrors);
  ASSERT_EQ(hashtable.rename("11", "16"), s21::keyAlreadyExists);
  ASSERT_EQ(hashtable.rename("12", "100"), s21::keyNotFound);

  std::vector<std::string> keys = hashtable.keys();
  std::vector<std::string> expKeys{"11", "16", "17", "18", "20",
                                   "21", "22", "30", "31", "32",
                                   "33", "34", "35", "44", "55"};
  ASSERT_EQ(keys.size(), expKeys.size());
}

TEST(hashtable, ttl_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  ASSERT_EQ(hashtable.Ttl("10"), s21::hasNoTtl);

  int ttl = 10;
  int bitmask = s21::pTtl;
  ASSERT_EQ(hashtable.update(s21::Key("10"), s21::Value(), ttl, bitmask),
            s21::noErrors);

  ASSERT_GT(hashtable.Ttl("10"), 0);

  ASSERT_EQ(hashtable.Ttl("100"), s21::keyNotFound);
}

TEST(hashtable, upload_good_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  int numKeyInFile = 3;
  ASSERT_EQ(hashtable.upload("examples/ex1.txt"), numKeyInFile);
}

TEST(hashtable, upload_bad_filename_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  std::vector<std::string> oldKeys = hashtable.keys();
  std::vector<s21::Value> oldValues = hashtable.showall();

  ASSERT_EQ(hashtable.upload("examples/bad.file"), s21::canNotOpenFile);

  std::vector<std::string> newKeys = hashtable.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = hashtable.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(hashtable, upload_corrupted_file_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  std::vector<std::string> oldKeys = hashtable.keys();
  std::vector<s21::Value> oldValues = hashtable.showall();

  ASSERT_EQ(hashtable.upload("examples/corrupted.txt"), s21::corruptedFile);

  std::vector<std::string> newKeys = hashtable.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = hashtable.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(hashtable, upload_empty_file_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  std::vector<std::string> oldKeys = hashtable.keys();
  std::vector<s21::Value> oldValues = hashtable.showall();

  ASSERT_EQ(hashtable.upload("examples/empty.txt"), s21::corruptedFile);

  std::vector<std::string> newKeys = hashtable.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = hashtable.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(hashtable, export_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  std::vector<std::string> oldKeys = hashtable.keys();
  std::vector<s21::Value> oldValues = hashtable.showall();

  ASSERT_EQ(hashtable.exportValues("examples/test.txt"), oldKeys.size());

  s21::HashTable newhashtable;
  newhashtable.upload("examples/test.txt");

  std::vector<std::string> newKeys = newhashtable.keys();
  ASSERT_EQ(oldKeys.size(), newKeys.size());
  for (size_t i = 0; i < newKeys.size(); ++i) {
    ASSERT_STREQ(newKeys[i].c_str(), oldKeys[i].c_str());
  }

  std::vector<s21::Value> newValues = newhashtable.showall();
  ASSERT_EQ(oldValues.size(), newValues.size());
  for (size_t i = 0; i < oldValues.size(); ++i) {
    ASSERT_STREQ(newValues[i].lastname.c_str(), oldValues[i].lastname.c_str());
    ASSERT_STREQ(newValues[i].name.c_str(), oldValues[i].name.c_str());
    ASSERT_EQ(newValues[i].year, oldValues[i].year);
    ASSERT_STREQ(newValues[i].city.c_str(), oldValues[i].city.c_str());
    ASSERT_EQ(newValues[i].coins, oldValues[i].coins);
  }
}

TEST(hashtable, find_test) {
  s21::HashTable hashtable;
  fillhashtable(hashtable);

  s21::Value v;
  v.city = "asd";
  v.coins = 234;
  v.lastname = "zxc";
  v.name = "vbn";
  v.year = 213;

  ASSERT_EQ(hashtable.set("11", v), s21::noErrors);
  ASSERT_EQ(hashtable.set("12", v), s21::noErrors);
  ASSERT_EQ(hashtable.set("13", v), s21::noErrors);

  int bitmask = (s21::pLastname | s21::pName | s21::pYear | s21::pCoins);

  std::vector<std::string> foundKeys = hashtable.find(v, 0, bitmask);
  std::vector<std::string> expKeys{"11", "12", "13"};
  ASSERT_EQ(foundKeys.size(), expKeys.size());
}