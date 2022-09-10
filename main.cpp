/**
 * @file main.cpp
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Test file for List.hpp
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <list>

#include "List.hpp"

struct A {
  std::string str;
  explicit A(const std::string &str) : str(str) { std::cout << "A(\"" << str << "\")\n"; }
  A(const A& o) : str(o.str) { std::cout << "A(\"" << str << "\") copy ctor\n"; }
  A(A&& o) : str(std::move(o.str)) {
    std::cout << "A(\"" << str << "\") move ctor \n";
    str += "moved";
    o.str = "empty becaus moved";
  }
  ~A() { std::cout << "~A(\"" << str << "\") destructor\n"; }
  void print() const { std::cout << "str =\"" << str << "\"\n"; }
};


struct B {
  int i;
  explicit B(int i = 0) : i(i) { std::cout << "B(" << i << ")\n"; }
  B(const B& o) : i(o.i) { std::cout << "B(" << i << ") copy ctor\n"; }
  B(B&& o) : i(std::move(o.i)) { std::cout << "B(" << i << ") move ctor \n"; }
  ~B() { std::cout << "~B(" << i << ") destructor\n"; }
  void print() const { std::cout << "i =" << i << "\n"; }
};

void testAobj() {
  List<A> l;
  std::cout << "\npush back test\n";
  for (int i = 0; i < 5; i++) {
    l.push_back(A("push_back=" + std::to_string(i)));
  }
  l.push_front(A("push_front =-1"));

  {
    const A ct("push_back with copyctor");
    l.push_back(ct);
  
  }

  auto ins = l.begin();
  ins++;
  ins++;

  ins = l.insert(ins, A("Insert"));
  ins->print();

  std::cout << "\ncall print with const_iterator\n";
  //*
  for (auto it = l.cbegin(); it != l.cend(); ++it) {
    it->print();  // print use const_iterator
  }
  //*/
}

void testAobj_1() {
  List<A> l;
  std::cout << "\npush back test\n";

  l.push_back(A("test"));
  A a("test2");
  l.push_back(a);
}

void testBobj() {
  List<B> l;
  std::cout << "\npush back test\n";
  for (int i = 0; i < 5; i++) {
    l.push_back(B(i));
  }
  l.push_front(B(-1));

  {
    const B ct(50);
    l.push_back(ct);
  }

  std::cout << "\ncall print with const_iterator\n";
  for (auto it = l.cbegin(); it != l.cend(); it++) {
    it->print();  // print use const_iterator
  }
}

void testBobj_1() {
  List<B> l;
  //*
  for (size_t i = 0; i < 10; i++) {
    l.push_back(B(i));
  }
  //*/

  for (auto it = l.cbegin(); it != l.cend(); ++it) {
    it->print();
  }
  l.push_back(B(1));
  l.pop_back();
  std::cout << "pop\n";
}

void testuptr() {
  std::unique_ptr<A> p(new A("uptr"));

  List<std::unique_ptr<A>> l;
  l.push_back(std::move(p));
}

void testVect() {
  std::vector<B> v(5);
  std::vector<B> v2(5);

  std::cout << "\n\t fill vector\n";
  for (size_t i = 0; i < 5; i++) {
    v.push_back(B(i));
  }

  for (size_t i = 5; i < 10; i++) {
    v.push_back(B(i));
  }

  std::cout << "push to list\n";
  List<std::vector<B>> l;
  l.push_back(v2);
  l.push_front(v);

  std::cout << "print list\n";
  for (auto it = l.cbegin(); it != l.cend(); it++) {
    for (auto j = it->cbegin(); j != it->cend(); ++j) {
      j->print();
    }
  }
}

int main() {
  testAobj_1();
  testBobj_1();
  testuptr();
  testAobj();
  testVect();


  std::cout << "\nend test\n";

  return 0;
}