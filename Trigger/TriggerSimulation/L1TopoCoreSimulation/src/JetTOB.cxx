/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  JetTOB.cpp
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include "L1TopoCoreSimulation/JetTOB.h"
#include "L1TopoCoreSimulation/GenericTOB.h"

unsigned int TCS::JetTOB::fg_instances = 0;
TCS::Heap<TCS::JetTOB> TCS::JetTOB::fg_heap;

// constructors
// default constructor
TCS::JetTOB::JetTOB() : BaseTOB()
   , m_Et1(0)
   , m_Et2(0)
   , m_eta(0)
   , m_phi(0)
{
   ++fg_instances;
}

// constructor with initial values
TCS::JetTOB::JetTOB(int Et1, int Et2, int eta, int phi) : BaseTOB()
   , m_Et1(Et1)
   , m_Et2(Et2)
   , m_eta(eta)
   , m_phi(phi)
{
   ++fg_instances;
}

// constructor with initial values
TCS::JetTOB::JetTOB(const JetTOB & jet) : BaseTOB()
   , m_Et1(jet.m_Et1)
   , m_Et2(jet.m_Et2)
   , m_eta(jet.m_eta)
   , m_phi(jet.m_phi)
{
   ++fg_instances;
}

TCS::JetTOB::~JetTOB() {
   --fg_instances;
}

TCS::JetTOB*
TCS::JetTOB::createOnHeap(const JetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::JetTOB::clearHeap() {
   return fg_heap.clear();
}


void
TCS::JetTOB::print(std::ostream &o) const {
   o << "jet energy wide window: " << Et1() << ", narrow window: " << Et2() << ", eta: " << eta() << ", phi: " << phi();
}
