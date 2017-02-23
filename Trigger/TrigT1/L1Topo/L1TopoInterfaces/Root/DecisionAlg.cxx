//  DecisionAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/15/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoInterfaces/DecisionAlg.h"

TCS::StatusCode
TCS::DecisionAlg::doReset() {
   return StatusCode::SUCCESS;
}

TCS::DecisionAlg::~DecisionAlg() {}

TCS::StatusCode
TCS::DecisionAlg::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                          const std::vector<TCS::TOBArray *> & output,
                                          Decision & decision )
{
   return process(input,output,decision);
}

