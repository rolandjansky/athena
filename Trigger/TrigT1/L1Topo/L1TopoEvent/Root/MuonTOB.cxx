// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/MuonTOB.h"

thread_local TCS::Heap<TCS::MuonTOB> TCS::MuonTOB::fg_heap("Muon");

const unsigned int TCS::MuonTOB::g_nBitsEt = 8;
const unsigned int TCS::MuonTOB::g_nBitsIsolation = 5;
const unsigned int TCS::MuonTOB::g_nBitsEta = 6;
const unsigned int TCS::MuonTOB::g_nBitsPhi = 6;

TCS::MuonTOB::MuonTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{}

// constructor with initial values
TCS::MuonTOB::MuonTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{}

// copy constructor
TCS::MuonTOB::MuonTOB(const TCS::MuonTOB & muon) = default;

// destructor
TCS::MuonTOB::~MuonTOB() = default;


TCS::MuonTOB*
TCS::MuonTOB::createOnHeap(const MuonTOB& cl) {
   return fg_heap.create(cl);
}

void
TCS::MuonTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::MuonTOB::print(std::ostream &o) const {
    o << "cluster energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
