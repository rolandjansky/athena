//  JetSortingAlgSize1.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/JetSortingAlgSize1.h"

REGISTER_ALG_TCS(JetSortingAlgSize1)

// constructor
TCS::JetSortingAlgSize1::JetSortingAlgSize1(const std::string & name) : JetSortingAlg(name)
{
  m_jetsize = JetTOB::JS1;
}

TCS::JetSortingAlgSize1::~JetSortingAlgSize1() {}
