// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

// constructor from jFEX Tau
TCS::GenericTOB::GenericTOB(const jTauTOB & tau) :
   BaseTOB(tau.roiWord(), tau.tobName())
   , m_Et(tau.Et())
   , m_eta(tau.eta())
   , m_phi(static_cast<int>(tau.phi()))
   , m_EtDouble(tau.EtDouble())
   , m_etaDouble(tau.etaDouble())
   , m_phiDouble(tau.phiDouble())
   , m_tobType(JTAU)
{}

// constructor from jFEX Em
TCS::GenericTOB::GenericTOB(const jEmTOB & jem) :
   BaseTOB(jem.roiWord(), jem.tobName())
   , m_Et(jem.Et())
   , m_eta(jem.eta())
   , m_phi(static_cast<int>(jem.phi()))
   , m_EtDouble(jem.EtDouble())
   , m_etaDouble(jem.etaDouble())
   , m_phiDouble(jem.phiDouble())
   , m_tobType(JEM)
{}

// constructor from jFEX LJet
TCS::GenericTOB::GenericTOB(const jLJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JLJET)
{}

// constructor from gFEX LJet
TCS::GenericTOB::GenericTOB(const gLJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(GLJET)
{}

// constructor from jFEX Jet
TCS::GenericTOB::GenericTOB(const jJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(JJET)
{}

// constructor from gFEX Jet
TCS::GenericTOB::GenericTOB(const gJetTOB & jet) :
   BaseTOB(jet.roiWord(), jet.tobName())
   , m_Et(jet.Et())
   , m_eta(jet.eta())
   , m_phi(static_cast<int>(jet.phi()))
   , m_EtDouble(jet.EtDouble())
   , m_etaDouble(jet.etaDouble())
   , m_phiDouble(jet.phiDouble())
   , m_tobType(GJET)
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

// constructor from eFEX Em
TCS::GenericTOB::GenericTOB(const eEmTOB & eem) :
   BaseTOB(eem.roiWord(), eem.tobName())
   , m_Et(eem.Et())
   , m_eta(eem.eta())
   , m_phi(static_cast<int>(eem.phi()))
   , m_EtDouble(eem.EtDouble())
   , m_etaDouble(eem.etaDouble())
   , m_phiDouble(eem.phiDouble())
   , m_tobType(EEM)
{}

// constructor from eFEX Tau
TCS::GenericTOB::GenericTOB(const eTauTOB & etau) :
   BaseTOB(etau.roiWord(), etau.tobName())
   , m_Et(etau.Et())
   , m_eta(etau.eta())
   , m_phi(static_cast<int>(etau.phi()))
   , m_EtDouble(etau.EtDouble())
   , m_etaDouble(etau.etaDouble())
   , m_phiDouble(etau.phiDouble())
   , m_tobType(ETAU)
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
   , m_phi(static_cast<int>(lateMuon.phi()))
   , m_EtDouble(lateMuon.EtDouble())
   , m_etaDouble(lateMuon.EtaDouble())
   , m_phiDouble(lateMuon.PhiDouble())
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

// constructor from met
TCS::GenericTOB::GenericTOB(const MetTOB & met) :
   BaseTOB(met.roiWord(), met.tobName())
   , m_Et(met.Et())
   , m_Ex(met.Ex())
   , m_Ey(met.Ey())
   , m_tobType(MET)
{}

// constructor from jFEX XE
TCS::GenericTOB::GenericTOB(const jXETOB & jxe) :
   BaseTOB(jxe.roiWord(), jxe.tobName())
   , m_Et(jxe.Et())
   , m_Ex(jxe.Ex())
   , m_Ey(jxe.Ey())
   , m_Et2(jxe.Et2())
   , m_EtDouble(jxe.EtDouble())
   , m_ExDouble(jxe.ExDouble())
   , m_EyDouble(jxe.EyDouble())
   , m_tobType(jxe.tobType())
{}

// constructor from jFEX TE
TCS::GenericTOB::GenericTOB(const jTETOB & jte) :
   BaseTOB(jte.roiWord(), jte.tobName())
   , m_sumEt(jte.sumEt())
   , m_sumEtDouble(jte.sumEtDouble())
   , m_tobType(jte.tobType())
{}

// constructor from gFEX XE
TCS::GenericTOB::GenericTOB(const gXETOB & gxe) :
   BaseTOB(gxe.roiWord(), gxe.tobName())
   , m_Et(gxe.Et())
   , m_Ex(gxe.Ex())
   , m_Ey(gxe.Ey())
   , m_Et2(gxe.Et2())
   , m_EtDouble(gxe.EtDouble())
   , m_ExDouble(gxe.ExDouble())
   , m_EyDouble(gxe.EyDouble())
   , m_tobType(gxe.tobType())
{}

// constructor from gFEX TE
TCS::GenericTOB::GenericTOB(const gTETOB & gte) :
   BaseTOB(gte.roiWord(), gte.tobName())
   , m_sumEt(gte.sumEt())
   , m_sumEtDouble(gte.sumEtDouble())
   , m_tobType(gte.tobType())
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
   o << "generic tob type: " << tobType() << ",  energy: " << Et() << ", sumEt(): " << sumEt() << ", eta: " << eta() << ", phi: " << phi();
}
