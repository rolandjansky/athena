//  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
{}

// copy constructor
TCS::jEmTOB::jEmTOB(const jEmTOB & jet) = default;

TCS::jEmTOB::~jEmTOB() = default;

TCS::jEmTOB*
TCS::jEmTOB::createOnHeap(const jEmTOB& jet) {
   return fg_heap.create(jet);
}

void
TCS::jEmTOB::clearHeap() {
   return fg_heap.clear();
}

void
TCS::jEmTOB::print(std::ostream &o) const {
   o << "jEm jet energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
