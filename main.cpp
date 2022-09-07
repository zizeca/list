#include <algorithm>
#include <iostream>
#include <memory>

#include "List.hpp"

struct A {
  std::string str;
  explicit A(std::string str = "default") : str(str) {
    std::cout << "A(\"" << str << "\")\n";
  }
  A(const A& o) : str(o.str) {
    std::cout << "A(\"" << str << "\") copy ctor\n";
  }
  A(A&& o) : str(std::move(o.str)) {
    std::cout << "A(\"" << str << "\") move ctor \n";
    str += "moved";
    o.str = "empty becaus moved";
  }
  ~A() { std::cout << "~A(\"" << str << "\") destructor\n"; }
  void print() const { std::cout << "str =\"" << str << "\"\n"; }
};

int main() {
  /*
  List<int> l;
  l.push_back(1);
  l.push_back(2);
  l.push_back(3);
  l.push_back(4);
  l.pop_back();
  l.push_back(5);
  l.push_back(6);
  l.push_front(10);
  std::reverse(l.begin(), l.end());

  for (auto it = l.begin(); it != l.end(); ++it){
    std::cout << *it << std::endl;
  }
*/

  List<A> l;
  std::cout << "\npush back test\n";
  for (int i = 0; i < 5; i++) {
    l.push_back(A("push_back=" + std::to_string(i)));
  }
  l.push_front(A("push_front =-1"));

  {
    A ct("push_back with copyctor");
    l.push_back(ct);
  }

  std::cout << "\ncall print with const_iterator\n";
  for (auto it = l.cbegin(); it != l.cend(); it++) {
    it->print();  // print use const_iterator
  }

  std::cout << "\n unique_ptr test\n";
  List<std::unique_ptr<A>> lu;
  {
    std::unique_ptr<A> uptr = std::make_unique<A>("uptr move");
    lu.push_back(std::move(uptr));
  }

  List<std::unique_ptr<A>> lu2 = std::move(lu);

  lu2.back()->print();

  std::cout << "\nend test\n";

  return 0;
}