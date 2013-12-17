/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_FACTORY_H
#define ANP_FACTORY_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : Factory
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Abstract template factory 
 *  
 *  Factory stores creator functors to template parameter class. 
 *  Functors are registered using prepocessor macros defined below.
 *  Factory is a singleton class which is explicitly deleted.
 * 
 *  Source: Andrei Alexandrescu, Modern C++ Design
 * 
 **********************************************************************************/

// C++
#include <iostream>
#include <map> 
#include <string>
#include <typeinfo>
#include <vector>

// Local
#include "Handle.h"

namespace Anp
{
  class FactoryBase: public Base {
  public:
    FactoryBase() {}
    virtual ~FactoryBase() {}

    virtual void Clear() = 0;   
  };

  inline std::vector<FactoryBase *>& GetFactoryList() { 
    static std::vector<FactoryBase *> FactoryList;
    return FactoryList; 
  }

   template<class T>
     class Factory: public FactoryBase
   {       
   public:
      
      // typedef for functor that creates object of class T
      typedef Handle<T> (*Creator)(); 
      
   public:
      
      static Factory& Instance();
      
      bool Register(const std::string &name, Creator creator); 

      bool Unregister(const std::string &name);

      Handle<T> Create(const std::string &name); 
      
      const std::vector<std::string> List() const;
 

      bool Hold(const std::string &name, Handle<T> ptr);

      Handle<T> Get(const std::string &name) const;
      
      std::map<std::string, Handle<T> >& GetHoldMap() const; 

      bool Remove(const std::string &name);

      void Clear();

      void Print() const;

   private:

      // must use Instance() method to access/create Factory
      Factory();
      virtual ~Factory();

      // Factory is singleton and can not be copied
      // These two methods are private and not defined by design
      Factory(const Factory &);
      const Factory& operator =(const Factory &);
      
   private: 
      
      static Factory *fgInstance;
      
      typedef std::map<std::string, Creator> CallMap; 
      typedef std::map<std::string, Handle<T> > HoldMap; 

      CallMap fCalls;
      HoldMap fHolds;           
   };
   
   //
   // Initialize static singleton pointer
   //
   template<class T> Factory<T>* Factory<T>::fgInstance = 0;
}

#ifndef __CINT__

/////////////////////////////////////////////////////////////////
///
/// REGISTER_ANP_OBJECT(BaseClass,MyClass)
/// 
/// expands to this code:
///
/// namespace
/// {
///    Anp::Handle<Anp::BaseClass> CreateBaseClass()
///    {
///       return Anp::Handle<Anp::BaseClass>(new Anp::MyClass);
///    }
///    bool RegisteredBaseClass = Anp::Factory<Anp::BaseClass>::Instance(). 
///       Register("MyClass", CreateBaseClass);
/// }
///
/////////////////////////////////////////////////////////////////

#define REGISTER_ANP_OBJECT(BASE,CLASS)					\
   namespace								\
   {									\
      Anp::Handle<Anp::BASE> Create##BASE()				\
      {									\
	 return Anp::Handle<Anp::BASE>(new Anp::CLASS);			\
      }									\
      bool Registered##BASE = Anp::Factory<Anp::BASE>::Instance().	\
	 Register(#CLASS, Create##BASE);				\
   }

#define REGISTER_ANY_OBJECT(PREF,BASE,CLASS)				\
   namespace								\
   {									\
      Anp::Handle<PREF::BASE> Create##PREF##BASE()			\
      {									\
	 return Anp::Handle<PREF::BASE>(new PREF::CLASS);		\
      }									\
      bool Registered##BASE = Anp::Factory<PREF::BASE>::Instance().	\
	 Register(#CLASS, Create##PREF##BASE);				\
   }

///
/// Definitions of template functions
///

template<class T>
Anp::Factory<T>::Factory()
{
  Anp::GetFactoryList().push_back(this);
}

template<class T>
Anp::Factory<T>::~Factory()
{
  fgInstance = 0;
}

template<class T>
Anp::Factory<T>& Anp::Factory<T>::Instance()
{
   if(!fgInstance)
   {
      fgInstance = new Anp::Factory<T>();
   }

  return *fgInstance;
}

template<class T>
bool Anp::Factory<T>::Register(const std::string &name, Creator creator) 
{ 
   if(!fCalls.insert(typename CallMap::value_type(name, creator)).second)
   {
      std::cerr << "Factory<T>::Register - \"" << name << "\" already exists" << std::endl;
      return false;
   }

   return true;
}

template<class T>
bool Anp::Factory<T>::Unregister(const std::string &name) 
{ 
    return fCalls.erase(name) == 1; 
}

template<class T>
Anp::Handle<T> Anp::Factory<T>::Create(const std::string &name) 
{
   typename CallMap::const_iterator it = fCalls.find(name);
   
   // handle unknown algorithm request
   if(it == fCalls.end())
   {
      std::cerr << "Factory<T>::Create - don't know anything about: \"" << name << "\"" << std::endl;
      return Handle<T>(NULL);
    }
   
   return (it->second)();
}

template<class T>
const std::vector<std::string> Anp::Factory<T>::List() const
{
   std::vector<std::string> svec;

   typename CallMap::const_iterator it = fCalls.begin();
   for(; it != fCalls.end(); ++it)
   {
      svec.push_back(it -> first);
   }

   return svec;
}

template<class T>
bool Anp::Factory<T>::Hold(const std::string &name, Handle<T> ptr)
{
   return fHolds.insert(typename HoldMap::value_type(name, ptr)).second;
}

template<class T>
Anp::Handle<T> Anp::Factory<T>::Get(const std::string &name) const
{
   typename HoldMap::const_iterator it = fHolds.find(name);

   // handle unknown algorithm request
   if(it == fHolds.end())
   {
      std::cerr << "Factory<T>::Get - don't know anything about: \"" << name << "\"" << std::endl;
      return Handle<T>(0);
    }
   
   return it->second;
}

template<class T>
std::map<std::string, Anp::Handle<T> >& Anp::Factory<T>::GetHoldMap() const
{
  return fHolds;
}

template<class T>
bool Anp::Factory<T>::Remove(const std::string &name)
{
   //
   // Atempt to erase handle with key "name"
   //
   if(fHolds.erase(name) < 1)
   {
     std::cerr << "Factory<T>::Remove - don't know anything about: \"" << name << "\"" << std::endl;
      return false;
   }

   return true;
}

template<class T>
void Anp::Factory<T>::Clear()
{
  typename HoldMap::iterator it = fHolds.begin();
  for(; it != fHolds.end(); ++it) {
    T *ptr = it->second.get();
    std::cout << "Factory::Clear - erasing handle to class: " << typeid(*ptr).name() << std::endl;
    it->second.release();
  }

  fHolds.clear();
}

template<class T>
void Anp::Factory<T>::Print() const
{
   std::cout << "Print: Factory<> knows about " << fCalls.size() << " objects" << std::endl;  

   typename CallMap::const_iterator it = fCalls.begin();
   for(; it != fCalls.end(); ++it)
   {
      std::cout << "Registerted object name " << it -> first << std::endl;
   }
}

#endif
#endif


