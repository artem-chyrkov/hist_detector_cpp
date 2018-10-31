#ifndef _CIRCULARLIST_H_
#define _CIRCULARLIST_H_

#include <vector>

template<class T>
class CircularList
{
public:
    CircularList();
    void init(int _maxSize);
    void add(T x);
    const std::vector<T>& get() const;
    std::size_t getActualSize() const;
    void clear();

private:
    int maxSize;
    int indexToReplace;
    std::size_t actualSize;
    std::vector<T> circularList;
};

#endif // _CIRCULARLIST_H_
