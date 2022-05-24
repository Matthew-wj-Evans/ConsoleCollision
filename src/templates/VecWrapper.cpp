#ifndef VEC_WRAPPER_CPP
#define VEC_WRAPPER_CPP

#include <vector>
#include <memory>

/*
    Class template code from
    C++ Primer Fifth Edition, p659

*/
template <typename T>
class VecWrapper
{
public:
    typedef T value_type;
    typedef typename std::vector<T>::size_type size_type;
    // Constructors
    VecWrapper();
    VecWrapper(std::initializer_list<T> il);
    // Element count
    size_type size() const { return data->size(); }
    bool empty() const { return data->empty(); }
    // Add and Remove
    void push_back(const T &t) { data->push_back(t); }
    void push_back(T &&t) { data->push_back(std::move(t)); }
    void pop_back();
    // Element access
    T &back();
    T &operator[](size_type i);
    // Remove element at index
    void RemoveElement(size_type i);
private:
    std::shared_ptr<std::vector<T>> data;
    void check(size_type i, const std::string &msg) const;
};

// VecWrapper<int> intList = {1,2,3};
template <typename T>
VecWrapper<T>::VecWrapper() : data(std::make_shared<std::vector<T>>()) {}

// VecWrapper<int> intList2(intList);
template <typename T>
VecWrapper<T>::VecWrapper(std::initializer_list<T> il) : data(std::make_shared<std::vector<T>>(il)) {}

template <typename T>
void VecWrapper<T>::check(size_type i, const std::string &msg) const
{
    if (i >= data->size())
    {
        throw std::out_of_range(msg);
    }
}

template <typename T>
T &VecWrapper<T>::back()
{
    check(0, "back on empty VecWrapper");
    return data->back();
}

template <typename T>
T &VecWrapper<T>::operator[](size_type i)
{
    check(i, "subscript out of range");
    return (*data)[i];
}

template <typename T>
void VecWrapper<T>::pop_back()
{
    check(0, "pop_back on empty VecWrapper");
    data->pop_back();
}



// My code //

template <typename T>
void VecWrapper<T>::RemoveElement(size_type i)
{
    check(i, "subscript out of range");
    T tempType = (*data)[data->size() - ((size_type) 1)];
    (*data)[data->size() - 1] = (*data)[i];
    (*data)[i] = tempType;
    (*data).pop_back();
}

#endif