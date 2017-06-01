#ifndef GENERATOR_PYTHIA8_USER_PROCESS_FACTORY_H
#define GENERATOR_PYTHIA8_USER_PROCESS_FACTORY_H

#include "Pythia8/SigmaProcess.h"

#include <boost/smart_ptr.hpp>

#include <map>
#include <string>

namespace Pythia8_UserProcess{
  
  using Pythia8::Sigma2Process;
    
  using std::string;
  using std::map;
  
  class UserProcessFactory{
    
  public:
    
    static Sigma2Process* create(const string &procName);
    
  private:
    
    UserProcessFactory(){};
    
    class ICreator{
    public:
      virtual Sigma2Process *create() const = 0;
      virtual ~ICreator(){};
    };
    
  public:
    
    template <class T>
    class Creator: public ICreator{
      
    public:
      Creator(const string &name){
        m_name = name;
        UserProcessFactory::s_creators()[name] = this;
      }
      
      ~Creator(){
        if(s_creators()[m_name] == this){
          s_creators().erase(m_name);
        }
      }
      
      Sigma2Process *create()const{
        return new T;
      }
      
    private:
      
      string m_name;
      
    };
    
  private:
    static map<string, const ICreator*> &s_creators();
    
  };
}
#endif
