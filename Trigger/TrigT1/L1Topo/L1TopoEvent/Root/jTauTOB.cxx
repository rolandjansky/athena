//  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jTauTOB.h"

thread_local TCS::Heap<TCS::jTauTOB> TCS::jTauTOB::fg_heap("jTau");

// constructors
// default constructor
TCS::jTauTOB::jTauTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::jTauTOB::jTauTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
{}

// copy constructor
TCS::jTauTOB::jTauTOB(const jTauTOB & jtau) = default;

TCS::jTauTOB::~jTauTOB() = default;

TCS::jTauTOB*
TCS::jTauTOB::createOnHeap(const jTauTOB& jtau) {
   return fg_heap.create(jtau);
}

void
TCS::jTauTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::jTauTOB::print(std::ostream &o) const {
   o << "jTau energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
