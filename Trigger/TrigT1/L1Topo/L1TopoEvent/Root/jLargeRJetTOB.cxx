//  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jLargeRJetTOB.h"

thread_local TCS::Heap<TCS::jLargeRJetTOB> TCS::jLargeRJetTOB::fg_heap("Jet");

// constructors
// default constructor
TCS::jLargeRJetTOB::jLargeRJetTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::jLargeRJetTOB::jLargeRJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble(Et/10.)
   , m_etaDouble(eta/40.)
   , m_phiDouble(phi/20.)
{}

// copy constructor
TCS::jLargeRJetTOB::jLargeRJetTOB(const jLargeRJetTOB & jet) = default;

TCS::jLargeRJetTOB::~jLargeRJetTOB() = default;

TCS::jLargeRJetTOB*
TCS::jLargeRJetTOB::createOnHeap(const jLargeRJetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::jLargeRJetTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::jLargeRJetTOB::print(std::ostream &o) const {
   o << "jLargeJet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
