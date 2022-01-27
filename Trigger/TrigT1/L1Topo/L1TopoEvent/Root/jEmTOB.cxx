//  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jEmTOB.h"

thread_local TCS::Heap<TCS::jEmTOB> TCS::jEmTOB::fg_heap("jEm");

// constructors
// default constructor
TCS::jEmTOB::jEmTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::jEmTOB::jEmTOB(unsigned int Et, int eta, unsigned phi, uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble(Et/10.)
   , m_etaDouble(eta/40.)
   , m_phiDouble(phi/20.)
{}

// copy constructor
TCS::jEmTOB::jEmTOB(const jEmTOB & cluster) = default;

TCS::jEmTOB::~jEmTOB() = default;

TCS::jEmTOB*
TCS::jEmTOB::createOnHeap(const jEmTOB& cluster) {
   return fg_heap.create(cluster);
}

void
TCS::jEmTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::jEmTOB::print(std::ostream &o) const {
   o << "eEm cluster energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
