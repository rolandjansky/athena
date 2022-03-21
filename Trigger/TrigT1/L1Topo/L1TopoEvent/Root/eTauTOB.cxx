// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/eTauTOB.h"

thread_local TCS::Heap<TCS::eTauTOB> TCS::eTauTOB::fg_heap("eTau");

const unsigned int TCS::eTauTOB::g_nBitsEt = 13;
const unsigned int TCS::eTauTOB::g_nBitsEta = 9;
const unsigned int TCS::eTauTOB::g_nBitsPhi = 7;

// default constructor
TCS::eTauTOB::eTauTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::eTauTOB::eTauTOB(unsigned int et, int eta, unsigned int phi, inputTOBType_t tobType, uint32_t roiWord, const std::string& tobName) :
  BaseTOB( roiWord,tobName )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
   , m_tobType( tobType )
{}

// copy constructor
TCS::eTauTOB::eTauTOB(const TCS::eTauTOB & eem) = default;

// destructor
TCS::eTauTOB::~eTauTOB() = default;


TCS::eTauTOB*
TCS::eTauTOB::createOnHeap(const eTauTOB& eem) {
   return fg_heap.create(eem);
}

void
TCS::eTauTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::eTauTOB::print(std::ostream &o) const {
  o << "eTau energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
