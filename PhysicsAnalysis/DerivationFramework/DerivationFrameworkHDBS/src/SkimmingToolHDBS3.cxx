/*
  Copyright (C) 2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHDBS3.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Panagiotis Bellos (panagiotis.bellos@cern.ch)
// Skimming tool for H->Za analysis

#include "DerivationFrameworkHDBS/SkimmingToolHDBS3.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::SkimmingToolHDBS3::SkimmingToolHDBS3( const std::string& t,const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p), m_ntot(0), m_npass(0), m_muonSGKey("Muons"), m_nMuons(2), m_muonPtCut(10.0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("MuonContainerKey", m_muonSGKey);
    declareProperty("ElectronContainerKey", m_electronSGKey=std::string("Electrons"));
    declareProperty("MuonPtCut", m_muonPtCut);
    declareProperty("ElectronPtCut", m_electronPtCut=10.*CLHEP::GeV);
    declareProperty("InvariantMassCut_min", m_invariantMassCut_min=0.*CLHEP::GeV);
    declareProperty("InvariantMassCut_max", m_invariantMassCut_max=0.*CLHEP::GeV);
    declareProperty("FilterType", m_filterType="2L");
  }

// Destructor
DerivationFramework::SkimmingToolHDBS3::~SkimmingToolHDBS3() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHDBS3::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::SkimmingToolHDBS3::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     ATH_MSG_INFO("Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter ");
     return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHDBS3::eventPassesFilter() const
{
    ++m_ntot;

    // Retrieve muon container
    const xAOD::MuonContainer* muons(0);
    StatusCode sc = evtStore()->retrieve(muons,m_muonSGKey);

    if (sc.isFailure()) { 	ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!"); return StatusCode::FAILURE;}

     /// Loop over muons, count up and set decision


    xAOD::MuonContainer::const_iterator muItr;
    unsigned int nGoodMu(0);
    std::vector<TLorentzVector> TLV;
    std::vector<int> Charge;
    TLorentzVector tlv;

    for (muItr=muons->begin(); muItr!=muons->end(); ++muItr) {
	    if (  (*muItr)->pt() > m_muonPtCut )  ++nGoodMu;
        Charge.push_back((*muItr)->charge());
        TLV.push_back(TLorentzVector());
        TLV.back().SetPtEtaPhiM((*muItr)->pt(),(*muItr)->eta(), (*muItr)->phi(), 105.6583); }

  ///----------- calculating dimuon invariant mass ------------------------------

    bool IMM=0;

     if (nGoodMu>=2){
        for (unsigned int I=0; I<TLV.size(); I++){
           for (unsigned int J=I+1; J<TLV.size(); J++){
                if (Charge[I]*Charge[J]>0) continue;
                tlv=TLV.at(I)+TLV.at(J);
                if (tlv.M()>m_invariantMassCut_min && tlv.M()<m_invariantMassCut_max) IMM=true;}}}

    TLV.clear();
    Charge.clear();

    // Retrieve electron container
    const xAOD::ElectronContainer* electrons(0);
    sc = evtStore()->retrieve(electrons,m_electronSGKey);

    if (sc.isFailure()) { 	ATH_MSG_FATAL("No electron collection with name " << m_electronSGKey << " found in StoreGate!"); return StatusCode::FAILURE; }

     /// Loop over electrons, count up and set decision
    xAOD::ElectronContainer::const_iterator elItr;
    unsigned int nGoodEl(0);


    for (elItr=electrons->begin(); elItr!=electrons->end(); ++elItr) {
      if ( (*elItr)->pt() > m_electronPtCut ) ++nGoodEl;
      Charge.push_back((*elItr)->charge());
      TLV.push_back(TLorentzVector());
      TLV.back().SetPtEtaPhiM((*elItr)->pt(),(*elItr)->eta(), (*elItr)->phi(), 0.510998); }


    ///----------- calculating dielectron invariant mass ------------------------------
    bool IME=0;

     if (nGoodEl>=2){
        for (unsigned int I=0; I<TLV.size(); I++){
           for (unsigned int J=I+1; J<TLV.size(); J++){
                if (Charge[I]*Charge[J]>0) continue;
                tlv=TLV.at(I)+TLV.at(J);
                if (tlv.M()>m_invariantMassCut_min && tlv.M()<m_invariantMassCut_max) IME=true;}}}

    TLV.clear();
    Charge.clear();


    bool ae2L = 0, aeIM = 0, acceptEvent=0;

    if (m_filterType=="2L" && (nGoodEl >= 2 || nGoodMu >= 2)) {ae2L = true;}
    if ((m_invariantMassCut_min!=0 || m_invariantMassCut_max!=0) && (IMM==true || IME==true)) {aeIM = true;}
    if ((m_invariantMassCut_min==0 && m_invariantMassCut_max==0))                             {aeIM = true;}


    if (ae2L==1 && aeIM==1) {++m_npass; acceptEvent=true;}

    return acceptEvent;
}

