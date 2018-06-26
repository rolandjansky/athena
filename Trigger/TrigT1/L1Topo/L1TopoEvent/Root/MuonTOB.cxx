/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MuonTOB.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoEvent/MuonTOB.h"

unsigned int TCS::MuonTOB::fg_instances = 0;
TCS::Heap<TCS::MuonTOB> TCS::MuonTOB::fg_heap("Muon");

unsigned int TCS::MuonTOB::g_nBitsEt = 8;
unsigned int TCS::MuonTOB::g_nBitsIsolation = 5;
unsigned int TCS::MuonTOB::g_nBitsEta = 6;
unsigned int TCS::MuonTOB::g_nBitsPhi = 6;

// default constructor
TCS::MuonTOB::MuonTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et(0)
   , m_isolation(0)
   , m_eta(0)
   , m_phi(0)
{
   ++fg_instances;
}

// constructor with initial values
TCS::MuonTOB::MuonTOB(unsigned int et, unsigned int isolation, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{
   ++fg_instances;
}

// constructor with individual values
TCS::MuonTOB::MuonTOB(const TCS::MuonTOB & muon) : 
   BaseTOB( muon.roiWord() )
   , m_Et( muon.m_Et )
   , m_isolation( muon.m_isolation )
   , m_eta( muon.m_eta )
   , m_phi( muon.m_phi )
   , m_EtDouble( muon.m_Et )
   , m_etaDouble( muon.m_eta )
   , m_phiDouble( muon.m_phi )
{
   ++fg_instances;
}


// destructor
TCS::MuonTOB::~MuonTOB() {
   --fg_instances;
}


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
