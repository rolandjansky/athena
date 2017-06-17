/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DeepCopyPointer_H
#define DeepCopyPointer_H

/** A "smart" pointer that implements deep copy and ownership.
 *  In other words, when the pointer is copied it copies the 
 *  object it points to using "new". It also deletes the
 *  object it points to when it is deleted.
 *  Very useful for use as private data member of a class:
 *  it handles the copy construction, assignment, and destruction.
 */

template <class T>
class DeepCopyPointer {
public:
  
  DeepCopyPointer() : m_theData(0) {}

  DeepCopyPointer( T* t) : m_theData(t) {}

  DeepCopyPointer( const DeepCopyPointer& other) {
    if (other.m_theData) m_theData = new T( *other); else m_theData = 0;
  }

  ~DeepCopyPointer() { delete m_theData;}

  DeepCopyPointer& operator=( const DeepCopyPointer& other) {
    if ( this != &other && m_theData != other.m_theData) {
      delete m_theData;
      if (other.m_theData) m_theData = new T( *other); else m_theData = 0;
    }
    return *this;
  }

        T* get()       { return m_theData; }
  const T* get() const { return m_theData; }

  T&       operator*()       { return *m_theData;}
  const T& operator*() const { return *m_theData;}

  T*       operator->()       { return m_theData;}
  const T* operator->() const { return m_theData;}

  /// to allow test like " if (p) {...}"
  operator bool() const { return m_theData != 0;}

  /// to allow test like " if (p == &someT) {...}"
  bool operator==( const T* otherP) const { return m_theData == otherP;}

private:
  T* m_theData;
};

#endif // DeepCopyPointer_H
