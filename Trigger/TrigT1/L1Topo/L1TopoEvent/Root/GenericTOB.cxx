//  GeneralTOP.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/GenericTOB.h"
#include "L1TopoEvent/JetTOB.h" // added by cw

unsigned int TCS::GenericTOB::fg_instances = 0;
TCS::Heap<TCS::GenericTOB> TCS::GenericTOB::fg_heap;

// default constructor
TCS::GenericTOB::GenericTOB() : BaseTOB()
{
   ++fg_instances;
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(int Et, int eta, int phi) : 
   BaseTOB()
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
{
   ++fg_instances;
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(const GenericTOB & other) :
   BaseTOB(other)
   , m_Et(other.m_Et)
   , m_eta(other.m_eta)
   , m_phi(other.m_phi)
   , m_EtDouble(other.m_EtDouble)
   , m_etaDouble(other.m_etaDouble)
   , m_phiDouble(other.m_phiDouble)
{
   ++fg_instances;
}

// constructor from jet
TCS::GenericTOB::GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize) : BaseTOB(jet)
   , m_Et(jet.Et(jetSize))
   , m_eta(jet.eta())
   , m_phi(jet.phi())
   , m_EtDouble(jet.EtDouble(jetSize))
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
{
   ++fg_instances;
}

// constructor from cluster
TCS::GenericTOB::GenericTOB(const ClusterTOB & cluster) : BaseTOB(cluster)
   , m_Et(cluster.Et())
   , m_eta(cluster.eta())
   , m_phi(cluster.phi())
   , m_EtDouble(cluster.EtDouble())
   , m_etaDouble(cluster.etaDouble())
   , m_phiDouble(cluster.phiDouble())
{
   ++fg_instances;
}

// destructor
TCS::GenericTOB::~GenericTOB() {
   --fg_instances;
}


TCS::GenericTOB*
TCS::GenericTOB::createOnHeap(const GenericTOB& tob) {
   return fg_heap.create(tob);
}


void
TCS::GenericTOB::clearHeap() {
   return fg_heap.clear();
}


void TCS::GenericTOB::print(std::ostream &o) const {
   o << "generic tob energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
