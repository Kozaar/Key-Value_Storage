#include "data.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace s21 {

std::vector<std::pair<Key, Value>> Data::loadData(const std::string& fileName) {
  std::ifstream fin(fileName);
  std::vector<std::pair<Key, Value>> res;
  if (!fin.is_open()) {
    std::stringstream str;
    str << "File " << fileName << " not open\n";
    throw std::runtime_error(str.str().c_str());
  }
  std::string currentStr;
  std::pair<Key, Value> parsedStr;
  while (!fin.eof()) {
    std::getline(fin, currentStr);
    if (!currentStr.size()) {
      continue;
    }
    parsedStr = parseOneStr(currentStr);
    res.push_back(parsedStr);
  }
  fin.close();
  return res;
}

int Data::saveData(const std::string& fileName,
                   const std::vector<std::pair<Key, Value>>& values) {
  std::ofstream fout(fileName);
  if (!fout.is_open()) {
    return canNotOpenFile;
  }
  int res = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    fout << values[i].first << " \"" << values[i].second.lastname << "\" \""
         << values[i].second.name << "\" " << values[i].second.year << " \""
         << values[i].second.city << "\" " << values[i].second.coins << "\n";
    ++res;
  }
  fout.close();
  return res;
}

std::pair<Key, Value> Data::parseOneStr(std::string& str) {
  // std::cout << "parse " << str << "\n";
  const int requiredCount = 6;
  std::pair<Key, Value> res;
  std::vector<std::string> elems = tokenizer(str, ' ');
  if (elems.size() < requiredCount) {
    throw std::runtime_error("Corrupted file 1");
  }
  // std::cout << "tokens\n";
  // for (size_t i = 0; i < elems.size(); ++i) {
  //     std::cout << elems[i] << "\n";
  // }
  // std::cout << "erase\n";
  res.first = eraseOneWord(elems);
  // std::cout <<
  res.second.lastname = eraseOneWord(elems);
  res.second.name = eraseOneWord(elems);
  res.second.year = std::stoi(eraseOneWord(elems));
  res.second.city = eraseOneWord(elems);
  res.second.coins = std::stoi(eraseOneWord(elems));

  return res;
}

std::vector<std::string> Data::tokenizer(std::string& str, char token) {
  std::vector<std::string> elems;
  std::string::size_type beg = 0;
  for (std::string::size_type end = 0;
       (end = str.find(token, end)) != std::string::npos; ++end) {
    elems.push_back(str.substr(beg, end - beg));
    beg = end + 1;
    while (beg < str.size() && str[beg] == ' ') {
      ++beg;
    }
    if (beg >= str.size()) {
      std::runtime_error("Corrupted file 2");
    }
    end = beg - 1;
  }
  if (str.substr(beg) != "") {
    elems.push_back(str.substr(beg));
  }
  return elems;
}

std::string Data::eraseOneWord(std::vector<std::string>& elems) {
  if (!elems.size()) {
    throw std::runtime_error("Corrupted file 3");
  }
  std::string res;
  res = elems.front();
  elems.erase(elems.begin());
  if (res.front() == '"' && (res.back() != '"' || res.size() == 1)) {
    while (elems.size() &&
           elems.front().find_first_of('"') == std::string::npos) {
      res += " " + elems.front();
      elems.erase(elems.begin());
    }
    if (elems.size()) {
      res += " " + elems.front();
      elems.erase(elems.begin());
    } else {
      throw std::runtime_error("Corrupted file 4");
    }
  }
  for (size_t i = res.find_first_of('"'); i != std::string::npos;
       i = res.find_first_of('"')) {
    res.erase(i, 1);
  }
  return res;
}

}  //  namespace s21
