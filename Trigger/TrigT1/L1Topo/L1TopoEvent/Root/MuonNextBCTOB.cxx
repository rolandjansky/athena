// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/MuonNextBCTOB.h"

thread_local TCS::Heap<TCS::MuonNextBCTOB> TCS::MuonNextBCTOB::fg_heap("MuonNextBC");

const unsigned int TCS::MuonNextBCTOB::g_nBitsEt = 8;
const unsigned int TCS::MuonNextBCTOB::g_nBitsIsolation = 5;
const unsigned int TCS::MuonNextBCTOB::g_nBitsEta = 6;
const unsigned int TCS::MuonNextBCTOB::g_nBitsPhi = 6;

// default constructor
TCS::MuonNextBCTOB::MuonNextBCTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{}

// constructor with initial values
TCS::MuonNextBCTOB::MuonNextBCTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{}

// copy constructor
TCS::MuonNextBCTOB::MuonNextBCTOB(const TCS::MuonNextBCTOB & muon) = default;

// destructor
TCS::MuonNextBCTOB::~MuonNextBCTOB() = default;

TCS::MuonNextBCTOB*
TCS::MuonNextBCTOB::createOnHeap(const MuonNextBCTOB& cl) {
   return fg_heap.create(cl);
}

void
TCS::MuonNextBCTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::MuonNextBCTOB::print(std::ostream &o) const {
    o << "cluster energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
