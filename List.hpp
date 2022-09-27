/**
 * @file List.hpp
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Simple implementation list container
 * @version 0.1
 * @date 2022-09-06
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _LIST_HPP_
#define _LIST_HPP_

#include <cassert>
#include <iterator>
#include <memory>
#include <utility>
template <class T, class Allocator = std::allocator<T>>
class List {
 public:  // NOLINT
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;

 private:
  struct Node {
    T value;
    Node* prev = this;
    Node* next = this;
    void hook(Node* node) noexcept;
    void unhook() noexcept;
  } * root_node_p;

  // typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
  // node_alloc; allocator traits
  Allocator vtype_alloc;
  typename std::allocator_traits<Allocator>::rebind_alloc<Node> node_alloc;
  using traits_node = std::allocator_traits<decltype(node_alloc)>;
  using traits_vtype = std::allocator_traits<Allocator>;

  /**
   * @brief Allocate memory for node before the current (ptr),
   * and changes all the corresponding pointers, then call constructor.
   *
   * @tparam Args Pack of arguments.
   * @param ptr Pointer to the node in front of a new node
   * (the new node will be inserted and ptr->prev = the new node).
   * @param args Argument for T type constructor ( as = new T(Args&&... args) )
   * @return Node* Pointer to new created node.
   */
  template <class... Args>
  Node* insertNode(Node* const ptr, Args&&... args);

  /**
   * @brief Destroy and free node and changes all the corresponding pointers.
   * @param ptr a pointer for destroying and freeing memory.
   * After calling the method, the pointer will be invalid.
   * @return Node* Pointer to next node (ptr->next);
   */
  Node* eraseNode(Node* ptr);

  size_t sz = 0;

 public:
  // ctors
  List();
  List(const List& other);
  List(List&& other);

  // asing move
  List& operator=(const List& other);
  List& operator=(List&& other);

  // dctor
  ~List();

  /// iterator
  template <bool _is_const>
  class common_iterator {
    friend class List;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::conditional_t<_is_const, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<_is_const, const T*, T*>;
    using reference = std::conditional_t<_is_const, const T&, T&>;
    using node_pointer = std::conditional_t<_is_const, const Node*, Node*>;

   private:
    node_pointer ptr = nullptr;

   public:
    common_iterator() = default;
    explicit common_iterator(Node* node) : ptr(node) {}
    common_iterator(const common_iterator& other) = default;

    reference operator*() const;  // { return ptr->value; }
    pointer operator->() const;   // { return &(ptr->value); }
    bool operator==(
        const common_iterator& other) const;  // { return ptr == other.ptr;}
    bool operator!=(
        const common_iterator& other) const;  // { return !(*this == other); }
    common_iterator& operator++();            // {ptr = ptr->next; return *this;}
    common_iterator operator++(
        int);                       // {common_iterator ret = *this; ptr = ptr->next; return ret;}
    common_iterator& operator--();  // {ptr = ptr->prev; return *this;}
    common_iterator operator--(
        int);  // {common_iterator ret = *this; ptr = ptr->prev; return ret;}

    operator common_iterator<true>();
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator rcbegin() const noexcept;
  const_reverse_iterator rcend() const noexcept;
  // end iterator

  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);

  iterator insert(const_iterator pos, const T& value);
  iterator insert(const_iterator pos, T&& value);

  template <class InputIt>
  std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<InputIt>::iterator_category,
                                         std::input_iterator_tag>,
                   iterator>
  insert(const_iterator pos, InputIt first, InputIt last);

  template <class... Args>
  reference emplace_back(Args&&... args);

  template <class... Args>
  reference emplace_front(Args&&... args);

  void push_back(const T& value);
  void push_back(T&& value);
  void pop_back();
  void push_front(const T& value);
  void push_front(T&& value);
  void pop_front();

 private:  // NOLINT
  void push_back(const_iterator first, const_iterator last);

 public:  // NOLINT
  reference front() noexcept;
  const_reference front() const noexcept;
  reference back() noexcept;
  const_reference back() const noexcept;

  size_t size() const noexcept;
  bool empty() const noexcept;
  void clear() noexcept;
};

template <class T, class Allocator>
void List<T, Allocator>::Node::hook(List<T, Allocator>::Node* node) noexcept {
  next = node;
  prev = node->prev;
  node->prev->next = this;
  node->prev = this;
}

template <class T, class Allocator>
void List<T, Allocator>::Node::unhook() noexcept{
  prev->next = next;
  next->prev = prev;
  prev = nullptr;
  next = nullptr;
}

template <class T, class Allocator>
template <class... Args>
List<T, Allocator>::Node* List<T, Allocator>::insertNode(
    List<T, Allocator>::Node* ptr, Args&&... args) {
  // allocate
  Node* const newnode = traits_node::allocate(node_alloc, 1);

  // construct
  try {
    traits_vtype::construct(vtype_alloc, &newnode->value, std::forward<Args>(args)...);
  } catch (...) {
    traits_node::deallocate(node_alloc, newnode, 1);
    throw;
  }

  // insert
  newnode->hook(ptr);

  // change count
  ++sz;
  return newnode;  // now newnode->next == ptr
}

template <class T, class Allocator>
List<T, Allocator>::Node* List<T, Allocator>::eraseNode(
    List<T, Allocator>::Node* ptr) {
  if (ptr == root_node_p) return ptr;  // root_node_p
  Node* ret = ptr->next;
  traits_vtype::destroy(vtype_alloc, &ptr->value);  // ptr->value now not valid
  ptr->unhook();
  traits_node::deallocate(node_alloc, ptr, 1);
  --sz;
  return ret;
}

template <class T, class Allocator>
List<T, Allocator>::List() {
  root_node_p = traits_node::allocate(node_alloc, 1u);
  root_node_p->next = root_node_p;
  root_node_p->prev = root_node_p;
}

template <class T, class Allocator>
List<T, Allocator>::List(const List& other) : List() {
  clear();
  node_alloc = other.node_alloc;
  vtype_alloc = other.vtype_alloc;
  push_back(other.cbegin(), other.cend());
}

template <class T, class Allocator>
List<T, Allocator>::List(List&& other) : List() {
  std::swap(node_alloc, other.node_alloc);
  std::swap(vtype_alloc, other.vtype_alloc);
  std::swap(root_node_p, other.root_node_p);
  std::swap(sz, other.sz);
}

template <class T, class Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (other.root_node_p == this->root_node_p) return *this;
  clear();
  push_back(other.cbegin(), other.cend());
  return *this;
}

template <class T, class Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& other) {
  if (other.root_node_p == this->root_node_p) return *this;
  std::swap(other.node_alloc, node_alloc);
  std::swap(other.vtype_alloc, vtype_alloc);
  std::swap(other.root_node_p, root_node_p);
  std::swap(other.sz, sz);
  return *this;
}

template <class T, class Allocator>
List<T, Allocator>::~List() {
  clear();
  traits_node::deallocate(node_alloc, root_node_p, 1u);
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>::reference
List<T, Allocator>::common_iterator<_is_const>::operator*() const {
  return ptr->value;
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>::pointer
List<T, Allocator>::common_iterator<_is_const>::operator->() const {
  return &(ptr->value);
}

template <class T, class Allocator>
template <bool _is_const>
inline bool List<T, Allocator>::common_iterator<_is_const>::operator==(
    const List<T, Allocator>::common_iterator<_is_const>& other) const {
  return ptr == other.ptr;
}

template <class T, class Allocator>
template <bool _is_const>
inline bool List<T, Allocator>::common_iterator<_is_const>::operator!=(
    const List<T, Allocator>::common_iterator<_is_const>& other) const {
  return !(*this == other);
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>&
List<T, Allocator>::common_iterator<_is_const>::operator++() {
  ptr = ptr->next;
  return *this;
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>
List<T, Allocator>::common_iterator<_is_const>::operator++(int) {
  common_iterator ret = *this;
  ptr = ptr->next;
  return ret;
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>&
List<T, Allocator>::common_iterator<_is_const>::operator--() {
  ptr = ptr->prev;
  return *this;
}

template <class T, class Allocator>
template <bool _is_const>
List<T, Allocator>::common_iterator<_is_const>::operator List<
    T, Allocator>::common_iterator<true>() {
  return common_iterator<true>(ptr);
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>
List<T, Allocator>::common_iterator<_is_const>::operator--(int) {
  common_iterator ret = *this;
  ptr = ptr->prev;
  return ret;
}

template <class T, class Allocator>
List<T, Allocator>::iterator List<T, Allocator>::begin() noexcept {
  iterator i(root_node_p->next);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::begin()
    const noexcept {
  return cbegin();
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::cbegin()
    const noexcept {
  const_iterator i(root_node_p->next);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::end() noexcept {
  iterator i(root_node_p);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::end()
    const noexcept {
  return cend();
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::cend()
    const noexcept {
  const_iterator i(root_node_p);
  return i;
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::iterator>
List<T, Allocator>::rbegin() noexcept {
  return std::reverse_iterator<iterator>(end());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator>
List<T, Allocator>::rbegin() const noexcept {
  return std::reverse_iterator<const_iterator>(end());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator>
List<T, Allocator>::rcbegin() const noexcept {
  return std::reverse_iterator<const_iterator>(cend());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::iterator>
List<T, Allocator>::rend() noexcept {
  return std::reverse_iterator<iterator>(begin());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator>
List<T, Allocator>::rend() const noexcept {
  return std::reverse_iterator<const_iterator>(begin());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator>
List<T, Allocator>::rcend() const noexcept {
  return std::reverse_iterator<const_iterator>(cbegin());
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::erase(typename List<T, Allocator>::const_iterator pos) {
  Node* p = eraseNode(const_cast<Node*>(pos.ptr));
  return iterator(p);
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::erase(typename List<T, Allocator>::const_iterator first,
                                                              typename List<T, Allocator>::const_iterator last) {
  iterator i(const_cast<iterator::node_pointer>(first.ptr));
  if (first.ptr == root_node_p || last == first) return i;

  for (auto it = first; it != last;) {
    it = erase(it);
  }
  return iterator(const_cast<iterator::node_pointer>(last.ptr));
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::insert(typename List<T, Allocator>::const_iterator pos,
                                                               const T& value) {
  Node* p = insertNode(const_cast<Node*>(pos.ptr), value);
  return iterator(p);
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::insert(
    typename List<T, Allocator>::const_iterator pos, T&& value) {
  Node* p = insertNode(const_cast<Node*>(pos.ptr), std::forward<T>(value));
  return iterator(p);
}

template <class T, class Allocator>
template <class InputIt>
std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<InputIt>::iterator_category,
                                       std::input_iterator_tag>,
                 typename List<T, Allocator>::iterator>
List<T, Allocator>::insert(typename List<T, Allocator>::const_iterator pos, InputIt first, InputIt last) {
  Node* p = const_cast<Node*>(pos.ptr);
  iterator ret(p->prev);
  for (auto it = first; it != last; ++it) {
    insertNode(p, *it);
  }
  return ret;
}

template <class T, class Allocator>
template <class... Args>
T& List<T, Allocator>::emplace_back(Args&&... args) {
  insertNode(root_node_p, std::forward<Args>(args)...);
  return back();
}

template <class T, class Allocator>
template <class... Args>
T& List<T, Allocator>::emplace_front(Args&&... args) {
  insertNode(root_node_p->next, std::forward<Args>(args)...);
  return back();
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(const T& value) {
  insertNode(root_node_p, value);
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(T&& value) {
  insertNode(root_node_p, std::forward<T>(value));
}

template <class T, class Allocator>
inline void List<T, Allocator>::pop_back() {
  eraseNode(root_node_p->prev);
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_front(const T& value) {
  insertNode(root_node_p->next, value);
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_front(T&& value) {
  insertNode(root_node_p->next, std::forward<T>(value));
}

template <class T, class Allocator>
inline void List<T, Allocator>::pop_front() {
  eraseNode(root_node_p->next);
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(
    List<T, Allocator>::const_iterator first,
    List<T, Allocator>::const_iterator last) {
  for (auto it = first; it != last; ++it) {
    this->push_back(*it);
  }
}

template <class T, class Allocator>
inline List<T, Allocator>::reference List<T, Allocator>::front() noexcept {
  return root_node_p->next->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_reference List<T, Allocator>::front()
    const noexcept {
  return root_node_p->next->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::reference List<T, Allocator>::back() noexcept {
  return root_node_p->prev->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_reference List<T, Allocator>::back()
    const noexcept {
  return root_node_p->prev->value;
}

template <class T, class Allocator>
inline size_t List<T, Allocator>::size() const noexcept {
  return sz;
}

template <class T, class Allocator>
inline bool List<T, Allocator>::empty() const noexcept {
  return !static_cast<bool>(sz);
}

template <class T, class Allocator>
inline void List<T, Allocator>::clear() noexcept {
  while (sz) this->pop_back();
}

#endif  // _LIST_HPP_
