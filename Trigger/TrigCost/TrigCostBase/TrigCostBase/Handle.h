/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HANDLE_H
#define ANP_HANDLE_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : Base, Handle
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Class Base defines (and implements) reference counting. All objects
 *  using Handle<> smart pointer should inherit from Base. All classes
 *  inherting from Base MUST have virtual destructor. A derived class
 *  must not redefine three public functions of Base class. Base class
 *  does not allow copy constructor and assignment operator.
 *  
 * 
 *  Handle class template - a simplified version of boost::intrusive_ptr. 
 * 
 *  It's a smart pointer for objects with "built-in" reference count. 
 *  Such implemetation makes the smallest possible smart pointer - its 
 *  size is equivalent to just a single raw pointer.
 *
 * @Sources: Andrei Alexandrescu, Modern C++ Design
 *           Sergei Avvakumov
 * 
 **********************************************************************************/

// C++/C
#include <cassert>
#include <iostream>

#define ANP_HANDLE_DEBUG

namespace Anp
{
   class Base {
   public:
     
     Base() : fCount(0) {}
     virtual ~Base() {}
     
     void add_ref() { ++fCount; }   
     void release() { if(--fCount == 0) delete this; }
     
     unsigned int use_count() const { return fCount; }
     
   private:
     
     // copy constructor and assignment operator are
     // private and not implemented by design
     Base(const Base &);
     Base& operator=(const Base &);
     
   private:
     
     unsigned int fCount;
   };
   
   template <typename T> class Handle {
   public:
     
    ///
    /// default ctor
    ///
    Handle();
    
    ///
    /// ctor from a raw pointer
    ///
    explicit Handle(T* ptr);
    
    ///
    /// copy ctor
    ///        
    Handle(const Handle& rhs);
    
    ///
    /// dtor
    ///
    ~Handle();

    ///
    /// assignment operator
    ///
    const Handle& operator=(const Handle& rhs);
    
    ///
    /// operator-> allows usage with pointer semantics
    /// 
    T* operator->() const;

    ///
    /// provides access to pointer
    /// 
    T* get() const;
    
    ///
    /// dereference operator
    ///
    T& operator*() const;

    ///
    /// provides access to reference
    /// 
    T& ref() const;

    ///
    /// check if pointer is set to 0
    ///
    bool valid() const;

    ///
    /// release - set pointer to 0
    ///
    void release();

private:

    T* p;

    void init();
};


template <typename T>
Handle<T>::Handle() : p(0)
{}

///
/// ctor from a raw pointer
///
template <typename T>
Handle<T>::Handle(T* realPtr) : p(realPtr)
{
  init();
}

///
/// copy ctor
///        
template <typename T>
Handle<T>::Handle(const Handle& rhs) : p(rhs.p)
{
  init();
}

///
/// assignment operator
///
template <typename T>
const Handle<T>& Handle<T>::operator=(const Handle& rhs)
{
  if (p != rhs.p)
  {
    if (p)
    {
      p->release();
    }
      
    p = rhs.p;
    init();
  }

  return *this;
}


///
/// init
///
template <typename T>
void Handle<T>::init()
{
  if (p)
  {
    p->add_ref();
  }
}

///
/// dtor
///
template <typename T>
Handle<T>::~Handle()
{
  if(p)
  {
    p->release();
  }
}

///
/// operator-> allows usage with pointer semantics
/// 
template <typename T>
T* Handle<T>::operator->() const 
{ 
  return p; 
}

///
/// provides access to pointer
/// 
template <typename T>
T* Handle<T>::get() const 
{ 
  return p; 
}

///
/// dereference operator
///
template <typename T>
T& Handle<T>::operator*() const 
{ 
  assert(p && "Handle error: refernce for null pointer");
  return *p; 
}

///
/// provides access to pointer
/// 
template <typename T>
T& Handle<T>::ref() const 
{ 
  assert(p && "Handle error: refernce for null pointer");
  return *p; 
}

///
/// check that handle points to non zero pointer
///
template <typename T>
bool Handle<T>::valid() const
{
  return p;
}

///
/// release - set pointer to 0
///
template <typename T>
   void Handle<T>::release()
{
  if(p) {
    p->release();
  }

  p = 0;
}
   
///
/// Handle<T> can be printed with "cout << handle;"
///
template <typename T>
std::ostream& operator<<(std::ostream& o, const Handle<T>& handle)
{
   o << *handle;
   return o;
}

} // end of Anp namespace

#endif  // HANDLE_H
