/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlgSize1.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoAlgorithms/JetSortingAlgSize1.h"

REGISTER_ALG_TCS(JetSortingAlgSize1)

// constructor
TCS::JetSortingAlgSize1::JetSortingAlgSize1(const std::string & name) : JetSortingAlg(name)
{
  m_jetsize = JetTOB::JS1;
}

TCS::JetSortingAlgSize1::~JetSortingAlgSize1() {}
