/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  DecisionAlg.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/15/12.
//

#include "L1TopoCoreSimulation/DecisionAlg.h"

TCS::StatusCode
TCS::DecisionAlg::doReset() {
   return m_decision.reset(); // set decision to 0
}
