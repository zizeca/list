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
#include <iterator>
#include <list>
#include <memory>

#include "List.hpp"

struct A {
  std::string str;
  explicit A(const std::string& str) : str(str) { std::cout << "A(\"" << str << "\")\n"; }
  A(const A& o) : str(o.str) { std::cout << "A(\"" << str << "\") copy ctor\n"; }
  A(A&& o) : str(std::move(o.str)) {
    std::cout << "A(\"" << str << "\") move ctor \n";
    str += "moved";
    o.str = "empty becaus moved";
  }
  A& operator=(const A& other) {
    str = other.str;
    std::cout << "A operator= copy assigned" << str << "\n";
    return *this;
  }
  A& operator=(A&& other) {
    str = std::move(other.str);
    std::cout << "A operator=move assigned" << str << "\n";
    return *this;
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

void testIteratorCast() {
  std::cout << "----Test cast pointer----\n";
  List<A> l;
  l.push_back(A("1"));
  l.push_back(A("2"));

  List<A>::iterator it = l.begin();
  List<A>::const_iterator cit = it;
  cit->print();
  *it = A("3");
  // List<A>::iterator it2 = cit; // error
  // it2->print();
}

void testAobj() {
  std::cout << "----Test A object----\n";
  List<A> l;
  std::cout << "--push back test--\n";
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

  std::cout << "--call print with const_iterator--\n";
  //*
  for (auto it = l.cbegin(); it != l.cend(); ++it) {
    it->print();  // print use const_iterator
  }
  std::cout << "--print reverse iterrator--\n";
  for (auto it = l.rcbegin(); it != l.rcend(); ++it) {
    it->print();  // print use const_iterator
  }
  //*/
}

void testAobj_1() {
  std::cout << "----Test A object one time----\n";
  List<A> l;
  l.push_back(A("test"));
  // A a("test2");
  // l.push_back(a);

  std::cout << "--compare std::list--\n";
  std::list<A> sl;
  sl.push_back(A("std test"));
}

void testBobj() {
  std::cout << "----Test B object----\n";
  List<B> l;
  std::cout << "--push back test--\n";
  for (int i = 0; i < 5; i++) {
    l.push_back(B(i));
  }
  l.push_front(B(-1));

  {
    const B ct(50);
    l.push_back(ct);
  }

  std::cout << "--call print with const_iterator--\n";
  for (auto it = l.cbegin(); it != l.cend(); it++) {
    it->print();  // print use const_iterator
  }
}

void testuptr() {
  std::cout << "----Test std::unique_ptr<A>----\n";
  std::unique_ptr<A> p1(new A("uptr1"));
  std::unique_ptr<A> p2(new A("uptr2"));
  std::unique_ptr<A> p3(new A("uptr3"));
  List<std::unique_ptr<A>> l;
  l.push_back(std::move(p1));
  l.push_back(std::move(p2));
  l.pop_front();
  l.push_front(std::move(p3));
}

void testVect() {
  std::cout << "----Test std::vector<B>----\n";
  std::vector<B> v;
  std::vector<B> v2;

  std::cout << "--fill vector--\n";
  for (size_t i = 0; i < 5; i++) {
    v.push_back(B(i));
  }

  for (size_t i = 5; i < 10; i++) {
    v2.push_back(B(i));
  }

  std::cout << "--push to list--\n";
  List<std::vector<B>> l;
  l.push_back(v2);
  l.push_front(v);

  std::cout << "--print list--\n";
  for (auto it = l.cbegin(); it != l.cend(); it++) {
    for (auto j = it->cbegin(); j != it->cend(); ++j) {
      j->print();
    }
  }
}

void testCopyList() {
  std::cout << "----Test copy List----\n";
  List<A> l1;
  l1.push_back(A("A"));
  List<A> l2;
  std::cout << "--List::opertor=--\n";
  l2 = l1;
  l1.front().print();
  l2.front().print();
}

void testErase() {
  std::cout << "----Test erase----\n";
  List<A> l;
  for (int i = 0; i < 10; i++) {
    l.push_back(A("push_back=" + std::to_string(i)));
  }

  List<A>::iterator it = l.begin();
  List<A>::iterator it2 = l.begin();

  std::advance(it, 2);
  std::advance(it2, 6);
  it->print();
  std::cout << "-erase one-\n";
  it = l.erase(it);
  it->print();
  std::cout << "-erase some-\n";
  it = l.erase(it, it2);
  it->print();
}

void testEmplace() {
  std::cout << "----Test emplace----\n";
  List<A> l;
  // std::list<A> l;
  l.emplace_back("test emplace back");
  // l.push_front(A("test push front"));
  l.emplace_front("test emplace front");
  l.emplace_back("test emplace back2 and print").print();
}

int main() {
  std::cout << "------start test------\n";
  try {
    // testAobj_1();
    // testBobj();
    testuptr();
    // testAobj();
    // testVect();
    // testIteratorCast();
    // testCopyList();
    // testErase();
    testEmplace();

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  std::cout << "------end test------\n";

  return 0;
}