#ifndef PYTHIA8_USERHOOKS_USERSETTING_H
#define PYTHIA8_USERHOOKS_USERSETTING_H

#include "Pythia8/Settings.h"
#include <string>
#include <iostream>

namespace Pythia8_UserHooks{
 
  using std::string;
  
  template <class T>
  class UserSetting{
    
    public:
    
    UserSetting(string name, T defaultValue):
    m_paramName(name),
    m_param(defaultValue),
    m_settingsPtr(0),
    m_retrieved(false){  }
    
    T operator()(Pythia8::Settings *settingsPtr){
    
      if(m_retrieved && m_settingsPtr == settingsPtr) return m_param;
      
      m_settingsPtr = settingsPtr;
      m_param = uncachedRetrieve();
      std::cout<<m_paramName<<" = "<<m_param<<std::endl;
      m_retrieved = true;
      return m_param;
    };
    
    protected:
    
    string m_paramName;
    T m_param;
    Pythia8::Settings *m_settingsPtr;
    bool m_retrieved;
    
    private:
    
    T uncachedRetrieve();
    
  };
  
  template<>
  inline double UserSetting<double>::uncachedRetrieve(){
    if(m_settingsPtr->isParm(m_paramName)){
      return m_settingsPtr->parm(m_paramName);
    }
    return m_param;
  }
  
  template<>
  inline int UserSetting<int>::uncachedRetrieve(){
    if(m_settingsPtr->isMode(m_paramName)){
      return m_settingsPtr->mode(m_paramName);
    }
    return m_param;
  }
  
  template<>
  inline string UserSetting<string>::uncachedRetrieve(){
    if(m_settingsPtr->isWord(m_paramName)){
      return m_settingsPtr->word(m_paramName);
    }
    return m_param;
  }
  
}
#endif
