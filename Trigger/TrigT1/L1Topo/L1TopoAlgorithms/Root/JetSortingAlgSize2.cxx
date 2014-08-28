//  JetSortingAlgSize1.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/JetSortingAlgSize2.h"

REGISTER_ALG_TCS(JetSortingAlgSize2)

// constructor
  TCS::JetSortingAlgSize2::JetSortingAlgSize2(const std::string & name) : JetSortingAlg(name)
{
  m_jetsize = JetTOB::JS2;
}

TCS::JetSortingAlgSize2::~JetSortingAlgSize2() {}
