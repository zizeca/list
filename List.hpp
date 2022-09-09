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

#include <iterator>
#include <memory>
#include <utility>

template <class T, class Allocator = std::allocator<T>>
class List {
 private:
  // container
  struct Node {
    T t;
    Node* prev;
    Node* next;
  };

  // typename std::allocator_traits<Allocator>::template rebind_alloc<Node> m_alloc;
  typename std::allocator_traits<Allocator>::rebind_alloc<Node> m_alloc;
  using traits_t = std::allocator_traits<decltype(m_alloc)>;

#define NODE_CREATE(ptr, val, prev, next) \
  ptr = traits_t::allocate(m_alloc, 1u);  \
  traits_t::construct(m_alloc, ptr, val, prev, next)
#define NODE_DESTROY(ptr)          \
  traits_t::destroy(m_alloc, ptr); \
  traits_t::deallocate(m_alloc, ptr, 1u)

  // start end fake obj
  Node* p_head;

  size_t sz = 0;

 public:
  // ctors
  List();
  List(const List& other);
  List(List&& other);

  // asing move
  T& operator=(const List& other);
  T& operator=(List&& other);

  // dctor
  ~List();

  /// iterator
  template <bool _is_const>
  class common_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::conditional_t<_is_const, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<_is_const, const T*, T*>;
    using reference = std::conditional_t<_is_const, const T&, T&>;
    using node_pointer = std::conditional_t<_is_const, const Node*, Node*>;

   private:
    node_pointer ptr = p_head;

   public:
    common_iterator() = default;
    explicit common_iterator(const std::remove_const_t<node_pointer> node) : ptr(node) {}
    common_iterator(const common_iterator& other) = default;

    reference operator*() const;                          // { return ptr->t; }
    pointer operator->() const;                           // { return &(ptr->t); }
    bool operator==(const common_iterator& other) const;  // { return ptr == other.ptr;}
    bool operator!=(const common_iterator& other) const;  // { return !(*this == other); }
    common_iterator& operator++();                        // {ptr = ptr->next; return *this;}
    common_iterator operator++(int);  // {common_iterator ret = *this; ptr = ptr->next; return ret;}
    common_iterator& operator--();    // {ptr = ptr->prev; return *this;}
    common_iterator operator--(int);  // {common_iterator ret = *this; ptr = ptr->prev; return ret;}
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;

  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  // end iterator

  void push_back(const T& t);
  void push_back(T&& t);
  void pop_back();
  void push_front(const T& t);
  void push_front(T&& t);
  void pop_front();
  T& front();
  T& back();
  size_t size() const noexcept;
  void clear() noexcept;
};

template <class T, class Allocator>
List<T, Allocator>::List() {
  // p_head = reinterpret_cast<Node*>(new int8_t[sizeof(Node)]);
  // p_head = m_alloc.allocate(1u);
  p_head = traits_t::allocate(m_alloc, 1u);
  p_head->next = p_head;
  p_head->prev = p_head;
}

template <class T, class Allocator>
List<T, Allocator>::List(const List& other) : List() {
  for (auto it = other.cbegin(); it != cend(); ++it) {
    this->push_back(*it);
  }
}

template <class T, class Allocator>
List<T, Allocator>::List(List&& other) : List() {
  std::swap(p_head, other.p_head);
  std::swap(sz, other.sz);
}

template <class T, class Allocator>
T& List<T, Allocator>::operator=(const List& other) {
  if (other == *this) return *this;
  this->clear();
  for (auto it = other.cbegin(); it != cend(); ++it) {
    this->push_back(*it);
  }
}

template <class T, class Allocator>
T& List<T, Allocator>::operator=(List&& other) {
  if (other == *this) return *this;

  std::swap(other.p_head, p_head);
  std::swap(other.sz, sz);
}

template <class T, class Allocator>
List<T, Allocator>::~List() {
  clear();
  // m_alloc.deallocate(p_head, 1u);
  // delete[] reinterpret_cast<int8_t*>(p_head);
  traits_t::deallocate(m_alloc, p_head, 1u);
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>::reference
List<T, Allocator>::common_iterator<_is_const>::operator*() const {
  return ptr->t;
}

template <class T, class Allocator>
template <bool _is_const>
inline List<T, Allocator>::common_iterator<_is_const>::pointer
List<T, Allocator>::common_iterator<_is_const>::operator->() const {
  return &(ptr->t);
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
inline List<T, Allocator>::common_iterator<_is_const>
List<T, Allocator>::common_iterator<_is_const>::operator--(int) {
  common_iterator ret = *this;
  ptr = ptr->prev;
  return ret;
}

template <class T, class Allocator>
List<T, Allocator>::iterator List<T, Allocator>::begin() noexcept {
  iterator i(p_head->next);
  return i;
}

template <class T, class Allocator>
List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const noexcept {
  const_iterator i(p_head->next);
  return i;
}

template <class T, class Allocator>
List<T, Allocator>::iterator List<T, Allocator>::end() noexcept {
  iterator i(p_head);
  return i;
}

template <class T, class Allocator>
List<T, Allocator>::const_iterator List<T, Allocator>::cend() const noexcept {
  const_iterator i(p_head);
  return i;
}

template <class T, class Allocator>
void List<T, Allocator>::push_back(const T& t) {
  Node* tmp = p_head->prev;
  // tmp->next = new Node{t, tmp, p_head};
  NODE_CREATE(tmp->next, t, tmp, p_head);
  p_head->prev = tmp->next;
  ++sz;
}

template <class T, class Allocator>
void List<T, Allocator>::push_back(T&& t) {
  Node* tmp = p_head->prev;
  // tmp->next = new Node{std::move(t), tmp, p_head};
  NODE_CREATE(tmp->next, std::move(t), tmp, p_head);
  p_head->prev = tmp->next;
  ++sz;
}

template <class T, class Allocator>
void List<T, Allocator>::pop_back() {
  if (p_head->next == p_head && p_head->prev == p_head) return;
  Node* d = p_head->prev;
  d->prev->next = p_head;
  p_head->prev = d->prev;
  // delete d;
  NODE_DESTROY(d);
  --sz;
}

template <class T, class Allocator>
void List<T, Allocator>::push_front(const T& t) {
  Node* tmp = p_head->next;
  // tmp->prev = new Node{t, p_head, tmp};
  NODE_CREATE(tmp->prev, t, p_head, tmp);
  p_head->next = tmp->prev;
  ++sz;
}

template <class T, class Allocator>
void List<T, Allocator>::push_front(T&& t) {
  Node* tmp = p_head->next;
  // tmp->prev = new Node{std::move(t), p_head, tmp};
  NODE_CREATE(tmp->prev, std::move(t), p_head, tmp);
  p_head->next = tmp->prev;
  ++sz;
}

template <class T, class Allocator>
void List<T, Allocator>::pop_front() {
  if (p_head->next == p_head && p_head->prev == p_head) return;
  Node* d = p_head->next;
  d->next->prev = p_head;
  p_head->next = d->next;
  // delete d;
  NODE_DESTROY(d);
  --sz;
}

template <class T, class Allocator>
T& List<T, Allocator>::front() {
  return p_head->next->t;
}

template <class T, class Allocator>
T& List<T, Allocator>::back() {
  return p_head->prev->t;
}

template <class T, class Allocator>
size_t List<T, Allocator>::size() const noexcept {
  return sz;
}

template <class T, class Allocator>
void List<T, Allocator>::clear() noexcept {
  while (sz) this->pop_back();
}

#endif  // _LIST_HPP_
