/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  DecisionAlg.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/15/12.
//

#ifndef __TopoCore__DecisionAlg__
#define __TopoCore__DecisionAlg__

#include "L1TopoCoreSimulation/ConfigurableAlg.h"
#include "L1TopoCoreSimulation/TOBArray.h"
#include "L1TopoCoreSimulation/StatusCode.h"
#include "L1TopoCoreSimulation/Decision.h"
#include "L1TopoCoreSimulation/CompositeTOB.h"

#include <iostream>

namespace TCS {

   class DecisionAlg : public ConfigurableAlg {
   public:
      // default constructor
      DecisionAlg(const  std::string & name) :
         ConfigurableAlg(name)
      {};
      virtual ~DecisionAlg(){};

      virtual bool isDecisionAlg() const { return true; }

      virtual TCS::StatusCode process(const std::vector<TCS::TOBArray> & input,
                                      TCS::TOBArray & output, Decision & decison) = 0;
    
      const Decision & decision() const { return m_decision; }
    
   private:

      virtual StatusCode doReset();

      friend class TopoSteering;
      Decision & decision() { return m_decision; }
      
      // the decision bit array
      Decision m_decision;
   };

} // end of namespace TCS

#endif /* defined(__TopoCore__DecisionAlg__) */
