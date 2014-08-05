// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_DecisionObjectHandle_h
#define TrigDecisionTool_DecisionObjectHandle_h

#include <string>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionEvent/TrigDecision.h"

namespace TrigDec {
  class TrigDecision;
}


/**
 * @brief collection of classes which care about invalidating TDT after event is finished
 **/

namespace Trig {
  class DecisionObjectHandle : public Logger{
  public: 
    DecisionObjectHandle();
    void invalidate();
    void validate();
    bool valid() const;
    virtual TrigDec::TrigDecision const * get() const = 0;
  protected:
    mutable TrigDec::TrigDecision const * m_object;
    bool m_validity;
  };

  /**
   * @brief Decision invalidator for ARA (fake)
   **/
  class DecisionObjectHandleARA : public DecisionObjectHandle {
  public:
    DecisionObjectHandleARA(TrigDec::TrigDecision const *);
    virtual TrigDec::TrigDecision const* get() const; 

  private:
  };
  
  
  /**
   * @brief Decision invalidator for Athena (realy dooing the job)
   **/
  
  class DecisionObjectHandleAthena : public DecisionObjectHandle, 
				     public DataHandle<TrigDec::TrigDecision>  {
  public:
    DecisionObjectHandleAthena( StoreGateSvc* sg, const std::string& key );
    virtual void reset();
    virtual TrigDec::TrigDecision const * get() const;
  private:
    StoreGateSvc* m_sg;
    const std::string m_key;
    
  };
  

}




#endif // TrigDecisionTool_DecisionObjectHandle_h
