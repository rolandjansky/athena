//  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gJetTOB.h"

thread_local TCS::Heap<TCS::gJetTOB> TCS::gJetTOB::fg_heap("gJet");

// constructors
// default constructor
TCS::gJetTOB::gJetTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::gJetTOB::gJetTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble(Et/10.)
   , m_etaDouble(eta/40.)
   , m_phiDouble(phi/20.)
{}

// copy constructor
TCS::gJetTOB::gJetTOB(const gJetTOB & jet) = default;

TCS::gJetTOB::~gJetTOB() = default;

TCS::gJetTOB*
TCS::gJetTOB::createOnHeap(const gJetTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::gJetTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::gJetTOB::print(std::ostream &o) const {
   o << "gJet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
