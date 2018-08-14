//
//  DecisionAlg.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/15/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/DecisionAlg.h"

TCS::StatusCode
TCS::DecisionAlg::doReset() {
   return m_decision.reset(); // set decision to 0
}
