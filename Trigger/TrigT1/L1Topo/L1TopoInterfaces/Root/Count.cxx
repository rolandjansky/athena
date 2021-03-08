/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  Count.cxx
//  TopoCore
//  Created by Carlos Moreno on 05/05/20.

#include "L1TopoInterfaces/Count.h"
#include "L1TopoCommon/Exception.h"

void 
TCS::Count::setSizeCount(unsigned int sizeCount){

  m_sizeCount = sizeCount;

  // Compute the final bits that make up the output of the multiplicity algorithm

  unsigned int maximum = (1 << m_nBits) - 1; // saturation limit given by the number of bits allowed for the trigger line

  int mask = (sizeCount < maximum)?sizeCount:maximum;

  m_count = (mask << m_firstBit);

}