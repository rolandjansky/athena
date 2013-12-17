/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_OBJECTFACTORY_H
#define ANP_OBJECTFACTORY_H

/**********************************************************************************
 * @Package: PhysicsBase
 * @Class  : Ptr, ObjectFactory
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Ptr template - smart non-intrusive reference counting pointer 
 *   - when reference count drops to 0, return object to ObjectFactory pool
 *  
 * 
 *  ObjectFactory template - create new object
 *   - objects are re-used with pool
 *   - objects are created with new when necessary (pool empty)
 * 
 *
 **********************************************************************************/

// C++
#include <vector>

// Local
#include "Factory.h"
#include "Registry.h"

namespace Anp
{
  template <class T> class ObjectFactory;

  //
  // Reference counting smart pointer - objects are recycled using factory
  //
  template <typename T> class Ptr {
  private:
    //
    // ctor from a raw pointer - only accesible through factory
    //
    explicit Ptr(T* ptr_, int *count_);

    friend class ObjectFactory<T>;

  public:
    
    //
    // default ctor
    //
    Ptr() :ptr(0), count(0) {}
      
    //
    // copy ctor
    //        
    Ptr(const Ptr& rhs);
    
    //
    // dtor
    //
    ~Ptr();

    //
    // assignment operator
    //
    const Ptr& operator=(const Ptr& rhs);
    
    //
    // operator-> allows usage with pointer semantics
    // 
    T* operator->() const { return ptr; }

    //
    // provides access to pointer
    // 
    T* get() const { return ptr; }
    
    //
    // dereference operator
    //
    T& operator*() const { return *ptr; }

    //
    // provides access to reference
    // 
    T& ref() const { return *ptr; }

    //
    // check if pointer is set to 0
    //
    bool valid() const { return ptr; }
    
    //
    // release - set pointer to 0
    //
    void release();

    //
    // deep copy - make new Ptr with copied data
    //
    Ptr<T> MakeCopy() const;

  private:

    void init();

  private:

    T   *ptr;    // Pointer to data
    int *count;  // Pointer to reference count
  };

  //
  // Object factory - re-use objects with pool
  //
  template<class T>
    class ObjectFactory: public FactoryBase
    {       
    public:
      
      static ObjectFactory& Instance();
      
      Ptr<T> CreateObject();

      Ptr<T> CreateObject(const T &);
      
      void Clear();
      
      void ConfigRegistry(const Registry &reg);

    private:

      void HoldObject(T *ptr, int *count);

    private:
      
      //
      // Use Instance() method to access/create Factory
      //
      ObjectFactory();
      virtual ~ObjectFactory() {}

      //
      // These two methods are private and not defined by design
      //
      ObjectFactory(const ObjectFactory &);
      const ObjectFactory& operator =(const ObjectFactory &);
      
    private:
      
      friend class Ptr<T>;

    private:

      struct PoolData {
	PoolData() : pool_ptr(0), pool_count(0) {}
	PoolData(T *ptr, int *count) : pool_ptr(ptr), pool_count(count) {}

	T   *pool_ptr;
	int *pool_count;
      };
      
      typedef std::vector<PoolData> PoolVec; 
      
      PoolVec   fPool;          // Pool of available T object pointers
      bool      fDebug;         // Print debugging info
      unsigned  fCountCreate;   // Count create calls
      unsigned  fCountHold;     // Count hold calls
      unsigned  fCountNew;      // Count new operator calls
    };

#ifndef __CINT__

  //----------------------------------------------------------------------------------------------
  //
  // Implementations of template functions
  //  
  template<class T>
    ObjectFactory<T>::ObjectFactory():
    fDebug(false),
    fCountCreate(0),
    fCountHold(0),
    fCountNew(0)
    { 
      Anp::GetFactoryList().push_back(this);
    } 

  template<class T>
    ObjectFactory<T>& ObjectFactory<T>::Instance()
    {
      static Anp::ObjectFactory<T> gInstance;
      return gInstance;
    }

  template<class T>
    Ptr<T> ObjectFactory<T>::CreateObject()
    { 
      ++fCountCreate;

      if(fPool.empty()) 
	{
	  ++fCountNew;
	  return Ptr<T>(new T, new int(0));
	}
      
      Ptr<T> h(fPool.back().pool_ptr, fPool.back().pool_count);
      fPool.pop_back();

      static T initT;
      h.ref() = initT;
      
      return h;
    }

  template<class T>
    Ptr<T> ObjectFactory<T>::CreateObject(const T &val)
    { 
      ++fCountCreate;

      if(fPool.empty()) 
	{
	  ++fCountNew;
	  return Ptr<T>(new T(val), new int(0));
	}
      
      Ptr<T> h(fPool.back().pool_ptr, fPool.back().pool_count);
      fPool.pop_back();

      h.ref() = val;
      
      return h;
    }
 
  template<class T>
    void ObjectFactory<T>::HoldObject(T *ptr, int *count)
    { 
      ++fCountHold;
      fPool.push_back(PoolData(ptr, count));
    } 

  template<class T>
    void ObjectFactory<T>::Clear()
    { 
      if(fDebug) {
	std::cout << "ObjectFactory: " << typeid(*this).name() << " - usage summary:" << std::endl
		  << "   count create: " << fCountCreate << std::endl
		  << "   count hold:   " << fCountHold   << std::endl
		  << "   count new:    " << fCountNew    << std::endl
		  << "   pool size:    " << fPool.size() << std::endl;
      }
      
      for(unsigned i = 0; i < fPool.size(); ++i) {
	PoolData &p = fPool.at(i);
	delete p.pool_ptr;
	delete p.pool_count;
      }
    }

  template<class T>
    void ObjectFactory<T>::ConfigRegistry(const Registry &reg)
    { 
      reg.Get("ObjectFactory", "Debug", fDebug);
    }

  //----------------------------------------------------------------------------------------------
  //
  // Ptr template implementation
  //
  template <typename T>
    Ptr<T>::Ptr(T *ptr_, int *count_)
    :ptr(ptr_), count(count_)
  {
    init();
  }

  template <typename T>
    Ptr<T>::Ptr(const Ptr& rhs)
  {
    ptr   = rhs.ptr;
    count = rhs.count;
    init();
  }

  template <typename T>
    Ptr<T>::~Ptr()
  {
    if(ptr) {
      release();
    }
  }

  template <typename T>
  const Ptr<T>& Ptr<T>::operator=(const Ptr& rhs)
  {
    if (ptr != rhs.ptr) {
      if (ptr) {
	release();
      }
      
      ptr   = rhs.ptr;
      count = rhs.count;
      init();
    }
    
    return *this;
  }

  template <typename T>
    void Ptr<T>::init()
  {
    if(ptr) ++(*count);
  }

  template <typename T>
    void Ptr<T>::release()
  {
    if(ptr && --(*count) == 0) {
      ObjectFactory<T>::Instance().HoldObject(ptr, count);
    }
    
    ptr   = 0;
    count = 0;
  }

  template <typename T>
    Ptr<T> Ptr<T>::MakeCopy() const
  {
    if(!valid()) {
      return Ptr<T>();
    }

    Ptr<T> copy = ObjectFactory<T>::Instance().CreateObject();    
    *copy.ptr = *ptr;

    return copy;
  }
   
  //
  // Ptr<T> can be printed with "cout << handle;"
  //
  template <typename T>
  std::ostream& operator<<(std::ostream& o, const Ptr<T>& h)
  {
    if(h.valid()) o << h.ref();
    return o;
  }
}

#endif
#endif


