//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/JetTOB.h"

TCS::Heap<TCS::JetTOB> TCS::JetTOB::fg_heap("Jet");

// constructors
// default constructor
TCS::JetTOB::JetTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{}

// constructor with initial values
TCS::JetTOB::JetTOB(unsigned int Et1, unsigned int Et2, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et1(Et1)
   , m_Et2(Et2)
   , m_eta(eta)
   , m_phi(phi)
   , m_Et1Double(Et1)
   , m_Et2Double(Et2)
   , m_etaDouble(eta/10.)
   , m_phiDouble(phi/10.)
{}

// constructor with initial values
TCS::JetTOB::JetTOB(const JetTOB & jet) :
   BaseTOB( jet.roiWord() )
   , m_Et1(jet.m_Et1)
   , m_Et2(jet.m_Et2)
   , m_eta(jet.m_eta)
   , m_phi(jet.m_phi)
   , m_Et1Double(jet.m_Et1Double)
   , m_Et2Double(jet.m_Et2Double)
   , m_etaDouble(jet.m_etaDouble)
   , m_phiDouble(jet.m_phiDouble)
{}

TCS::JetTOB::~JetTOB() = default;

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
