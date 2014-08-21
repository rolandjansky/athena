/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_DATAPTR_H
#define ATHLINKS_DATAPTR_H

#ifndef _CPP_ALGORITHM
 #include <algorithm>          // for std::swap
#endif
#include <cassert>
#ifndef __TYPEINFO__
 #include <typeinfo>
#endif


/** @class DataPtr
 * @brief a ref-counted pointer
 *
 * @param T        the pointee type
 *
 * @author ATLAS Collaboration
 * $Id: DataPtr.h 567807 2013-10-30 09:30:16Z krasznaa $
 **/

template<typename T>
class DataPtr
{
 public:

  typedef T DataPtr_type;
  typedef unsigned short counter_type;

  /// default constructor
  DataPtr() : m_pointer(0), p_count(new counter_type(0)) {
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "default constructor called on " <<this << "->" << ptr()  
    	 << " use_count " << use_count() <<endl;
#endif
  }

  /// constructor from ptr: stores a copy of pointer
  ///  Y must be convertible to a T*
  template <typename Y>
  DataPtr(Y* pointer) : m_pointer( pointer ), p_count(new counter_type(1)) {
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "pointer constructor called on " <<this << "->" << ptr()  
    	 << " input " << pointer
    	 << " use_count " << use_count() <<endl;
#endif
  }
 
  /// copy constructor: makes a copy of the pointer in rhs and of its count ptr
  DataPtr( const DataPtr& rhs ) :  
    m_pointer( rhs.m_pointer ), p_count( rhs.p_count ) 
  {
    ++(*p_count);
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "copy constructor called on " <<this << "->" << ptr()  
    	 << " input " << rhs.ptr()
    	 << " use_count " << use_count() <<endl;
#endif
  }

  /// the destructor will delete the object pointed to
  ~DataPtr() {
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "destructor called on " <<this << "->" << ptr()  
    	 << " use_count " << use_count() <<endl;
#endif
    if (*p_count == 0) {
      assert(m_pointer==0);
      delete p_count;
    } else if (*p_count == 1) {
      delete p_count;
      delete m_pointer;
    } else --(*p_count);
    
  }

  /// dereference operators:
  T& operator*()  const { return *ptr(); }
  T* operator->() const { return ptr(); }

  // converter to pointer
  operator T*() const { return ptr(); }

  /// explicit pointer accessors (sometimes necessary to help the compiler)
  T* ptr() const { return m_pointer; }

  /// assignment operator, protect against self-assignment and exceptions
  DataPtr& operator=( const DataPtr& rhs )
  {
    DataPtr<T> temp( rhs );
    swap( temp );
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "equal operator called on " <<this << "->" << ptr()  
    	 << " input " << rhs.ptr()
    	 << " use_count " << use_count() <<endl;
#endif
    return *this;
  }

  /// assignment from a Convertible DataPtr
  template <typename U>
  DataPtr& operator=( const DataPtr<U>& rhs )
  {
    DataPtr<T> temp( rhs );
    swap( temp );
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "convert equal operator called on " <<this << "->" << ptr()  
    	 << " input " << rhs.ptr()
    	 << " use_count " << use_count() <<endl;
#endif
    return *this;
  }

  /// swap contents
  void swap( DataPtr& rhs ) 
  { 
    std::swap(m_pointer, rhs.m_pointer); 
    std::swap(p_count, rhs.p_count); 
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "swap called on " <<this << "->" << ptr()  
    	 << " input " << rhs.ptr()
    	 << " use_count " << use_count() <<endl;
#endif
  }

  /// reset contents
  void reset() 
  { 
    swap(DataPtr<T>(), this); 
#ifdef DATAPTR_DEBUG
    cout << typeid(*this).name() << " DEBUG: "
         << "reset called on " <<this << "->" << ptr()  
    	 << " use_count " << use_count() <<endl;
#endif
  }

  ///returns number of objects using the shared_ptr. Slow!
  long use_count() const { assert(p_count); return *p_count; }

 private:
  T* m_pointer;
  counter_type* p_count;

};

//comparison operator at the object level
//this of course requires the pointed-to type T to define operator ==
template <typename T>
inline bool operator==(const DataPtr<T>& lhs, const DataPtr<T>& rhs) {
  return (lhs.ptr() == rhs.ptr());
  //FIXME this requires T to be concrete
  //  boost::function_requires< boost::EqualityComparableConcept<T> >();
  //FIXME this requires T to have an == oper
  //  return ((lhs.operator->() == 0  && rhs.operator->() == 0) ||
  //	  (lhs.operator->() != 0  && rhs.operator->() != 0 && *lhs == *rhs) );
} 
template <typename T>
inline bool operator!=(const DataPtr<T>& lhs, const DataPtr<T>& rhs) {
  return !(lhs == rhs);
} 


#endif // not ATHLINKS_DATAPTR_H
