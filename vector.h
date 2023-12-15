#pragma once

#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_MEMORY_IMPLEMENTED

#include <algorithm>
#include <exception>
#include <iostream>

template <typename T>
class Vector {
 public:
  using ValueType = T;              // NOLINT
  using Pointer = T*;               // NOLINT
  using Reference = T&;             // NOLINT
  using ConstPointer = const T*;    // NOLINT
  using ConstReference = const T&;  // NOLINT
  using SizeType = uint64_t;        // NOLINT

  Vector() : size_(0), capacity_(0) {
    buffer_ = nullptr;
  }

  explicit Vector(SizeType size) : Vector() {
    if (size != 0) {
      auto copy = static_cast<Pointer>(operator new(sizeof(ValueType) * size));
      SizeType i = 0;
      try {
        for (SizeType j = 0; j < size; ++j) {
          new (copy + j) ValueType;
          ++i;
        }
      } catch (...) {
        for (SizeType k = 0; k < i; ++k) {
          (copy + k)->~ValueType();
        }
        operator delete(copy);
        throw;
      }
      buffer_ = copy;
      capacity_ = size;
      size_ = size;
    }
  }

  Vector(SizeType size, ValueType value) : Vector() {
    if (size != 0) {
      auto copy = static_cast<Pointer>(operator new(sizeof(ValueType) * size));
      SizeType i = 0;
      try {
        for (SizeType j = 0; j < size; ++j) {
          new (copy + j) ValueType(value);
          ++i;
        }
      } catch (...) {
        for (SizeType k = 0; k < i; ++k) {
          (copy + k)->~ValueType();
        }
        operator delete(copy);
        throw;
      }
      buffer_ = copy;
      capacity_ = size;
      size_ = size;
    }
  }

  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
                                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  Vector(Iterator begin, Iterator end) : Vector() {
    if (begin != end) {
      auto copy = static_cast<Pointer>(operator new(sizeof(ValueType) * (end - begin)));
      SizeType i = 0;
      try {
        for (auto it = begin; it != end; ++it) {
          new (copy + i) ValueType(*it);
          ++i;
        }
      } catch (...) {
        for (SizeType j = 0; j < i; ++j) {
          (copy + j)->~ValueType();
        }
        operator delete(copy);
        throw;
      }
      buffer_ = copy;
      capacity_ = end - begin;
      size_ = end - begin;
    }
  }
  Vector(std::initializer_list<ValueType> list) : Vector() {  // NOLINT
    if (list.size() != 0) {
      buffer_ = static_cast<Pointer>(operator new(sizeof(ValueType) * list.size()));
      capacity_ = list.size();
      for (auto it = list.begin(); it != list.end(); ++it) {
        new (buffer_ + size_) ValueType(*it);
        ++size_;
      }
    }
  }

  Vector(const Vector<ValueType>& other) {  // NOLINT
    if (other.buffer_ == nullptr) {
      buffer_ = nullptr;
      size_ = other.size_;
      capacity_ = other.capacity_;
    } else {
      auto copy = static_cast<Pointer>(operator new(sizeof(ValueType) * other.capacity_));
      size_ = 0;
      try {
        while (size_ < other.size_) {
          new (copy + size_) ValueType(other.buffer_[size_]);
          ++size_;
        }
      } catch (...) {
        for (SizeType j = 0; j < size_; ++j) {
          (copy + j)->~ValueType();
        }
        operator delete(copy);
        throw;
      }
      size_ = other.size_;
      capacity_ = other.capacity_;
      buffer_ = copy;
    }
  }

  Vector(Vector&& other) noexcept {  // NOLINT
    buffer_ = other.buffer_;
    capacity_ = other.capacity_;
    size_ = other.size_;
    other.buffer_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  ~Vector() {
    if (buffer_ != nullptr) {
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
    }
    size_ = 0;
    capacity_ = 0;
    buffer_ = nullptr;
  }

  Vector& operator=(const Vector<ValueType>& other) {
    if (this == &other) {
      return *this;
    }
    Vector<ValueType> copy(other);
    this->Swap(copy);
    return *this;
  }

  Vector& operator=(Vector<ValueType>&& other) {
    Vector<ValueType> copy(std::move(other));
    this->Swap(copy);
    other.buffer_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
    return *this;
  }

  SizeType Size() const {
    return size_;
  }
  SizeType Capacity() const {
    return capacity_;
  }
  bool Empty() const {
    return size_ == 0;
  }

  Reference operator[](SizeType index) {
    return buffer_[index];
  }
  ConstReference operator[](SizeType index) const {
    return buffer_[index];
  }
  Reference At(SizeType index) {
    if (index >= size_) {
      throw std::out_of_range("");
    }
    return buffer_[index];
  }
  ConstReference At(SizeType index) const {
    if (index < size_) {
      return buffer_[index];
    }
    throw std::out_of_range("");
  }

  Reference Front() {
    return *buffer_;
  }

  ConstReference Front() const {
    return *buffer_;
  }
  Reference Back() {
    return *(buffer_ + size_ - 1);
  }
  ConstReference Back() const {
    return *(buffer_ + size_ - 1);
  }

  Pointer Data() noexcept {
    return buffer_;
  }

  ConstPointer Data() const {
    return buffer_;
  }

  void Swap(Vector<T>& other) {
    std::swap(capacity_, other.capacity_);
    std::swap(buffer_, other.buffer_);
    std::swap(size_, other.size_);
  }

  Vector& operator=(std::initializer_list<ValueType>& list) {
    auto new_buf = static_cast<Pointer>(operator new(sizeof(ValueType) * 2 * list.size()));
    SizeType j = 0;
    try {
      for (SizeType i = 0; i < list.size(); ++i) {
        new (new_buf + i) ValueType(buffer_[i]);
        ++j;
      }
    } catch (...) {
      while (j > 0) {
        (new_buf + j - 1)->~ValueType();
        --j;
      }
      throw;
    }
    if (buffer_ != nullptr) {
      for (auto i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
    }
    capacity_ = 2 * list.size();
    buffer_ = new_buf;
    Vector v(list);
    this->Swap(v);
    return *this;
  }

  void Reserve(SizeType new_capacity) {
    if (capacity_ < new_capacity) {
      auto copy = static_cast<Pointer>(operator new(sizeof(ValueType) * new_capacity));
      SizeType c = 0;
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (copy + i) ValueType(std::move(buffer_[i]));
          ++c;
        }
      } catch (...) {
        while (c > 0) {
          (copy + c - 1)->~ValueType();
          --c;
        }
        throw;
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      capacity_ = new_capacity;
      buffer_ = copy;
    }
  }
  void ShrinkToFit() {
    if ((capacity_ != 0) && (capacity_ > size_)) {
      if (size_ == 0) {
        operator delete(buffer_);
        buffer_ = nullptr;
        capacity_ = 0;
      } else {
        auto new_buf = static_cast<Pointer>(operator new(sizeof(ValueType) * size_));
        SizeType j = 0;
        try {
          for (SizeType i = 0; i < size_; ++i) {
            new (new_buf + i) ValueType(std::move(buffer_[i]));
            ++j;
          }
        } catch (...) {
          while (j > 0) {
            (new_buf + j - 1)->~ValueType();
            --j;
          }
          throw;
        }
        for (SizeType i = 0; i < size_; ++i) {
          (buffer_ + i)->~ValueType();
        }
        operator delete(buffer_);
        buffer_ = new_buf;
        capacity_ = size_;
      }
    }
  }

  void Clear() noexcept {
    while (size_ > 0) {
      (buffer_ + size_ - 1)->~ValueType();
      --size_;
    }
  }

  void Resize(SizeType new_size) {
    if (new_size == 0) {
      this->~Vector();
      return;
    }
    if (new_size == capacity_) {
      try {
        while (size_ < capacity_) {
          new (buffer_ + size_) ValueType;
          ++size_;
        }
      } catch (...) {
        throw;
      }
      return;
    }
    if (new_size < capacity_ && new_size >= size_) {
      try {
        while (new_size > size_) {
          new (buffer_ + size_) ValueType;
          ++size_;
        }
      } catch (...) {
        throw;
      }
      return;
    }

    if (new_size < size_) {
      while (new_size < size_) {
        (buffer_ + size_ - 1)->~ValueType();
        --size_;
      }
      return;
    }
    auto new_buffer = static_cast<Pointer>(operator new(sizeof(ValueType) * new_size));
    SizeType c = 0;
    try {
      for (SizeType i = 0; i < Size() && i < new_size; ++i) {
        new (new_buffer + i) ValueType(std::move(buffer_[i]));
        ++c;
      }
      for (SizeType i = size_; i < new_size; ++i) {
        new (new_buffer + i) ValueType;
        ++c;
      }
    } catch (...) {
      while (c > 0) {
        (new_buffer + c - 1)->~ValueType();
        --c;
      }
      operator delete(new_buffer);
      throw;
    }
    for (SizeType i = 0; i < size_; ++i) {
      (buffer_ + i)->~ValueType();
    }
    operator delete(buffer_);
    capacity_ = new_size;
    size_ = new_size;
    buffer_ = new_buffer;
  }

  void Resize(SizeType new_size, ConstReference value) {
    if (new_size <= size_) {
      Resize(new_size);
      return;
    }
    if (new_size > size_ && new_size <= capacity_) {
      try {
        SizeType s = size_;
        SizeType c = capacity_;
        for (auto i = s; i < new_size; ++i) {
          new (buffer_ + i) ValueType(value);
          ++size_;
        }
      } catch (...) {
        throw;
      }
      return;
    }
    if (new_size > capacity_) {
      auto arr = static_cast<Pointer>(operator new(sizeof(ValueType) * new_size));
      SizeType j = 0;
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (arr + i) ValueType(buffer_[i]);
          ++j;
        }
        for (SizeType i = size_; i < new_size; ++i) {
          new (arr + i) ValueType(value);
          ++j;
        }
      } catch (...) {
        while (j > 0) {
          (arr + j - 1)->~ValueType();
          --j;
        }
        operator delete(arr);
        throw;
      }
      this->~Vector();
      buffer_ = arr;
      capacity_ = new_size;
      size_ = new_size;
    }
  }

  void PopBack() {
    SizeType s = size_;
    try {
      if (size_ != 0) {
        --size_;
        (buffer_ + size_)->~ValueType();
      } else {
        throw std::out_of_range("");
      }
    } catch (...) {
      size_ = s;
      throw;
    }
  }

  template <typename... El>
  void EmplaceBack(El&&... args) {
    if (size_ >= capacity_) {
      EmplaceFunc(capacity_ == 0 ? 1 : capacity_);
    }
    try {
      new (buffer_ + size_) ValueType(std::forward<El>(args)...);
      ++size_;
    } catch (...) {
      --size_;
      throw;
    }
  }

  void EmplaceFunc(SizeType plus) {
    capacity_ += plus;
    auto arr = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
    if (buffer_ == nullptr) {
      buffer_ = arr;
    } else {
      for (SizeType i = 0; i < size_; ++i) {
        new (arr + i) ValueType(std::move(buffer_[i]));
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      buffer_ = arr;
    }
  }

  void PushBack(const T& value) {
    if (capacity_ == 0) {
      auto arr = static_cast<Pointer>(operator new(sizeof(ValueType)));
      try {
        new (arr) ValueType(value);
        ++size_;
      } catch (...) {
        while (size_ > 0) {
          (arr + size_ - 1)->~ValueType();
        }
        operator delete(arr);
        throw;
      }
      buffer_ = arr;
      capacity_ = 1;
      size_ = 1;
      return;
    }
    if (size_ < capacity_) {
      try {
        new (buffer_ + size_) ValueType(value);
        ++size_;
      } catch (...) {
        throw;
      }
      return;
    }
    if (size_ == capacity_) {
      SizeType counter = 0;
      auto arr = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_ * 2));
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (arr + i) ValueType(std::move(buffer_[i]));
          ++counter;
        }
        new (arr + counter) ValueType(value);
        ++counter;
      } catch (...) {
        while (counter > 0) {
          (arr + counter - 1)->~ValueType();
          --counter;
        }
        operator delete(arr);
        throw;
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      buffer_ = arr;
      capacity_ += capacity_;
      ++size_;
      return;
    }
  }

  void PushBack(T&& value) {
    if (capacity_ == 0) {
      auto arr = static_cast<Pointer>(operator new(sizeof(ValueType)));
      try {
        new (arr) ValueType(std::move(value));
        ++size_;
      } catch (...) {
        while (size_ > 0) {
          (arr + size_ - 1)->~ValueType();
        }
        operator delete(arr);
        throw;
      }
      buffer_ = arr;
      capacity_ = 1;
      size_ = 1;
      return;
    }
    if (size_ < capacity_) {
      try {
        new (buffer_ + size_) ValueType(std::move(value));
        ++size_;
      } catch (...) {
        throw;
      }
      return;
    }
    if (size_ == capacity_) {
      SizeType counter = 0;
      auto arr = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_ * 2));
      try {
        for (SizeType i = 0; i < size_; ++i) {
          new (arr + i) ValueType(std::move(buffer_[i]));
          ++counter;
        }
        new (arr + counter) ValueType(std::move(value));
        ++counter;
      } catch (...) {
        while (counter > 0) {
          (arr + counter - 1)->~ValueType();
          --counter;
        }
        operator delete(arr);
        throw;
      }
      for (SizeType i = 0; i < size_; ++i) {
        (buffer_ + i)->~ValueType();
      }
      operator delete(buffer_);
      buffer_ = arr;
      capacity_ += capacity_;
      ++size_;
      return;
    }
  }
  template <typename Type>
  struct VecIterator {
   public:
    using value_type = std::remove_cv_t<Type>;                  // NOLINT
    using pointer = ValueType*;                                 // NOLINT
    using reference = ValueType&;                               // NOLINT
    using difference_type = ptrdiff_t;                          // NOLINT
    using iterator_category = std::random_access_iterator_tag;  // NOLINT
    const pointer data_;                                        // NOLINT
    SizeType idx_;

    VecIterator() : data_(nullptr), idx_(0) {
    }

    VecIterator(Pointer data, SizeType idx) : data_(data), idx_(idx) {
    }

    explicit operator VecIterator<const Type>() const {
      return VecIterator<const Type>(data_, idx_);
    }

    VecIterator(const VecIterator& other) = default;

    VecIterator& operator=(const VecIterator& other) = default;

    VecIterator& operator++() {
      ++idx_;
      return *this;
    }

    VecIterator operator++(int) {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    VecIterator& operator--() {
      idx_--;
      return *this;
    }

    VecIterator operator--(int) {
      auto copy = *this;
      --(*this);
      return copy;
    }

    VecIterator& operator+=(difference_type x) {
      idx_ += x;
      return (*this);
    }

    VecIterator& operator-=(difference_type x) {
      idx_ -= x;
      return *this;
    }

    VecIterator operator+(difference_type x) const {
      VecIterator copy = *this;
      return copy += x;
    }

    VecIterator operator-(difference_type x) const {
      VecIterator copy = *this;
      return copy -= x;
    }

    bool operator==(const VecIterator& other) const {
      return (idx_ == other.idx_);
    }

    bool operator!=(const VecIterator& other) const {
      return !(*this == other);
    }

    Reference operator*() const {
      return data_[idx_];
    }

    Pointer operator->() const {
      return data_ + idx_;
    }

    ~VecIterator() = default;
  };

  using Iterator = VecIterator<ValueType>;
  using ConstIterator = VecIterator<const ValueType>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Iterator begin() {  // NOLINT
    return Iterator(buffer_, 0);
  }

  ConstIterator begin() const {  // NOLINT
    return ConstIterator(buffer_, 0);
  }

  ConstIterator cbegin() const {  // NOLINT
    return ConstIterator(buffer_, 0);
  }

  Iterator end() {  // NOLINT
    return Iterator(buffer_, size_);
  }

  ConstIterator end() const {  // NOLINT
    return ConstIterator(buffer_, size_);
  }

  ConstIterator cend() const {  // NOLINT
    return ConstIterator(buffer_, size_);
  }

  ReverseIterator rbegin() {  // NOLINT
    return ReverseIterator({buffer_, size_});
  }

  ReverseIterator rend() {  // NOLINT
    return ReverseIterator({buffer_, 0});
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return ConstReverseIterator({buffer_, size_});
  }

  ConstReverseIterator rend() const {  // NOLINT
    return ConstReverseIterator({buffer_, 0});
  }

  ConstReverseIterator crbegin() const {  // NOLINT
    return ConstReverseIterator({buffer_, size_});
  }
  ConstReverseIterator crend() const {  // NOLINT
    return ConstReverseIterator({buffer_, 0});
  }

 private:
  uint64_t size_;
  uint64_t capacity_;
  T* buffer_;
};

template <class T>
bool operator==(const Vector<T>& first, const Vector<T>& other) {
  if (first.Size() != other.Size()) {
    return false;
  }
  for (size_t i = 0; i < other.Size(); ++i) {
    if (first[i] != other[i]) {
      return false;
    }
  }
  return true;
}
template <class T>
bool operator!=(const Vector<T>& first, const Vector<T>& other) {
  return !(first == other);
}
template <class T>
bool operator<(const Vector<T>& first, const Vector<T>& other) {
  for (size_t i = 0; i < std::min(other.Size(), first.Size()); ++i) {
    if (first[i] == other[i]) {
      continue;
    }
    return first[i] < other[i];
  }
  return first.Size() < other.Size();
}

template <class T>
bool operator<=(const Vector<T>& first, const Vector<T>& other) {
  return !(other < first);
}

template <class T>
bool operator>(const Vector<T>& first, const Vector<T>& other) {
  return (other < first);
}

template <class T>
bool operator>=(const Vector<T>& first, const Vector<T>& other) {
  return !(first < other);
}
#endif