// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/MuonNextBCTOB.h"

TCS::Heap<TCS::MuonNextBCTOB> TCS::MuonNextBCTOB::fg_heap("MuonNextBC");

const unsigned int TCS::MuonNextBCTOB::g_nBitsEt = 8;
const unsigned int TCS::MuonNextBCTOB::g_nBitsIsolation = 5;
const unsigned int TCS::MuonNextBCTOB::g_nBitsEta = 6;
const unsigned int TCS::MuonNextBCTOB::g_nBitsPhi = 6;

// default constructor
TCS::MuonNextBCTOB::MuonNextBCTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et(0)
   , m_isolation(0)
   , m_eta(0)
   , m_phi(0)
{}

// constructor with initial values
TCS::MuonNextBCTOB::MuonNextBCTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{}

// constructor with individual values
TCS::MuonNextBCTOB::MuonNextBCTOB(const TCS::MuonNextBCTOB & muon) : 
   BaseTOB( muon.roiWord() )
   , m_Et( muon.m_Et )
   , m_isolation( muon.m_isolation )
   , m_eta( muon.m_eta )
   , m_phi( muon.m_phi )
   , m_EtDouble( muon.m_Et )
   , m_etaDouble( muon.m_eta )
   , m_phiDouble( muon.m_phi )
{}


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
