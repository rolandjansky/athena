/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetSortingAlgSize1.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoAlgorithms/JetSortingAlgSize2.h"

REGISTER_ALG_TCS(JetSortingAlgSize2)

// constructor
  TCS::JetSortingAlgSize2::JetSortingAlgSize2(const std::string & name) : JetSortingAlg(name)
{
  m_jetsize = JetTOB::JS2;
}

TCS::JetSortingAlgSize2::~JetSortingAlgSize2() {}
