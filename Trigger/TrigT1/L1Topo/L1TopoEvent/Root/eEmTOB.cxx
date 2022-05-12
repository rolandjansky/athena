// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/eEmTOB.h"

thread_local TCS::Heap<TCS::eEmTOB> TCS::eEmTOB::fg_heap("eEm");

const unsigned int TCS::eEmTOB::g_nBitsEt = 13;
const unsigned int TCS::eEmTOB::g_nBitsEta = 9;
const unsigned int TCS::eEmTOB::g_nBitsPhi = 7;

// default constructor
TCS::eEmTOB::eEmTOB(uint32_t roiWord, const std::string& tobName) :
   BaseTOB( roiWord,tobName )
{}

// constructor with initial values
TCS::eEmTOB::eEmTOB(unsigned int et, int eta, unsigned int phi, inputTOBType_t tobType, uint32_t roiWord, const std::string& tobName) :
  BaseTOB( roiWord,tobName )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
   , m_tobType( tobType )
{}

// copy constructor
TCS::eEmTOB::eEmTOB(const TCS::eEmTOB & eem) = default;

// destructor
TCS::eEmTOB::~eEmTOB() = default;


TCS::eEmTOB*
TCS::eEmTOB::createOnHeap(const eEmTOB& eem) {
   return fg_heap.create(eem);
}

void
TCS::eEmTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::eEmTOB::print(std::ostream &o) const {
    o << "eEm energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
