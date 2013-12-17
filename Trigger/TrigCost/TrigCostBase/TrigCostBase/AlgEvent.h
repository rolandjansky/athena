/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_ALGEVENT_H
#define ANP_ALGEVENT_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : AlgEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Algorithm base class
 * 
 **********************************************************************************/

// Order of execution:
//
// 1) Config() - called once with Registry configuration from macro
//
// 2) Save()   - called once after config if ouput ROOT is created
//
// 3) Init()   - called after Config() and Save() but before first Exec()
//
// 4) Exec()   - called for every TTree entry read from input files
//
// 5) Done()   - called once after all TTrees are read or maximum number of events is processed
//
// 6) Signal() - sends signals to algorithms
//

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/UtilCore.h"

class TDirectory;

namespace Anp
{
  class Callback {
  public:   
    Callback() {}
    virtual ~Callback() {}
    
    virtual void Signal(const Anp::Registry &, const std::string &) {}

    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetType() const = 0;
  };

  class AlgEvent: 
    public virtual Base, 
    public virtual Callback {
  public:   

    AlgEvent() : fPass(false) {}
    virtual ~AlgEvent() {}

    virtual void Config(const Anp::Registry &) {}

    virtual void Save(TDirectory *) {}

    virtual bool Init() = 0;

    virtual void Exec() = 0;
    
    virtual void Done() {}

    virtual std::ostream& log() const;


    virtual const std::string& GetName() const { return fName; }
    virtual const std::string& GetType() const { return fType; }

    virtual void SetName(const std::string &val) { fName = val; }
    virtual void SetType(const std::string &val) { fType = val; }
    
    bool IsPassedEvent() const { return fPass; }

    void FailEvent() { fPass = false; }
    void PassEvent() { fPass = true; }

  private:

    std::string fName;
    std::string fType;

    bool        fPass; // pass current event?
  };
  
  //
  // Special method to register callbacks functions
  //
  template<class T> inline bool RegisterCallback(const Registry &reg,
						 const std::string& key, 
						 Handle<T> &val, 
						 Callback *caller)
    {
      //
      // Attempt to retrieve and register algorithm for callback
      //
      
      std::string alg_name;
      if(key.find("::") != std::string::npos) {
	//
	// Split key using 
	//
	std::vector<std::string> keys;
	Anp::StringTok(keys, key, "::");

	if(keys.size() == 2) {
	  reg.Get(keys.at(0), keys.at(1), alg_name);
	}
	else {
	  return false;
	}
      }
      else {
	reg.Get(key, alg_name);
      }
      
      if(alg_name.empty()) {
	return false;
      }
      
      val = Factory<T>::Instance().Get(alg_name);
      
      if(val.valid()) {
	val->RegisterCallback(caller);
	return true;
      }
      
      return false;
    }
  
  //
  // Simple inline implementation of log function
  //
  inline std::ostream& AlgEvent::log() const {
    std::cout << GetName() << "::";
    return std::cout;
  }
}

#endif
