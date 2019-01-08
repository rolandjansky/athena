/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  SortingAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#include "L1TopoInterfaces/SortingAlg.h"


TCS::StatusCode
TCS::SortingAlg::doReset() {
   m_overflow = false;
   return StatusCode::SUCCESS;
}

TCS::StatusCode TCS::SortingAlg::sortBitCorrect(const InputTOBArray & input, TOBArray & output)
{
  return sort(input, output);
}

TCS::SortingAlg::~SortingAlg() {}
