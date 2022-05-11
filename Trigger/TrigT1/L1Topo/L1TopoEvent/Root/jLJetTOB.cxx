//  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jLJetTOB.h"

thread_local TCS::Heap<TCS::jLJetTOB> TCS::jLJetTOB::fg_heap("Jet");

// constructors
// default constructor
TCS::jLJetTOB::jLJetTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::jLJetTOB::jLJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
{}

// copy constructor
TCS::jLJetTOB::jLJetTOB(const jLJetTOB & jet) = default;

TCS::jLJetTOB::~jLJetTOB() = default;

TCS::jLJetTOB*
TCS::jLJetTOB::createOnHeap(const jLJetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::jLJetTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::jLJetTOB::print(std::ostream &o) const {
   o << "jLargeJet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
