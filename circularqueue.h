/* Copyright 2022 Visiongo Ltd.
 *
 * @author lijunyu
 * @date 2022/2/16
 * @file circularqueue.h
 */
#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <iostream>

template<typename T>
class CircularQueue
{
private:
    /* data */
    T *array;
    int head;
    int tail;
    int max_size;

public:
    CircularQueue(int size);
    ~CircularQueue();
    T front();
    int push(T data);
    int pop();
    int length();
};

template<typename T>
CircularQueue<T>::CircularQueue(int size)
{
    this->max_size = size;
    this->array = new T[size + 1];
    this->head = 0;
    this->tail = 0;
}

template<typename T>
CircularQueue<T>::~CircularQueue()
{
    delete []array;
}
template<typename T>
T CircularQueue<T>::front()
{
    if (head == tail) {
        throw std::out_of_range("CircularQueue");
    }
    return this->array[head];
}

template<typename T>
int CircularQueue<T>::push(T data)
{
    if ((tail + 1) % max_size == head) {
        return -1;
    }
    array[tail] = data;
    tail = (tail + 1) % max_size;

    return 1;
}

template<typename T>
int CircularQueue<T>::pop()
{
    if (head == tail) {
        return -1;
    }
    head = (head + 1) % max_size;

    return 1;
}

template<typename T>
int CircularQueue<T>::length()
{
    return (tail - head + max_size) % max_size;
}

#endif // CIRCULARQUEUE_H
