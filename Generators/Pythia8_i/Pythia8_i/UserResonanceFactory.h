#ifndef GENERATOR_PYTHIA8_USER_RESONANCE_FACTORY_H
#define GENERATOR_PYTHIA8_USER_RESONANCE_FACTORY_H

#include "Pythia8/ResonanceWidths.h"

#include <string>

namespace Pythia8_UserResonance{
 
  using Pythia8::ResonanceWidths;
  using std::string;
  using std::map;
  
  class UserResonanceFactory{
    
  public:
    
    /**
     *  Call this with the name of the ResonanceWidth and PDG ID to which it will be applied
     *  e.g. create("MyResonance", 23) will return a MyResonance instance that will be applied to the Z
     *
     */
    static ResonanceWidths* create(const string &name, int pdgid);
    
  private:
    
    UserResonanceFactory(){};
    
    class ICreator{
    public:
      virtual ResonanceWidths *create(int idResIn)const = 0;
      virtual ~ICreator(){};
    };
    
  public:
    
    template <class T>
    class Creator: public ICreator{
      
    public:
      Creator(const string &name){
        m_name = name;
        UserResonanceFactory::s_creators()[name] = this;
      }
      
      ~Creator(){
        if(s_creators()[m_name] == this){
          s_creators().erase(m_name);
        }
      }
      
      ResonanceWidths *create(int idResIn)const{
        return new T(idResIn);
      }
      
    private:
      
      string m_name;
    };
    
  private:
    
    static map<string, const ICreator*> &s_creators();
    
  };
}

#endif
