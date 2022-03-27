// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/cTauTOB.h"

thread_local TCS::Heap<TCS::cTauTOB> TCS::cTauTOB::fg_heap("cTau");

const unsigned int TCS::cTauTOB::g_nBitsEt = 13;
const unsigned int TCS::cTauTOB::g_nBitsEta = 9;
const unsigned int TCS::cTauTOB::g_nBitsPhi = 7;

// default constructor
TCS::cTauTOB::cTauTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::cTauTOB::cTauTOB(unsigned int et, int eta, unsigned int phi, inputTOBType_t tobType, uint32_t roiWord, const std::string& tobName) :
  BaseTOB( roiWord,tobName )
  , m_Et( et ) //sizeCheck(et, nBitsEt()) )
  , m_eta( eta ) //sizeCheck(eta, nBitsEta()) )
  , m_phi( phi ) //sizeCheck(phi, nBitsPhi()) )
  , m_tobType( tobType )
{}

// copy constructor
TCS::cTauTOB::cTauTOB(const TCS::cTauTOB & ctau) = default;

// destructor
TCS::cTauTOB::~cTauTOB() = default;


TCS::cTauTOB*
TCS::cTauTOB::createOnHeap(const cTauTOB& ctau) {
   return fg_heap.create(ctau);
}

void
TCS::cTauTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::cTauTOB::print(std::ostream &o) const {
    o << "cTau energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
