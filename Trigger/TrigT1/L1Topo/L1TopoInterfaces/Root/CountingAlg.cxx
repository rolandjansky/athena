/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  CountingAlg.cxx
//  TopoCore
//  Created by Carlos Moreno on 05/05/20.

#include "L1TopoInterfaces/CountingAlg.h"

TCS::StatusCode
TCS::CountingAlg::doReset() {
   return StatusCode::SUCCESS;
}

TCS::CountingAlg::~CountingAlg() {}

TCS::StatusCode
TCS::CountingAlg::processBitCorrect( const TCS::InputTOBArray & input,
				     Count & count )
{
  return process(input, count);
}
