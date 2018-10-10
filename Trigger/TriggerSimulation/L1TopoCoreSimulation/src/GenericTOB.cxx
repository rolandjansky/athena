/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  GeneralTOP.cpp
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include "L1TopoCoreSimulation/GenericTOB.h"

unsigned int TCS::GenericTOB::fg_instances = 0;
TCS::Heap<TCS::GenericTOB> TCS::GenericTOB::fg_heap;

// default constructor
TCS::GenericTOB::GenericTOB() : BaseTOB()
   , m_Et(0)
   , m_eta(0)
   , m_phi(0)
{
   ++fg_instances;
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(int Et, int eta, int phi) : BaseTOB()
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
{
   ++fg_instances;
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(const GenericTOB & other) : BaseTOB(other)
   , m_Et(other.m_Et)
   , m_eta(other.m_eta)
   , m_phi(other.m_phi)
{
   ++fg_instances;
}

// constructor from jet
TCS::GenericTOB::GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize) : BaseTOB(jet)
   , m_Et(jet.Et(jetSize))
   , m_eta(jet.eta())
   , m_phi(jet.phi())
{
   ++fg_instances;
}



// constructor from cluster
TCS::GenericTOB::GenericTOB(const ClusterTOB & cluster) : BaseTOB(cluster)
   , m_Et(cluster.Et())
   , m_eta(cluster.eta())
   , m_phi(cluster.phi())
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


