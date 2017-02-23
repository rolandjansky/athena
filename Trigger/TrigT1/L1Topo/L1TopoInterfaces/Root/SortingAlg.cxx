//  SortingAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoInterfaces/SortingAlg.h"


TCS::StatusCode
TCS::SortingAlg::doReset() {
   return StatusCode::SUCCESS;
}

TCS::StatusCode TCS::SortingAlg::sortBitCorrect(const InputTOBArray & input, TOBArray & output)
{
  return sort(input, output);
}

TCS::SortingAlg::~SortingAlg() {}
