// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/GenericTOB.h"

TCS::Heap<TCS::GenericTOB> TCS::GenericTOB::fg_heap("Generic",100);

// default constructor
TCS::GenericTOB::GenericTOB(uint32_t roiWord) :
   BaseTOB( roiWord )
{
   m_tobType = JET;
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
{}

// constructor from generic data
TCS::GenericTOB::GenericTOB(const GenericTOB & other) :
   BaseTOB(other)
   , m_Et(other.m_Et)
   , m_EtNarrow(other.EtNarrow())
   , m_EtWide(other.EtWide())
   , m_Ex(other.m_Ex)
   , m_Ey(other.m_Ey)
   , m_eta(other.m_eta)
   , m_phi(other.m_phi)
   , m_EtDouble(other.m_EtDouble)
   , m_etaDouble(other.m_etaDouble)
   , m_phiDouble(other.m_phiDouble)
   , m_tobType(other.m_tobType)
{}

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
{}

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
{}

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
{}

// constructor for latemuon
TCS::GenericTOB::GenericTOB(const LateMuonTOB & lateMuon) :
   BaseTOB(lateMuon)
   , m_Et(lateMuon.Et())
   , m_eta(lateMuon.eta())
   , m_phi(lateMuon.phi())
   , m_EtDouble((double)lateMuon.Et())
   , m_etaDouble(((double)lateMuon.eta())/10.)
   , m_phiDouble(((double)lateMuon.phi())/10.)
   , m_tobType(LATEMUON) 
{}

// constructor for muonNextBC
TCS::GenericTOB::GenericTOB(const MuonNextBCTOB & muonNextBC) :
   BaseTOB(muonNextBC)
   , m_Et(muonNextBC.Et())
   , m_eta(muonNextBC.eta())
   , m_phi(muonNextBC.phi())
   , m_EtDouble((double)muonNextBC.Et())
   , m_etaDouble(((double)muonNextBC.eta())/10.)
   , m_phiDouble(((double)muonNextBC.phi())/10.)
   , m_tobType(MUONNEXTBC) 
{}



// constr from met
TCS::GenericTOB::GenericTOB(const MetTOB & met) :
   BaseTOB(met)
   , m_Et(met.Et())
   , m_Ex(met.Ex())
   , m_Ey(met.Ey())
   , m_tobType(MET)
{}


// destructor
TCS::GenericTOB::~GenericTOB() = default;


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
