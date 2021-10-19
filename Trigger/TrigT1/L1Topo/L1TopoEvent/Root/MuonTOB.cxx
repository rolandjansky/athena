// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/MuonTOB.h"

thread_local TCS::Heap<TCS::MuonTOB> TCS::MuonTOB::fg_heap("Muon");

TCS::MuonTOB::MuonTOB(uint32_t roiWord, const std::string& tobName) :
  BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::MuonTOB::MuonTOB(unsigned int et, unsigned int isolation, int eta, unsigned int phi, uint32_t roiWord, const std::string& tobName) :
  BaseTOB( roiWord,tobName )
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
TCS::MuonTOB::createOnHeap(const MuonTOB& muon) {
   return fg_heap.create(muon);
}

void
TCS::MuonTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::MuonTOB::print(std::ostream &o) const {
    o << "muon energy: " << Et() << ", eta: " << eta() << ", phi: " << phi() << ", bw2or3: " << bw2or3() << ", innerCoin: " << innerCoin() << ", goodMF: " << goodMF() << ", charge: " << charge() << ", is2cand: " << is2cand() << ", isTGC: " << isTGC();
}
