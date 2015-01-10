//  GeneralTOP.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/GenericTOB.h"

unsigned int TCS::GenericTOB::fg_instances = 0;
TCS::Heap<TCS::GenericTOB> TCS::GenericTOB::fg_heap("Generic",100);

// default constructor
TCS::GenericTOB::GenericTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{
   m_tobType = JET;
   ++fg_instances;
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(unsigned int Et, int eta, int phi, uint32_t roiWord) :
   BaseTOB( roiWord )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble((double)Et)
   , m_etaDouble(((double)eta)/10.)
   , m_phiDouble(((double)phi)/10.)
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
   , m_tobType(other.m_tobType)
{
   ++fg_instances;
}

// constructor from jet
TCS::GenericTOB::GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize) :
   BaseTOB(jet)
   , m_Et(jet.Et(jetSize))
   , m_EtNarrow(jet.EtNarrow())
   , m_EtWide(jet.EtWide())
   , m_eta(jet.eta())
   , m_phi(jet.phi())
   , m_EtDouble(jet.EtDouble(jetSize))
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JET)
{
   ++fg_instances;
}

// constructor from cluster
TCS::GenericTOB::GenericTOB(const ClusterTOB & cluster) :
   BaseTOB(cluster)
   , m_Et(cluster.Et())
   , m_eta(cluster.eta())
   , m_phi(cluster.phi())
   , m_EtDouble(cluster.EtDouble())
   , m_etaDouble(cluster.etaDouble())
   , m_phiDouble(cluster.phiDouble())
   , m_tobType(cluster.tobType())
{
   ++fg_instances;
}

// constructor from muon
TCS::GenericTOB::GenericTOB(const MuonTOB & muon) :
   BaseTOB(muon)
   , m_Et(muon.Et())
   , m_eta(muon.eta())
   , m_phi(muon.phi())
   , m_EtDouble((double)muon.Et())
   , m_etaDouble(((double)muon.eta())/10.)
   , m_phiDouble(((double)muon.phi())/10.)
   , m_tobType(MUON)
{
    ++fg_instances;
}

// destructor
TCS::GenericTOB::~GenericTOB() {
   --fg_instances;
}


TCS::GenericTOB*
TCS::GenericTOB::createOnHeap(const GenericTOB& tob) {
   //std::cout << "Creating GenericTOB #instance before creation " << fg_instances << ", heap size " << fg_heap.size() << ", heap capacity " << fg_heap.capacity() << std::endl;
   return fg_heap.create(tob);
}


void
TCS::GenericTOB::clearHeap() {
   return fg_heap.clear();
}


void TCS::GenericTOB::print(std::ostream &o) const {
   o << "generic tob energy: " << Et() << ", eta: " << eta() << ", phi: " << phi();
}
