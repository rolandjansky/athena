/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H
#define GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H

#include "Pythia8/UserHooks.h"

#include <string>
#include <map>

namespace Pythia8_UserHooks{
 
  using Pythia8::UserHooks;
  
  
  class UserHooksFactory{
    
  public:
    
    static UserHooks* create(const std::string &hookName);
    
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
      
      Creator(const std::string &name){
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
      
      std::string m_name;
    };
    
    template<typename T>
    static std::map<std::string, T> &userSettings();
    
    static std::map<std::string, double> &userParams();
    static std::map<std::string, int> &userModes();
    static std::map<std::string, std::string> &userWords();
    
  private:
    
    static std::map<std::string, const ICreator*> &s_creators();
    
  };
  
  
}


#endif
