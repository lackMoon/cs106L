#ifndef GAPBUFFER_H
#define GAPBUFFER_H
#include <iostream> // cout for debug
#include <algorithm> // std::copy, std::move
#include <sstream> // stringstreams
#include <memory> // for unique_ptr

const int kDefaultSize = 10;

class GapBuffer {
public:
    using value_type = char;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = char*;
    using const_pointer = const char*;

    // Part 1
    GapBuffer();
    GapBuffer(size_type count, const value_type& val = value_type());
    void insert_at_cursor(const_reference element);
    void delete_at_cursor();
    reference get_at_cursor();
    reference at(size_type pos);

    // Part 2 - you need to declare these yourself
    const_reference at(size_type pos) const;
    const_reference get_at_cursor() const;
    size_type size() const;
    size_type cursor_index() const;
    bool empty() const;

    // Part 3 - you need to declare these yourself
    // member operators
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    /* Implemented for you */
    void move_cursor(int num);
    void reserve(size_type new_size);
    void debug() const;

private:
    pointer _elems; // uses array_index
    size_type _logical_size; // uses external_index
    size_type _buffer_size; // uses array_index
    size_type _cursor_index; // uses array_index
    size_type _gap_size;
    size_type to_external_index(size_type array_index) const;
    size_type to_array_index(size_type external_index) const;
    void move_to_left_of_buffer(size_type num);
};

GapBuffer::GapBuffer():
    _buffer_size(kDefaultSize),
    _logical_size(0),
    _cursor_index(0),
    _gap_size(_buffer_size - _logical_size) {
    _elems = new value_type[_buffer_size];
}

GapBuffer::GapBuffer(size_type count, const value_type& val):
    _buffer_size(count == 0 ? kDefaultSize : 2 * count),
    _logical_size(0),
    _cursor_index(0) {
    _elems = new value_type[_buffer_size];
    while(_cursor_index < count) {
        _elems[_cursor_index++] = val;
        _logical_size++;
    }
    _gap_size = _buffer_size - _logical_size;
}

void GapBuffer::insert_at_cursor(const_reference element) {
    if(_logical_size == _buffer_size) {
        reserve(2 * _buffer_size);
    }
    _elems[_cursor_index++] = element;
    _logical_size++;
    _gap_size--;
}

void GapBuffer::delete_at_cursor() {
    if(_cursor_index != 0) {
        _cursor_index--;
        _logical_size--;
        _gap_size++;
    }
}

typename GapBuffer::reference GapBuffer::get_at_cursor() {
    return const_cast<reference>(static_cast<const GapBuffer*>(this)->get_at_cursor());
}
typename GapBuffer::const_reference GapBuffer::get_at_cursor() const {
    if (_logical_size == _cursor_index){
            throw ("cursor: array_index is out of bounds!");
    }
    return _elems[_cursor_index];
}

typename GapBuffer::reference GapBuffer::at(size_type pos) {
    return const_cast<reference>(static_cast<const GapBuffer*>(this)->at(pos));
}
typename GapBuffer::const_reference GapBuffer::at(size_type pos) const {
    if(pos >= _logical_size) {
         throw ("at: pos is out of bounds!");
    }
    return _elems[to_array_index(pos)];
}
typename GapBuffer::size_type GapBuffer::size() const {
    return _logical_size;
}

typename GapBuffer::size_type GapBuffer::cursor_index() const {
    return _cursor_index;
}

bool GapBuffer::empty() const {
    return _logical_size == 0;
}

// Implement Part 3 functions here
// Members and non-member non-friends.
typename GapBuffer::reference GapBuffer::operator[](size_type pos) {
    return const_cast<reference>(static_cast<const GapBuffer*>(this)->operator[](pos));
}

typename GapBuffer::const_reference GapBuffer::operator[](size_type pos) const{
    return at(pos);
}

/* We've implemented these for you, do not edit! */
void GapBuffer::move_cursor(int delta) {
    int new_index = _cursor_index + delta;
    if (new_index < 0 || new_index > static_cast<int>(_buffer_size)) {
        throw std::string("move_cursor: delta moves cursor out of bounds");
    }
    if (delta > 0) {
        pointer begin_move = _elems + _cursor_index + _gap_size;
        pointer end_move = begin_move + delta;
        pointer destination = _elems + _cursor_index;
        std::move(begin_move, end_move, destination);
    } else {
        pointer end_move = _elems + _cursor_index;
        pointer begin_move = end_move + delta;
        pointer destination = _elems + _cursor_index + _gap_size + delta;
        std::move(begin_move, end_move, destination);
    }
    _cursor_index += delta;
}

void GapBuffer::reserve(size_type new_size) {
    if (size() >= new_size) return;
    pointer new_elems = new char[new_size];
    std::move(_elems, _elems + _cursor_index, new_elems);
    size_t new_gap_size = new_size - _buffer_size;
    std::move(_elems+ _cursor_index, _elems + _buffer_size,
              new_elems + _cursor_index + new_gap_size);
    _buffer_size = new_size;
    _elems = std::move(new_elems);
    _gap_size = new_gap_size;
}

void GapBuffer::debug() const {
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

typename GapBuffer::size_type GapBuffer::to_external_index(size_type array_index) const {
    if (array_index < _cursor_index) {
        return array_index;
    } else if (array_index >= _cursor_index + _gap_size){
        return array_index - _cursor_index;
    } else {
        throw ("to_external_index: array_index is out of bounds!");
    }
}

typename GapBuffer::size_type GapBuffer::to_array_index(size_type external_index) const {
    if (external_index < _cursor_index) {
        return external_index;
    } else {
        return external_index + _gap_size;
    }
}

#endif // GAPBUFFER_H
