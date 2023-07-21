
#ifndef GAPBUFFER_H
#define GAPBUFFER_H
#include <vector>
#include <iostream> // for cout in debug
#include <algorithm>
#include <sstream> // for stringstreams
#include <memory> // for unique_ptr

using std::max;
const size_t kDefaultSize = 10;

// forward declaration for the GapBufferIterator class
template <typename T>
class GapBufferIterator;

// declaration for the GapBuffer class
template <typename T>
class GapBuffer {
public:
    friend class GapBufferIterator<T>;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = GapBufferIterator<T>;    

    explicit GapBuffer();
    explicit GapBuffer(size_type count, const value_type& val = value_type());
    ~GapBuffer();
    GapBuffer(std::initializer_list<T> init);
    GapBuffer(const GapBuffer& other);
    GapBuffer(GapBuffer&& other);
    GapBuffer& operator=(const GapBuffer& rhs);
    GapBuffer& operator=(GapBuffer&& rhs);

    void insert_at_cursor(const_reference element);
    void insert_at_cursor(value_type&& element);
    template <typename... Args>
    void emplace_at_cursor(Args&&... args); // optional
    void delete_at_cursor();
    reference get_at_cursor();
    const_reference get_at_cursor() const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference at(size_type pos);
    const_reference at(size_type pos) const;
    void move_cursor(int num);
    void reserve(size_type new_size);
    size_type size() const;
    size_type cursor_index() const;
    bool empty() const;
    void debug() const;

    iterator begin();
    iterator end();
    iterator cursor();

private:
    size_type _logical_size; // uses external_index
    size_type _buffer_size;  // uses array_index
    size_type _cursor_index; // uses array_index
    size_type _gap_size;
    std::unique_ptr<value_type[]> _elems; // uses array_index

    size_type to_external_index(size_type array_index) const;
    size_type to_array_index(size_type external_index) const;
    void move_to_left_of_buffer(size_type num);
};

// Class declaration of the GapBufferIterator class
template <typename T>
class GapBufferIterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
    friend class GapBuffer<T>;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using iterator = GapBufferIterator<T>;

    reference operator*();
    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    // the operators below are implemented as non-friend non-members
    // iterator operator+(const iterator& lhs, size_type diff);
    // iterator operator-(const iterator& lhs, size_type diff);
    // iterator operator+(size_type diff, const iterator& rhs);

    /* we implemented these for you. don't change these */
    friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs._index == rhs._index; }
    friend bool operator!=(const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }
    friend difference_type operator-(const iterator& lhs, const iterator& rhs) { return lhs._index - rhs._index; }
    iterator& operator+=(size_type diff) { _index += diff; return *this; }
    iterator& operator-=(size_type diff) { _index -= diff; return *this; }
    friend bool operator>(const iterator& lhs, const iterator& rhs)  { return lhs._index < rhs._index; }
    friend bool operator<(const iterator& lhs, const iterator& rhs)  { return lhs._index > rhs._index; }
    friend bool operator<=(const iterator& lhs, const iterator& rhs)  { return !(lhs > rhs); }
    friend bool operator>=(const iterator& lhs, const iterator& rhs)  { return !(lhs < rhs); }
    reference operator[](size_t index) { return *(*this + index); }

private:
    GapBuffer<T>* _pointee;
    size_t _index;
    GapBufferIterator(GapBuffer<T>* pointee, size_t index) : _pointee(pointee), _index(index) {}
};

template <typename T>
GapBuffer<T>::GapBuffer():
    _buffer_size(kDefaultSize),
    _logical_size(0),
    _cursor_index(0),
    _gap_size(_buffer_size - _logical_size),
    _elems(std::make_unique<value_type[]>(_buffer_size)){}

template <typename T>
GapBuffer<T>::GapBuffer(size_type count, const value_type& val):
    _buffer_size(max(kDefaultSize, 2*_logical_size)),
    _logical_size(count),
    _cursor_index(count),
    _gap_size(_buffer_size - _logical_size),
    _elems(std::make_unique<value_type[]>(_buffer_size)){
    std::fill(_elems.get(), _elems.get() + count, val);
}

template <typename T>
void GapBuffer<T>::insert_at_cursor(const_reference element) {
    if(_logical_size == _buffer_size) {
        reserve(2 * _buffer_size);
    }
    _elems[_cursor_index++] = element;
    _logical_size++;
    _gap_size--;
}

template <typename T>
void GapBuffer<T>::delete_at_cursor() {
    if(_cursor_index != 0) {
        _cursor_index--;
        _logical_size--;
        _gap_size++;
    }
}

template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::get_at_cursor() {
    return const_cast<reference>(static_cast<const GapBuffer<T>*>(this)->get_at_cursor());
}

template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::at(size_type pos) {
    return const_cast<reference>(static_cast<const GapBuffer<T>*>(this)->at(pos));
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::size() const {
    return _logical_size;
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::cursor_index() const {
    return _cursor_index;
}

template <typename T>
bool GapBuffer<T>::empty() const {
    return _logical_size == 0;
}

template <typename T>
typename GapBuffer<T>::const_reference GapBuffer<T>::get_at_cursor() const {
    if (_logical_size == _cursor_index){
            throw ("cursor: array_index is out of bounds!");
    }
    return at(_cursor_index);
}

template <typename T>
typename GapBuffer<T> ::const_reference GapBuffer<T>::at(size_type pos) const {
    if(pos >= _logical_size) {
         throw ("at: pos is out of bounds!");
    }
    return _elems[to_array_index(pos)];
}

template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::operator[](size_type pos) {
    return const_cast<reference>(static_cast<const GapBuffer<T>*>(this)->operator[](pos));
}

template <typename T>
typename GapBuffer<T>::const_reference GapBuffer<T>::operator[](size_type pos) const {
    return at(pos);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const GapBuffer<T>& buf) {
    os << "{";
    size_t size = buf.size();
    size_t current_index = buf.cursor_index();
    size_t index = 0;
    for(index; index < size; ++index) {
        if(index > 0) {
            os << ", ";
        }
        if(index == current_index) {
            os << "^";
        }
        os << buf[index];
    }
    if(index == current_index) {
        os << "^";
    }
    os << "}";
    return os;
}

template <typename T>
bool operator==(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    auto& lhs_nonconst = const_cast<GapBuffer<T>&>(lhs);
    auto& rhs_nonconst = const_cast<GapBuffer<T>&>(rhs);
    return std::equal(lhs_nonconst.begin(),lhs_nonconst.end(),rhs_nonconst.begin(),rhs_nonconst.end());
}

template <typename T>
bool operator!=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    auto& lhs_nonconst = const_cast<GapBuffer<T>&>(lhs);
    auto& rhs_nonconst = const_cast<GapBuffer<T>&>(rhs);
    return std::lexicographical_compare(lhs_nonconst.begin(),lhs_nonconst.end(),rhs_nonconst.begin(),rhs_nonconst.end());
}

template <typename T>
bool operator>(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    auto& lhs_nonconst = const_cast<GapBuffer<T>&>(lhs);
    auto& rhs_nonconst = const_cast<GapBuffer<T>&>(rhs);
    return std::lexicographical_compare(rhs_nonconst.begin(),rhs_nonconst.end(),lhs_nonconst.begin(),lhs_nonconst.end());
}

template <typename T>
bool operator<=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs > rhs);
}

template <typename T>
bool operator>=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs < rhs);
}

template <typename T>
typename GapBufferIterator<T>::reference GapBufferIterator<T>::operator*() {
    return _pointee->at(_index);
}

template <typename T>
GapBufferIterator<T>& GapBufferIterator<T>::operator++() {
    _index++;
    return *this;
}

template <typename T>
GapBufferIterator<T> GapBufferIterator<T>::operator++(int) {
    return iterator(_pointee, _index++);
}

template <typename T>
GapBufferIterator<T>& GapBufferIterator<T>::operator--() {
    _index--;
    return *this;
}

template <typename T>
GapBufferIterator<T> GapBufferIterator<T>::operator--(int) {
    return iterator(_pointee, _index--);
}

template <typename T>
GapBufferIterator<T> operator+(const GapBufferIterator<T>& lhs,
                               typename GapBufferIterator<T>::size_type diff) {
    GapBufferIterator<T> rhs = lhs;
    rhs += diff;
    return rhs;
}

template <typename T>
GapBufferIterator<T> operator+(typename GapBufferIterator<T>::size_type diff,
                               const GapBufferIterator<T>& rhs) {
    return rhs+diff;
}

template <typename T>
GapBufferIterator<T> operator-(const GapBufferIterator<T>& lhs,
                               typename GapBufferIterator<T>::size_type diff) {
    GapBufferIterator<T> rhs = lhs;
    rhs -= diff;
    return rhs;
}

template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::begin() {
    return iterator(this, 0);
}

template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::end() {
    return iterator(this, _logical_size);
}

template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::cursor() {
    return iterator(this, _cursor_index);
}

// Part 6: Constructors and assignment

template <typename T>
GapBuffer<T>::~GapBuffer() {
}
template <typename T>
GapBuffer<T>::GapBuffer(std::initializer_list<T> init):
    _buffer_size(max(kDefaultSize,2*_logical_size)),
    _logical_size(init.size()),
    _cursor_index(init.size()),
    _gap_size(_buffer_size - _logical_size),
    _elems(std::make_unique<value_type[]>(_buffer_size)){
    std::copy(init.begin(),init.end(),_elems.get());
}

template <typename T>
GapBuffer<T>::GapBuffer(const GapBuffer<T>& other) :
    _buffer_size(other._buffer_size),
    _logical_size(other._logical_size),
    _cursor_index(other._cursor_index),
    _gap_size(_buffer_size - _logical_size),
    _elems(std::make_unique<value_type[]>(_buffer_size)){
    auto& another = const_cast<GapBuffer<T>&>(other);
    std::copy(another.begin(),another.end(),begin());
}

template <typename T>
GapBuffer<T>& GapBuffer<T>::operator=(const GapBuffer& rhs) {
    if(this != &rhs) {
        _buffer_size = rhs._buffer_size;
        _logical_size = rhs._logical_size;
        _cursor_index = rhs._cursor_index;
        _gap_size = _buffer_size - _logical_size;

        _elems = std::make_unique<value_type[]>(_buffer_size);
        auto& rhs_nonconst = const_cast<GapBuffer<T>&>(rhs);
        std::copy(rhs_nonconst.begin(),rhs_nonconst.end(),begin());
    }
    return *this;
}

// Part 7: Move semantics
template <typename T>
GapBuffer<T>::GapBuffer(GapBuffer&& other):
    _buffer_size(std::move(other._buffer_size)),
    _logical_size(std::move(other._logical_size)),
    _cursor_index(std::move(other._cursor_index)),
    _gap_size(_buffer_size - _logical_size),
    _elems(std::move(other._elems)) {
    other._elems = nullptr;
}

template <typename T>
GapBuffer<T>& GapBuffer<T>::operator=(GapBuffer&& rhs) {
    if(this != &rhs) {
        _buffer_size = std::move(rhs._buffer_size);
        _logical_size = std::move(rhs._logical_size);
        _cursor_index = std::move(rhs._cursor_index);
        _gap_size = _buffer_size - _logical_size;
        _elems = std::move(rhs._elems);
        rhs._elems = nullptr;
    }
    return *this;
}

template <typename T>
void GapBuffer<T>::insert_at_cursor(value_type&& element) {
    if(_logical_size == _buffer_size) {
        reserve(2 * _buffer_size);
    }
    _elems[_cursor_index++] = std::move(element);
    _logical_size++;
    _gap_size--;
}

// Part 8: Make your code RAII-compliant - change the code throughout

// optional:
template <typename T>
template <typename... Args>
void GapBuffer<T>::emplace_at_cursor(Args&&... args) {
    if(_logical_size == _buffer_size) {
        reserve(2 * _buffer_size);
    }
    _elems[_cursor_index++] = std::move(T(std::forward<Args>(args)...));
    _logical_size++;
    _gap_size--;
}


// We've implemented the following functions for you.
// However...they do use raw pointers, so you might want to turn them into smart pointers!
template <typename T>
void GapBuffer<T>::move_cursor(int delta) {
    int new_index = _cursor_index + delta;
    if (new_index < 0 || new_index > static_cast<int>(_buffer_size)) {
        throw std::string("move_cursor: delta moves cursor out of bounds");
    }
    if (delta > 0) {
        auto begin_move = _elems.get() + _cursor_index + _gap_size;
        auto end_move = begin_move + delta;
        auto destination = _elems.get() + _cursor_index;
        std::move(begin_move, end_move, destination);
    } else {
        auto end_move = _elems.get() + _cursor_index;
        auto begin_move = end_move + delta;
        auto* destination = _elems.get() + _cursor_index + _gap_size + delta;
        std::move(begin_move, end_move, destination);
    }
    _cursor_index += delta;
}

template <typename T>
void GapBuffer<T>::reserve(size_type new_size) {
    if (_logical_size >= new_size) return;
    auto new_elems = std::make_unique<T[]>(new_size);
    std::move(_elems.get(), _elems.get() + _cursor_index, new_elems.get());
    size_t new_gap_size = new_size - _logical_size;
    std::move(_elems.get() + _buffer_size - _logical_size + _cursor_index,
              _elems.get() + _buffer_size,
              new_elems.get() + _cursor_index + new_gap_size);
    _buffer_size = new_size;
    _elems = std::move(new_elems);
    _gap_size = new_gap_size;
}

template <typename T>
void GapBuffer<T>::debug() const {
    std::cout << "[";
    for (size_t i = 0; i < _buffer_size; ++i) {
        if (i == _cursor_index) {
            std::cout << "|";
        } else {
            std::cout << " ";
        }
        if (i >= _cursor_index && i < _cursor_index + _gap_size) {
            std::cout << "*";
        } else {
            std::cout << _elems[i];
        }
    }
    std::cout << (_cursor_index == _buffer_size ? "|" : " ");
    std::cout << "]" << std::endl;
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::to_external_index(size_type array_index) const {
    if (array_index < _cursor_index) {
        return array_index;
    } else if (array_index >= _cursor_index + _gap_size){
        return array_index - _cursor_index;
    } else {
        throw ("to_external_index: array_index is out of bounds!");
    }
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::to_array_index(size_type external_index) const {
    if (external_index < _cursor_index) {
        return external_index;
    } else {
        return external_index + _gap_size;
    }
}


#endif // GAPBUFFER_H
