#ifndef GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H
#define GENERATOR_PYTHIA8_USER_HOOKS_FACTORY_H

#include "Pythia8/UserHooks.h"

#include <string>

namespace Pythia8_UserHooks{
 
  using Pythia8::UserHooks;
  using std::string;
  
  
  class UserHooksFactory{
    
  public:
    
    static UserHooks* create(const string &hookName);
    
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
    
  private:
    
    static std::map<string, const ICreator*> &s_creators();
    
  };
}


#endif
