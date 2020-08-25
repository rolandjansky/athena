/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  LateMuonTOB.cpp
//  TopoCore
//  Based on MuonTOB created by Joerg Stelzer on 11/10/12.

#include "L1TopoEvent/LateMuonTOB.h"

TCS::Heap<TCS::LateMuonTOB> TCS::LateMuonTOB::fg_heap("LateMuon");

const unsigned int TCS::LateMuonTOB::g_nBitsEt = 8;
const unsigned int TCS::LateMuonTOB::g_nBitsIsolation = 5;
const unsigned int TCS::LateMuonTOB::g_nBitsEta = 6;
const unsigned int TCS::LateMuonTOB::g_nBitsPhi = 6;

// default constructor
TCS::LateMuonTOB::LateMuonTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{}

// constructor with initial values
TCS::LateMuonTOB::LateMuonTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{}

// copy constructor
TCS::LateMuonTOB::LateMuonTOB(const TCS::LateMuonTOB & muon) :
   BaseTOB( muon.roiWord() )
   , m_Et( muon.m_Et )
   , m_isolation( muon.m_isolation )
   , m_eta( muon.m_eta )
   , m_phi( muon.m_phi )
   , m_EtDouble( muon.m_Et )
   , m_etaDouble( muon.m_eta )
   , m_phiDouble( muon.m_phi )
{}

TCS::LateMuonTOB::~LateMuonTOB() = default;

TCS::LateMuonTOB*
TCS::LateMuonTOB::createOnHeap(const LateMuonTOB& cl)
{
   return fg_heap.create(cl);
}

void
TCS::LateMuonTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::LateMuonTOB::print(std::ostream &o) const {
    o << "cluster energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
