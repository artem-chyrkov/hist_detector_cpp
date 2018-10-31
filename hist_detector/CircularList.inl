#include "CircularList.h"

template<class T>
CircularList<T>::CircularList()
{
}

template<class T>
void CircularList<T>::init(int _maxSize)
{
    maxSize = _maxSize;
    indexToReplace = -1;
    actualSize = 0;
    circularList.resize(_maxSize);
}

template<class T>
void CircularList<T>::add(T x)
{
    ++indexToReplace;
    if (indexToReplace >= maxSize) indexToReplace = 0;
    if (actualSize < maxSize) ++actualSize;
    circularList[indexToReplace] = x;
}

template<class T>
const std::vector<T>& CircularList<T>::get() const
{
    return circularList;
}

template<class T>
std::size_t CircularList<T>::getActualSize() const
{
    return actualSize;
}

template<class T>
void CircularList<T>::clear()
{
    indexToReplace = -1;
    actualSize = 0;
}

//template CircularList<std::pair<Float, Float> >;
//template CircularList<Float>;
