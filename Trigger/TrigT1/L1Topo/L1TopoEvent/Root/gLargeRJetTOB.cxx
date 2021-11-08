//  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gLargeRJetTOB.h"

thread_local TCS::Heap<TCS::gLargeRJetTOB> TCS::gLargeRJetTOB::fg_heap("gLargeRJet");

// constructors
// default constructor
TCS::gLargeRJetTOB::gLargeRJetTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::gLargeRJetTOB::gLargeRJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble(Et/10.)
   , m_etaDouble(eta/40.)
   , m_phiDouble(phi/20.)
{}

// copy constructor
TCS::gLargeRJetTOB::gLargeRJetTOB(const gLargeRJetTOB & jet) = default;

TCS::gLargeRJetTOB::~gLargeRJetTOB() = default;

TCS::gLargeRJetTOB*
TCS::gLargeRJetTOB::createOnHeap(const gLargeRJetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::gLargeRJetTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::gLargeRJetTOB::print(std::ostream &o) const {
   o << "gLargeRJet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
