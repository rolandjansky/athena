/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H
#define GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H

#include "Pythia8/UserHooks.h"

#include <string>
#include <map>


#define PYTHIA8_PTRWRAP(A) A
#ifdef PYTHIA_VERSION_INTEGER
  #if PYTHIA_VERSION_INTEGER > 8299
    #define PYTHIA8_3SERIES
    typedef std::shared_ptr<Pythia8::UserHooks> UserHooksPtrType;
    #undef PYTHIA8_PTRWRAP
    #define PYTHIA8_PTRWRAP(A) std::shared_ptr<Pythia8::UserHooks>(A)
  #else
    typedef Pythia8::UserHooks* UserHooksPtrType;
  #endif
#else
  typedef Pythia8::UserHooks* UserHooksPtrType;
#endif

namespace Pythia8_UserHooks{
 
  using Pythia8::UserHooks;
  using std::string;
  
  
  class UserHooksFactory{
    
  public:
    
    static UserHooks *create(const string &hookName);
    
  private:
    
    UserHooksFactory(){};
    
    class ICreator{
    public:
      virtual UserHooks *create() const = 0;
      virtual ~ICreator(){};
    };
    
  public:
    
    template <class T>
    class Creator: public ICreator{
      
    public:
      
      Creator(const string &name){
        m_name = name;
        UserHooksFactory::s_creators()[name] = this;
      }
      
      ~Creator(){
        if(s_creators()[m_name] == this){
          s_creators().erase(m_name);
        }
      }
      
      UserHooks *create()const{
        return new T;
      }
      
    private:
      
      string m_name;
    };
    
    template<typename T>
    static std::map<std::string, T> &userSettings();
    
    static std::map<std::string, double> &userParams();
    static std::map<std::string, int> &userModes();
    static std::map<std::string, std::string> &userWords();
    
  private:
    
    static std::map<string, const ICreator*> &s_creators();
    
  };
  
  
}


#endif
