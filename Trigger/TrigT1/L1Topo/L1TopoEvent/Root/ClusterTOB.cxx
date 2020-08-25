// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/ClusterTOB.h"

TCS::Heap<TCS::ClusterTOB> TCS::ClusterTOB::fg_heap("Cluster");

const unsigned int TCS::ClusterTOB::g_nBitsEt = 10;
const unsigned int TCS::ClusterTOB::g_nBitsIsolation = 5;
const unsigned int TCS::ClusterTOB::g_nBitsEta = 6;
const unsigned int TCS::ClusterTOB::g_nBitsPhi = 7;

// default constructor
TCS::ClusterTOB::ClusterTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{}

// constructor with initial values
TCS::ClusterTOB::ClusterTOB(unsigned int et, unsigned int isolation, int eta, int phi, inputTOBType_t tobType, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
   , m_tobType( tobType )
{}

// constructor with individual values
TCS::ClusterTOB::ClusterTOB(const TCS::ClusterTOB & cluster) :
   BaseTOB( cluster.roiWord() )
   , m_Et( cluster.m_Et )
   , m_isolation( cluster.m_isolation )
   , m_eta( cluster.m_eta )
   , m_phi( cluster.m_phi )
   , m_EtDouble( cluster.m_EtDouble )
   , m_etaDouble( cluster.m_etaDouble )
   , m_phiDouble( cluster.m_phiDouble )
   , m_tobType( cluster.m_tobType )
{}


// destructor
TCS::ClusterTOB::~ClusterTOB() = default;


TCS::ClusterTOB*
TCS::ClusterTOB::createOnHeap(const ClusterTOB& cl) {
   return fg_heap.create(cl);
}

void
TCS::ClusterTOB::clearHeap() {
   return fg_heap.clear();
}

void TCS::ClusterTOB::print(std::ostream &o) const {
    o << "cluster energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
