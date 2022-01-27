// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/GenericTOB.h"

thread_local TCS::Heap<TCS::GenericTOB> TCS::GenericTOB::fg_heap("Generic",100);

// default constructor
TCS::GenericTOB::GenericTOB(uint32_t roiWord) :
  BaseTOB( roiWord,"GenericTOB" ),
  m_tobType (JET)
{
}

// constructor from generic data
TCS::GenericTOB::GenericTOB(unsigned int Et, int eta, int phi, uint32_t roiWord) :
  BaseTOB( roiWord,"GenericTOB" )
   , m_Et(Et)
   , m_eta(eta)
   , m_phi(phi)
   , m_EtDouble((double)Et)
   , m_etaDouble(((double)eta)/10.)
   , m_phiDouble(((double)phi)/10.)
{}

// copy constructor
TCS::GenericTOB::GenericTOB(const GenericTOB & other) = default;

// constructor from jet
TCS::GenericTOB::GenericTOB(const JetTOB & jet, JetTOB::JetSize jetSize) :
   BaseTOB(jet.roiWord(), jet.tobName())
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

// constructor from jtau
TCS::GenericTOB::GenericTOB(const jTauTOB & tau) :
   BaseTOB(tau.roiWord(), tau.tobName())
   , m_Et(tau.Et())
   , m_eta(tau.eta())
   , m_phi(static_cast<int>(tau.phi()))
   , m_EtDouble(tau.EtDouble())
   , m_etaDouble(tau.etaDouble())
   , m_phiDouble(tau.phiDouble())
   , m_jtauiso(tau.isolation())
   , m_tobType(JET)
{}

// constructor from jEm
TCS::GenericTOB::GenericTOB(const jEmTOB & jem) :
   BaseTOB(jem.roiWord(), jem.tobName())
   , m_Et(jem.Et())
   , m_eta(jem.eta())
   , m_phi(static_cast<int>(jem.phi()))
   , m_EtDouble(jem.EtDouble())
   , m_etaDouble(jem.etaDouble())
   , m_phiDouble(jem.phiDouble())
   , m_tobType(JET)
{}

// constructor from large R jet
TCS::GenericTOB::GenericTOB(const jLargeRJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JET)
{}

// constructor from large R gjet
TCS::GenericTOB::GenericTOB(const gLargeRJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JET)
{}

// constructor from small R jjet
TCS::GenericTOB::GenericTOB(const jJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JET)
{}

// constructor from small R gjet
TCS::GenericTOB::GenericTOB(const gJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JET)
{}

// constructor from cluster
TCS::GenericTOB::GenericTOB(const ClusterTOB & cluster) :
   BaseTOB(cluster.roiWord(), cluster.tobName())
   , m_Et(cluster.Et())
   , m_eta(cluster.eta())
   , m_phi(cluster.phi())
   , m_EtDouble(cluster.EtDouble())
   , m_etaDouble(cluster.etaDouble())
   , m_phiDouble(cluster.phiDouble())
   , m_tobType(cluster.tobType())
{}

// constructor from eEm
TCS::GenericTOB::GenericTOB(const eEmTOB & eem) :
   BaseTOB(eem.roiWord(), eem.tobName())
   , m_Et(eem.Et())
   , m_eta(eem.eta())
   , m_phi(static_cast<int>(eem.phi()))
   , m_EtDouble(eem.EtDouble())
   , m_etaDouble(eem.etaDouble())
   , m_phiDouble(eem.phiDouble())
   , m_reta(eem.Reta())
   , m_rhad(eem.Rhad())
   , m_wstot(eem.Wstot())
   , m_tobType(eem.tobType())
{}

// constructor from eTau
TCS::GenericTOB::GenericTOB(const eTauTOB & etau) :
   BaseTOB(etau.roiWord(), etau.tobName())
   , m_Et(etau.Et())
   , m_eta(etau.eta())
   , m_phi(static_cast<int>(etau.phi()))
   , m_EtDouble(etau.EtDouble())
   , m_etaDouble(etau.etaDouble())
   , m_phiDouble(etau.phiDouble())
   , m_fcore(etau.isolation())
   , m_tobType(etau.tobType())
{}

// constructor from cTau
TCS::GenericTOB::GenericTOB(const cTauTOB & ctau) :
   BaseTOB(ctau.roiWord(), ctau.tobName())
   , m_Et(ctau.Et())
   , m_eta(ctau.eta())
   , m_phi(static_cast<int>(ctau.phi()))
   , m_EtDouble(ctau.EtDouble())
   , m_etaDouble(ctau.etaDouble())
   , m_phiDouble(ctau.phiDouble())
   , m_reta(ctau.Reta())
   , m_rhad(ctau.Rhad())
   , m_wstot(ctau.Wstot())
   , m_tobType(ctau.tobType())
{}

// constructor from muon
TCS::GenericTOB::GenericTOB(const MuonTOB & muon) :
   BaseTOB(muon.roiWord(), muon.tobName())
   , m_Et(muon.Et())
   , m_eta(muon.eta())
   , m_phi(static_cast<int>(muon.phi()))
   , m_bw2or3(muon.bw2or3())
   , m_innerCoin(muon.innerCoin())
   , m_goodMF(muon.goodMF())
   , m_charge(muon.charge())
   , m_is2cand(muon.is2cand())
   , m_EtDouble(muon.EtDouble())
   , m_etaDouble(muon.EtaDouble())
   , m_phiDouble(muon.PhiDouble())
   , m_tobType(MUON)
{}

// constructor for latemuon
TCS::GenericTOB::GenericTOB(const LateMuonTOB & lateMuon) :
   BaseTOB(lateMuon.roiWord(), lateMuon.tobName())
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
   BaseTOB(muonNextBC.roiWord(), muonNextBC.tobName())
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
   BaseTOB(met.roiWord(), met.tobName())
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
