//
//  ClusterTOB.cpp
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/ClusterTOB.h"

unsigned int TCS::ClusterTOB::fg_instances = 0;
TCS::Heap<TCS::ClusterTOB> TCS::ClusterTOB::fg_heap;

unsigned int TCS::ClusterTOB::g_nBitsEt = 8;
unsigned int TCS::ClusterTOB::g_nBitsIsolation = 5;
unsigned int TCS::ClusterTOB::g_nBitsEta = 6;
unsigned int TCS::ClusterTOB::g_nBitsPhi = 6;

// default constructor
TCS::ClusterTOB::ClusterTOB() : BaseTOB()
   , m_Et(0)
   , m_isolation(0)
   , m_eta(0)
   , m_phi(0)
{
   ++fg_instances;
}

// constructor with initial values
TCS::ClusterTOB::ClusterTOB(int et, int isolation, int eta, int phi) : BaseTOB()
   , m_Et( sizeCheck(et, nBitsEt()) )
   , m_isolation( sizeCheck( isolation, nBitsIsolation()) )
   , m_eta( sizeCheck(eta, nBitsEta()) )
   , m_phi( sizeCheck(phi, nBitsPhi()) )
{
   ++fg_instances;
}

// constructor with individual values
TCS::ClusterTOB::ClusterTOB(const TCS::ClusterTOB & cluster) : BaseTOB()
   , m_Et( cluster.m_Et )
   , m_isolation( cluster.m_isolation )
   , m_eta( cluster.m_eta )
   , m_phi( cluster.m_phi )
{
   ++fg_instances;
}


// destructor
TCS::ClusterTOB::~ClusterTOB() {
   --fg_instances;
}


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
