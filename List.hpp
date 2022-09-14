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
    T value;
    Node* prev;
    Node* next;
  };

  // typename std::allocator_traits<Allocator>::template rebind_alloc<Node> m_alloc;
  typename std::allocator_traits<Allocator>::rebind_alloc<Node> m_alloc;
  using traits_t = std::allocator_traits<decltype(m_alloc)>;

#define NODE_CREATE(ptr, val, prev, next) \
  ptr = traits_t::allocate(m_alloc, 1u);  \
  traits_t::construct(m_alloc, ptr, val, prev, next); \
  ++sz
#define NODE_DESTROY(ptr)                 \
  traits_t::destroy(m_alloc, ptr);        \
  traits_t::deallocate(m_alloc, ptr, 1u); \
  --sz

  // start end fake obj
  Node* p_head;

  size_t sz = 0;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;

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

    reference operator*() const;                          // { return ptr->value; }
    pointer operator->() const;                           // { return &(ptr->value); }
    bool operator==(const common_iterator& other) const;  // { return ptr == other.ptr;}
    bool operator!=(const common_iterator& other) const;  // { return !(*this == other); }
    common_iterator& operator++();                        // {ptr = ptr->next; return *this;}
    common_iterator operator++(int);  // {common_iterator ret = *this; ptr = ptr->next; return ret;}
    common_iterator& operator--();    // {ptr = ptr->prev; return *this;}
    common_iterator operator--(int);  // {common_iterator ret = *this; ptr = ptr->prev; return ret;}

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
List<T, Allocator>::List() {
  p_head = traits_t::allocate(m_alloc, 1u);
  p_head->next = p_head;
  p_head->prev = p_head;
}

template <class T, class Allocator>
List<T, Allocator>::List(const List& other) : List() {
  clear();
  push_back(other.cbegin(), other.cend());
}

template <class T, class Allocator>
List<T, Allocator>::List(List&& other) : List() {
  std::swap(p_head, other.p_head);
  std::swap(sz, other.sz);
}

template <class T, class Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (other.p_head == this->p_head) return *this;
  clear();
  push_back(other.cbegin(), other.cend());
  return *this;
}

template <class T, class Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List&& other) {
  if (other.p_head == this->p_head) return *this;
  std::swap(other.p_head, p_head);
  std::swap(other.sz, sz);
  return *this;
}

template <class T, class Allocator>
List<T, Allocator>::~List() {
  clear();
  traits_t::deallocate(m_alloc, p_head, 1u);
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
List<T, Allocator>::common_iterator<_is_const>::operator List<T,
                                                              Allocator>::common_iterator<true>() {
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
  iterator i(p_head->next);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::begin() const noexcept {
  return cbegin();
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const noexcept {
  const_iterator i(p_head->next);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::end() noexcept {
  iterator i(p_head);
  return i;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::end() const noexcept {
  return cend();
}

template <class T, class Allocator>
inline List<T, Allocator>::const_iterator List<T, Allocator>::cend() const noexcept {
  const_iterator i(p_head);
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
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator> List<T, Allocator>::rend()
    const noexcept {
  return std::reverse_iterator<const_iterator>(begin());
}

template <class T, class Allocator>
inline std::reverse_iterator<typename List<T, Allocator>::const_iterator>
List<T, Allocator>::rcend() const noexcept {
  return std::reverse_iterator<const_iterator>(cbegin());
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::erase(
    typename List<T, Allocator>::const_iterator pos) {
  iterator i(const_cast<iterator::node_pointer>(pos.ptr));
  if (pos.ptr == p_head) return i;

  i.ptr->prev->next = i.ptr->next;  // change prev node
  i.ptr->next->prev = i.ptr->prev;  // change next node
  iterator ret(i.ptr->next);        // return iterator
  NODE_DESTROY(i.ptr);
  return ret;
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::erase(
    typename List<T, Allocator>::const_iterator first,
    typename List<T, Allocator>::const_iterator last) {
  iterator i(const_cast<iterator::node_pointer>(first.ptr));
  if (first.ptr == p_head || last == first) return i;

  for (auto it = first; it != last;) {
    it = erase(it);
  }
  return iterator(const_cast<iterator::node_pointer>(last.ptr));
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::insert(
    typename List<T, Allocator>::const_iterator pos, const T& value) {
  iterator it(const_cast<iterator::node_pointer>(pos.ptr));
  Node* prev = it.ptr->prev;
  NODE_CREATE(it.ptr->prev, value, prev, it.ptr);
  prev->next = it.ptr->prev;
  return iterator(it.ptr->prev);
}

template <class T, class Allocator>
inline List<T, Allocator>::iterator List<T, Allocator>::insert(
    typename List<T, Allocator>::const_iterator pos, T&& value) {
  iterator it(const_cast<iterator::node_pointer>(pos.ptr));
  Node* prev = it.ptr->prev;
  NODE_CREATE(it.ptr->prev, std::move(value), prev, it.ptr);
  prev->next = it.ptr->prev;
  return iterator(it.ptr->prev);
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(const T& value) {
  Node* tmp = p_head->prev;
  NODE_CREATE(tmp->next, value, tmp, p_head);
  p_head->prev = tmp->next;
}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(T&& value) {
  Node* tmp = p_head->prev;
  NODE_CREATE(tmp->next, std::move(value), tmp, p_head);
  p_head->prev = tmp->next;

}

template <class T, class Allocator>
inline void List<T, Allocator>::pop_back() {
  if (p_head->next == p_head && p_head->prev == p_head) return;
  Node* d = p_head->prev;
  d->prev->next = p_head;
  p_head->prev = d->prev;
  NODE_DESTROY(d);

}

template <class T, class Allocator>
inline void List<T, Allocator>::push_front(const T& value) {
  Node* tmp = p_head->next;
  NODE_CREATE(tmp->prev, value, p_head, tmp);
  p_head->next = tmp->prev;

}

template <class T, class Allocator>
inline void List<T, Allocator>::push_front(T&& value) {
  Node* tmp = p_head->next;
  NODE_CREATE(tmp->prev, std::move(value), p_head, tmp);
  p_head->next = tmp->prev;

}

template <class T, class Allocator>
inline void List<T, Allocator>::pop_front() {
  if (p_head->next == p_head && p_head->prev == p_head) return;
  Node* d = p_head->next;
  d->next->prev = p_head;
  p_head->next = d->next;
  NODE_DESTROY(d);

}

template <class T, class Allocator>
inline void List<T, Allocator>::push_back(List<T, Allocator>::const_iterator first,
                                          List<T, Allocator>::const_iterator last) {
  for (auto it = first; it != last; ++it) {
    this->push_back(*it);
  }
}

template <class T, class Allocator>
inline List<T, Allocator>::reference List<T, Allocator>::front() noexcept {
  return p_head->next->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_reference List<T, Allocator>::front() const noexcept {
  return p_head->next->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::reference List<T, Allocator>::back() noexcept {
  return p_head->prev->value;
}

template <class T, class Allocator>
inline List<T, Allocator>::const_reference List<T, Allocator>::back() const noexcept {
  return p_head->prev->value;
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
