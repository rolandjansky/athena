//  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gLJetTOB.h"

thread_local TCS::Heap<TCS::gLJetTOB> TCS::gLJetTOB::fg_heap("gLJet");

// constructors
// default constructor
TCS::gLJetTOB::gLJetTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::gLJetTOB::gLJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble(Et/10.)
   , m_etaDouble(eta/40.)
   , m_phiDouble(phi/20.)
{}

// copy constructor
TCS::gLJetTOB::gLJetTOB(const gLJetTOB & jet) = default;

TCS::gLJetTOB::~gLJetTOB() = default;

TCS::gLJetTOB*
TCS::gLJetTOB::createOnHeap(const gLJetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::gLJetTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::gLJetTOB::print(std::ostream &o) const {
   o << "gLJet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
