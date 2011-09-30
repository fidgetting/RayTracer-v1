/* ****************************************************************************
 * Copyright (C) 2010 Alex Norton                                             *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify it    *
 * under the terms of the BSD 2-Clause License.                               *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRENTY; without even the implied warranty of                 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 **************************************************************************** */

#ifndef QUEUE_TPP_INCLUDE
#define QUEUE_TPP_INCLUDE

#include <thread>
#include <deque>

/**
 * A simple concurrent queue implementation. This is by definition thread-safe.
 * This type that this concurrent queue implements should be a functor of some
 * kind.
 *
 * The functor should be of type:
 *   bool(*functor)(void);
 *
 * If the functor returns true, it will be placed back in the queue, otherwise
 * it will be deleted.
 *
 * @file queue.tpp
 */
template<typename T>
class concurrent_queue {
  public:

    typedef typename std::deque<T*>::iterator       iterator;
    typedef typename std::deque<T*>::const_iterator const_iterator;

    concurrent_queue() { }
    virtual ~concurrent_queue() { }

    inline iterator       begin()       { return _queue.begin(); }
    inline const_iterator begin() const { return _queue.begin(); }
    inline iterator         end()       { return _queue.end();   }
    inline const_iterator   end() const { return _queue.end();   }

    void push(T* t);

    unsigned int size() const { return _queue.size(); }
    void worker();

  protected:

    std::mutex       _lock;
    std::deque<T*>   _queue;
};

/**
 * Pushes a new element into the concurrent queue.
 *
 * @param t the element to put into the concurrent queue
 */
template<typename T>
void concurrent_queue<T>::push(T* t) {
  std::unique_lock<std::mutex> ul(_lock);
  _queue.push_back(t);
}

/**
 * Working function. This concurrent queue implements a single producer thread
 * and many worker thread architecture. All the worker threads should be created
 * by calling this function. This function will return when there is no longer
 * anything available in the queue.
 */
template<typename T>
void concurrent_queue<T>::worker() {
  T* ret;

  while(size() != 0) {
    {
      std::unique_lock<std::mutex> ul(_lock);
      if(size() != 0) {
        ret = _queue.front();
        _queue.pop_front();
      } else {
        break;
      }
    }

    if(ret->operator()()) {
      std::unique_lock<std::mutex> ul(_lock);
      _queue.push_back(ret);
    } else {
      delete ret;
    }
  }
}

#endif /* QUEUE_TPP_INCLUDE */
