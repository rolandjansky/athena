// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_DecisionObjectHandle_h
#define TrigDecisionTool_DecisionObjectHandle_h

#include <string>

#include "TrigDecisionTool/Logger.h"

namespace TrigDec {
  class TrigDecision;

#if XAOD_ANALYSIS
  class TrigDecision{
  };
#endif
}

/**
 * @brief collection of classes which care about invalidating TDT after event is finished
 **/

namespace Trig {
  template<typename DECISIONTYPE, typename NAVIGATIONTYPE>
  class DecisionObjectHandle : public Logger {
  public: 
    DecisionObjectHandle();
    virtual ~DecisionObjectHandle(){;}
    void invalidate();
    void validate();
    bool valid() const;
    virtual DECISIONTYPE const * getDecision() const = 0;
    virtual NAVIGATIONTYPE const * getNavigation() const = 0;
  protected:
    bool m_validity;
  };

  template<typename DECISIONTYPE, typename NAVIGATIONTYPE>
  DecisionObjectHandle<DECISIONTYPE,NAVIGATIONTYPE>::DecisionObjectHandle()
    : m_validity(false) {  
  }

  template<typename DECISIONTYPE, typename NAVIGATIONTYPE>
  void DecisionObjectHandle<DECISIONTYPE,NAVIGATIONTYPE>::invalidate() {
    m_validity = false;
  }

  template<typename DECISIONTYPE, typename NAVIGATIONTYPE>
  void DecisionObjectHandle<DECISIONTYPE,NAVIGATIONTYPE>::validate() {
    m_validity = true;
  }

  template<typename DECISIONTYPE, typename NAVIGATIONTYPE>
  bool DecisionObjectHandle<DECISIONTYPE,NAVIGATIONTYPE>::valid() const {
    return m_validity;
  }
}

#endif // TrigDecisionTool_DecisionObjectHandle_h
