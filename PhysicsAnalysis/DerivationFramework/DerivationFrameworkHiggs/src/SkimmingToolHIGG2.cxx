/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHIGG2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Susumu Oda (Susumu.Oda@cern.ch)
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkHiggs/SkimmingToolHIGG2.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"

// Constructor
DerivationFramework::SkimmingToolHIGG2::SkimmingToolHIGG2(const std::string& t,
							  const std::string& n,
							  const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_ntot(0),
  m_npass(0),
  m_DFCommonElectrons_pt(0),
  m_DFCommonElectrons_eta(0),
  m_DFCommonElectrons_phi(0)
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("SkipTriggerRequirement", m_skipTriggerRequirement=false);

  declareProperty("FilterType", m_filterType="2L");

  declareProperty("ElectronContainerKey", m_electronSGKey="Electrons");
  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
  declareProperty("JetContainerKey", m_jetSGKey="AntiKt4EMTopoJets");
  declareProperty("MergedJetContainerKey0", m_mergedJetSGKey[0]="AntiKt4EMTopoJets");
  declareProperty("MergedJetContainerKey1", m_mergedJetSGKey[1]="AntiKt10LCTopoJets");
  declareProperty("MergedJetContainerKey2", m_mergedJetSGKey[2]="CamKt12LCTopoJets");
  declareProperty("PhotonContainerKey", m_photonSGKey="Photons");

  declareProperty("NumberOfLeptons", m_nLeptons=2);
  declareProperty("NumberOfElectrons", m_nElectrons=0);
  declareProperty("NumberOfMuons", m_nMuons=0);
  declareProperty("NumberOfJets", m_nJets=0);
  declareProperty("NumberOfMergedJets0", m_nMergedJets[0]=0);
  declareProperty("NumberOfMergedJets1", m_nMergedJets[1]=0);
  declareProperty("NumberOfMergedJets2", m_nMergedJets[2]=0);
  declareProperty("NumberOfPhotons", m_nPhotons=0);

  declareProperty("ElectronQuality", m_electronQual="any");
  declareProperty("MuonQuality", m_muonQual="inMS");
  declareProperty("JetQuality", m_jetQual="any");
  declareProperty("MergedJetQuality0", m_mergedJetQual[0]="any");
  declareProperty("MergedJetQuality1", m_mergedJetQual[1]="any");
  declareProperty("MergedJetQuality2", m_mergedJetQual[2]="any");
  declareProperty("PhotonQuality", m_photonQual="any");

  declareProperty("PrimaryElectronQuality4L", m_primaryElectronQual4L="DFCommonElectronsLHLoose");

  declareProperty("Trigger2L", m_trigger2L=std::vector<std::string>()); 
  declareProperty("TriggerTP", m_triggerTP=std::vector<std::string>()); 
  declareProperty("TriggerJPSI", m_triggerJPSI=std::vector<std::string>()); 

  declareProperty("ElectronEtCut", m_electronEtCut=10.*CLHEP::GeV); 
  declareProperty("MuonPtCut", m_muonPtCut=10.*CLHEP::GeV); 
  declareProperty("JetPtCut", m_jetPtCut=15.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut0", m_mergedJetPtCut[0]=100.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut1", m_mergedJetPtCut[1]=150.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut2", m_mergedJetPtCut[2]=150.*CLHEP::GeV); 
  declareProperty("PhotonPtCut", m_photonPtCut=15.*CLHEP::GeV); 

  declareProperty("ElectronEtaCut", m_electronEtaCut=9999.); 
  declareProperty("MuonEtaCut", m_muonEtaCut=9999.); 
  declareProperty("CaloMuonEtaCut", m_caloMuonEtaCut=0.2); 
  declareProperty("JetEtaCut", m_jetEtaCut=2.6); 
  declareProperty("MergedJetEtaCut0", m_mergedJetEtaCut[0]=2.6); 
  declareProperty("MergedJetEtaCut1", m_mergedJetEtaCut[1]=2.6); 
  declareProperty("MergedJetEtaCut2", m_mergedJetEtaCut[2]=2.6); 
  declareProperty("PhotonEtaCut", m_photonEtaCut=2.5); 

  declareProperty("InvariantMassCut", m_invariantMassCut=0.*CLHEP::GeV); 
  declareProperty("InvariantMassJpsiLowCut", m_invariantMassJpsiLowCut=2.0*CLHEP::GeV); 
  declareProperty("InvariantMassJpsiUpCut", m_invariantMassJpsiUpCut=4.3*CLHEP::GeV); 
  declareProperty("InvariantMassUpsilonLowCut", m_invariantMassUpsilonLowCut=8.0*CLHEP::GeV); 
  declareProperty("InvariantMassUpsilonUpCut", m_invariantMassUpsilonUpCut=12.0*CLHEP::GeV); 

  declareProperty("LeadingElectronEtCut", m_leadingElectronEtCut=0.*CLHEP::GeV); 
  declareProperty("LeadingMuonPtCut", m_leadingMuonPtCut=0.*CLHEP::GeV); 

  declareProperty("DRElectronJetCut", m_dRElectronJetCut=0.05);   

  declareProperty("CheckLArError", m_checkLArError=true);	
  declareProperty("UseDefaultElectronFourMomentum", m_defaultElectronFourMomentum=false);
  declareProperty("UseDFCommonElectronFourMomentum", m_DFCommonElectronFourMomentum=true);
}
  
// Destructor
DerivationFramework::SkimmingToolHIGG2::~SkimmingToolHIGG2() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHIGG2::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_defaultElectronFourMomentum==true and m_DFCommonElectronFourMomentum==true) {
    ATH_MSG_FATAL("Wrong setting both m_defaultElectronFourMomentum and m_DFCommonElectronFourMomentum are true.");
    return StatusCode::FAILURE;
  }

  // trigger decision tool
  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolHIGG2::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHIGG2::eventPassesFilter() const
{
  m_ntot++;
  bool acceptEvent(false);
  StatusCode sc(StatusCode::SUCCESS, true);

  // Retrieve EventInfo
  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo)); 

  // LAr error check
  if(m_checkLArError) {
    if(eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) {
      return acceptEvent; 
    }
  }
  
  // Electrons
  m_goodElectrons.clear();
  m_mapElectrons.clear();
  if(m_nElectrons>0 or m_nLeptons>0) {
    const xAOD::ElectronContainer *electrons(0); 
    ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
    xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
    xAOD::ElectronContainer::const_iterator el_end(electrons->end());
    unsigned int el_i(0);
    for(; el_itr != el_end; ++el_itr, el_i++) {
      if(this->checkElectronQuality(*el_itr)) m_goodElectrons.push_back(*el_itr);
      m_mapElectrons.insert(std::pair<const xAOD::Electron*, unsigned int>(*el_itr, el_i));
    } 

    if(m_DFCommonElectronFourMomentum) {
      ATH_CHECK(evtStore()->retrieve(m_DFCommonElectrons_pt,  "DFCommonElectrons_pt"));
      ATH_CHECK(evtStore()->retrieve(m_DFCommonElectrons_eta, "DFCommonElectrons_eta"));
      ATH_CHECK(evtStore()->retrieve(m_DFCommonElectrons_phi, "DFCommonElectrons_phi"));
    }
  }

  // Muons
  m_goodMuons.clear();
  if(m_nMuons>0 or m_nLeptons>0) {
    const xAOD::MuonContainer *muons(0); 
    ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));
    xAOD::MuonContainer::const_iterator mu_itr(muons->begin());
    xAOD::MuonContainer::const_iterator mu_end(muons->end());
    for(; mu_itr != mu_end; ++mu_itr) {
      if(this->checkMuonQuality(*mu_itr)) m_goodMuons.push_back(*mu_itr);
    } 
  }

  // Jets
  m_goodJets.clear();
  if(m_nJets>0) {
    const xAOD::JetContainer *jets(0); 
    ATH_CHECK(evtStore()->retrieve(jets, m_jetSGKey));
    xAOD::JetContainer::const_iterator jet_itr(jets->begin());
    xAOD::JetContainer::const_iterator jet_end(jets->end());
    for(; jet_itr != jet_end; ++jet_itr) {
      if(this->checkJetQuality(*jet_itr)) m_goodJets.push_back(*jet_itr);
     }
  }
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    m_goodMergedJets[type].clear();
    if(m_nMergedJets[type]>0) {
      const xAOD::JetContainer *jets(0);
      ATH_CHECK(evtStore()->retrieve(jets, m_mergedJetSGKey[type]));
      xAOD::JetContainer::const_iterator jet_itr(jets->begin());
      xAOD::JetContainer::const_iterator jet_end(jets->end());
      for(; jet_itr != jet_end; ++jet_itr) {
	if(this->checkMergedJetQuality(*jet_itr, type)) m_goodMergedJets[type].push_back(*jet_itr);
      }
    }
  }

  // Photons
  m_goodPhotons.clear();
  if(m_nPhotons>0) {
    const xAOD::PhotonContainer *photons(0); 
    ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
    xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
    xAOD::PhotonContainer::const_iterator ph_end(photons->end());
    for(; ph_itr != ph_end; ++ph_itr) {
      if(this->checkPhotonQuality(*ph_itr)) m_goodPhotons.push_back(*ph_itr);
    }
  }

  // Check conditions of filters
  if(m_filterType=="2L") {
    if(this->check2L()) acceptEvent = true;
  } else if(m_filterType=="4L") {
    if(this->check4L()) acceptEvent = true;
  } else if(m_filterType=="TP") {
    if(this->checkTP()) acceptEvent = true;
  } else if(m_filterType=="2L2Q") {
    if(this->check2L2Q()) acceptEvent = true;
  } else if(m_filterType=="JPSI") {
    if(this->checkJPSI()) acceptEvent = true;
  }

  if(acceptEvent) m_npass++;
  return acceptEvent; 
}  

bool DerivationFramework::SkimmingToolHIGG2::checkElectronQuality(const xAOD::Electron *el) const 
{
  if(!el) return false;

  if(m_electronQual!="any") {
    bool value(false);
    if(!el->passSelection(value, m_electronQual)) {
      ATH_MSG_WARNING("xAOD::Electron does not have menu of " << m_electronQual);
      return false;
    } 
    if(!value) return false;
  }

  const xAOD::TrackParticle *trackParticle(el->trackParticle());
  if(!trackParticle) {
    ATH_MSG_DEBUG("xAOD::TrackParticle pointer is null");
    return false;
  }
  uint8_t numberOfPixelHits(0);
  if(!trackParticle->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits)) {
    ATH_MSG_WARNING("xAOD::TrackParticle does not give summaryValue correctly for xAOD::numberOfPixelHits");
    return false;
  }
  uint8_t numberOfSCTHits(0);
  if(!trackParticle->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits)) {
    ATH_MSG_WARNING("xAOD::TrackParticle does not give summaryValue correctly for xAOD::numberOfSCTHits");
    return false;
  }
  if(numberOfPixelHits+numberOfSCTHits==0) return false; 

  const xAOD::CaloCluster *caloCluster(el->caloCluster());
  if(!caloCluster) {
    ATH_MSG_WARNING("xAOD::CaloCluster pointer is null");
    return false;
  }

  double eta(trackParticle->eta());
  if(fabs(eta)>10.) return false;
  double et(caloCluster->e()/cosh(eta));

  if(et<m_electronEtCut) return false;
  if(fabs(eta)>m_electronEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHIGG2::checkMuonQuality(const xAOD::Muon *mu) const 
{
  if(!mu) return false;

  if(m_muonQual=="any") {
    // do nothing
  } else if(m_muonQual=="combined") {
    if(mu->muonType()!=xAOD::Muon::Combined) return false;
  } else if(m_muonQual=="standalone") {
    if(mu->muonType()!=xAOD::Muon::MuonStandAlone) return false;
  } else if(m_muonQual=="lowpt") {
    if(mu->muonType()!=xAOD::Muon::SegmentTagged) return false;
  } else if(m_muonQual=="combined+lowpt") {
    if(mu->muonType()!=xAOD::Muon::Combined and mu->muonType()!=xAOD::Muon::SegmentTagged) return false;
  } else if(m_muonQual=="inMS") {
    if(mu->muonType()==xAOD::Muon::MuonStandAlone and fabs(fabs(mu->eta())-2.6)>0.12) return false;
  } else if(m_muonQual=="DFCommonGoodMuon") { // Derivation Framework variable
    if(!mu->isAvailable<int>("DFCommonGoodMuon") or !mu->auxdata<int>("DFCommonGoodMuon")) return false;
  } else {
    ATH_MSG_WARNING("Muon quality " << m_muonQual << "is not defined");
    return false; 
  }

  if(mu->pt()<m_muonPtCut) return false;
  if(fabs(mu->eta())>m_muonEtaCut) return false;
  if(mu->muonType()==xAOD::Muon::CaloTagged and fabs(mu->eta())>m_caloMuonEtaCut) return false;

  return true;
}
  
bool DerivationFramework::SkimmingToolHIGG2::checkJetQuality(const xAOD::Jet *jet) const 
{
  if(!jet) return false;

  if(m_jetQual!="any") {
    // do something
  }

  if(jet->pt()<m_jetPtCut) return false;
  if(fabs(jet->eta())>m_jetEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHIGG2::checkMergedJetQuality(const xAOD::Jet *jet, const unsigned int type) const 
{
  if(!jet) return false;

  if(m_mergedJetQual[type]!="any") {
    // do something
  }

  if(jet->pt()<m_mergedJetPtCut[type]) return false;
  if(fabs(jet->eta())>m_mergedJetEtaCut[type]) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHIGG2::checkPhotonQuality(const xAOD::Photon *ph) const 
{
  if(!ph) return false;

  if(m_photonQual!="any") {
    bool value(false);
    if(!ph->passSelection(value, m_photonQual)) {
      ATH_MSG_WARNING("xAOD::Photon does not have menu of " << m_photonQual);
      return false;
    } 
    if(!value) return false;
  }

  if(ph->pt()<m_photonPtCut) return false;
  if(fabs(ph->eta())>m_photonEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHIGG2::check2L() const 
{
  if(!(m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;
  if(!(m_goodJets.size()>=m_nJets and m_goodPhotons.size()>=m_nPhotons)) return false;

  bool isTriggerFired(m_trigger2L.size()==0 or m_skipTriggerRequirement);
  for(unsigned int i(0); i<m_trigger2L.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_trigger2L.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  if(!isTriggerFired) return false; 

  std::vector<TLorentzVector> v_tlv;
  std::vector<bool> v_isElectron;
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(true);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(false);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
      // Check two lepton mass
      if(tlv_2lep.M()>m_invariantMassCut) return true;
    }
  }

  return false;
}

bool DerivationFramework::SkimmingToolHIGG2::check4L() const 
{
  if(!(m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;
  if(!(m_goodJets.size()>=m_nJets and m_goodPhotons.size()>=m_nPhotons)) return false;

  std::vector<TLorentzVector> v_tlv;
  std::vector<bool> v_pid;
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i)); 
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.push_back(tlv);

    bool val(false);
    bool defined(false);
    if(m_primaryElectronQual4L=="DFCommonElectronsLHLoose"  or
       m_primaryElectronQual4L=="DFCommonElectronsLHMedium" or
       m_primaryElectronQual4L=="DFCommonElectronsLHTight"  or
       m_primaryElectronQual4L=="DFCommonElectronsML") {
      // Use Derivation Framework variable to pickup possible new correction result
      // If the same electron likelihood configuration is used in AOD creation and DAOD creation, 
      // DFCommonElectronsLHLoose and LHLoose are identical.
      if(el->isAvailable<int>(m_primaryElectronQual4L)) {
	defined = true;
	val = static_cast<bool>(el->auxdata<int>(m_primaryElectronQual4L));
      }
    } else {
      // Electron menu is defined in http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
      defined = el->passSelection(val, m_primaryElectronQual4L);
    }

    v_pid.push_back(val and defined);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
    v_pid.push_back(true);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      for(unsigned int i2(i1+1); i2<nGoodLeptons; i2++) {
	for(unsigned int i3(i2+1); i3<nGoodLeptons; i3++) {
	  TLorentzVector tlv_4lep(v_tlv.at(i0) + v_tlv.at(i1) + v_tlv.at(i2) + v_tlv.at(i3));
	  // Check four lepton mass
	  if(tlv_4lep.M()<m_invariantMassCut) continue;
	  
	  // Check primary dilepton's electron PID (muon PID is always true)
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i1)).M()-s_MZ)<fabs((v_tlv.at(i2)+v_tlv.at(i3)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i1)) return true; 
	  } else {
	    if(v_pid.at(i2) and v_pid.at(i3)) return true;
	  }
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i2)).M()-s_MZ)<fabs((v_tlv.at(i1)+v_tlv.at(i3)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i2)) return true;
	  } else {
	    if(v_pid.at(i1) and v_pid.at(i3)) return true;
	  }
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i3)).M()-s_MZ)<fabs((v_tlv.at(i1)+v_tlv.at(i2)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i3)) return true;
	  } else {
	    if(v_pid.at(i1) and v_pid.at(i2)) return true;
	  }
	}
      }
    } 
  }

  return false;
}

bool DerivationFramework::SkimmingToolHIGG2::checkTP() const 
{
  if(!(m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;

  bool isTriggerFired(m_triggerTP.size()==0 or m_skipTriggerRequirement);
  for(unsigned int i(0); i<m_triggerTP.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_triggerTP.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  if(!isTriggerFired) return false; 

  std::vector<TLorentzVector> v_tlv;
  std::vector<bool> v_isElectron;
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(true);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(false);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      if(v_isElectron.at(i0)!=v_isElectron.at(i1)) continue;
      
      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
      // Check two lepton mass
      if(tlv_2lep.M()<m_invariantMassCut) continue;

      // Leading lepton ET/pT cut
      if((v_tlv.at(i0).Pt()>v_tlv.at(i1).Pt() ? v_tlv.at(i0).Pt() : v_tlv.at(i1).Pt()) > (v_isElectron.at(i0) ? m_leadingElectronEtCut : m_leadingMuonPtCut)) return true;
    }
  }

  return false;
}

bool DerivationFramework::SkimmingToolHIGG2::check2L2Q() const 
{
  if(!(m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;

  bool checkGoodJets(m_goodJets.size()>=m_nJets and m_nJets>0);
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    if(m_nMergedJets[type]>0) {
      checkGoodJets = (checkGoodJets or (m_goodMergedJets[type].size()>=m_nMergedJets[type]));
    }
  }
  if(!checkGoodJets) return false;

  std::vector<TLorentzVector> v_tlv;
  std::vector<bool> v_isElectron;
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(true);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
    v_isElectron.push_back(false);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      if(v_isElectron.at(i0)!=v_isElectron.at(i1)) continue;
      
      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
      // Check two lepton mass
      if(tlv_2lep.M()<m_invariantMassCut) continue;

      // dR(e-jet)>0.05 is required for at least two jets
      if(v_isElectron.at(i0)) {
	unsigned int nGoodJetsWithDRCut(0);
	unsigned int nGoodJets(m_goodJets.size());
	for(unsigned int j(0); j<nGoodJets; j++) {
	  const xAOD::Jet *jet(m_goodJets.at(j));

	  double dR_0(this->getDeltaR(v_tlv.at(i0).Eta(), v_tlv.at(i0).Phi(), jet->eta(), jet->phi()));
	  if(dR_0<m_dRElectronJetCut) continue;
	  
	  double dR_1(this->getDeltaR(v_tlv.at(i1).Eta(), v_tlv.at(i1).Phi(), jet->eta(), jet->phi()));
	  if(dR_1<m_dRElectronJetCut) continue;
	  
	  nGoodJetsWithDRCut++;
	}

	bool checkGoodJetsWithDRCut(nGoodJetsWithDRCut>=m_nJets and m_nJets>0);

	for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
	  if(m_nMergedJets[type]>0) {
	    unsigned int nGoodMergedJetsWithDRCut(0);
	    unsigned int nGoodMergedJets(m_goodMergedJets[type].size());
	    for(unsigned int j(0); j<nGoodMergedJets; j++) {
	      const xAOD::Jet *jet(m_goodMergedJets[type].at(j));
	      
	      double dR_0(this->getDeltaR(v_tlv.at(i0).Eta(), v_tlv.at(i0).Phi(), jet->eta(), jet->phi()));
	      if(dR_0<m_dRElectronJetCut) continue;
	      
	      double dR_1(this->getDeltaR(v_tlv.at(i1).Eta(), v_tlv.at(i1).Phi(), jet->eta(), jet->phi()));
	      if(dR_1<m_dRElectronJetCut) continue;
	      
	      nGoodMergedJetsWithDRCut++;
	    }

	    checkGoodJetsWithDRCut = (checkGoodJetsWithDRCut or (nGoodMergedJetsWithDRCut>=m_nMergedJets[type]));
	  }
	}
	if(!checkGoodJetsWithDRCut) return false; 
      }

      // Leading lepton ET/pT cut
      if((v_tlv.at(i0).Pt()>v_tlv.at(i1).Pt() ? v_tlv.at(i0).Pt() : v_tlv.at(i1).Pt()) > (v_isElectron.at(i0) ? m_leadingElectronEtCut : m_leadingMuonPtCut)) return true;
    }
  }

  return false;
}

bool DerivationFramework::SkimmingToolHIGG2::checkJPSI() const 
{
  if(!(m_nMuons>0 and m_goodMuons.size()>=m_nMuons)) return false;
  if(!(m_goodPhotons.size()>=m_nPhotons)) return false;

  bool isTriggerFired(m_triggerJPSI.size()==0 or m_skipTriggerRequirement);
  for(unsigned int i(0); i<m_triggerJPSI.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_triggerJPSI.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  if(!isTriggerFired) return false; 

  std::vector<TLorentzVector> v_tlv;
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));

      // Check di-muon mass
      if((tlv_2lep.M()<m_invariantMassJpsiLowCut    or tlv_2lep.M()>m_invariantMassJpsiUpCut) and
	 (tlv_2lep.M()<m_invariantMassUpsilonLowCut or tlv_2lep.M()>m_invariantMassUpsilonUpCut)) continue;

      // Check leading muon pT
      if((v_tlv.at(i0).Pt()>v_tlv.at(i1).Pt() ? v_tlv.at(i0).Pt() : v_tlv.at(i1).Pt())>m_leadingMuonPtCut) return true;
    }
  }

  return false;
}

TLorentzVector DerivationFramework::SkimmingToolHIGG2::electronFourMomentum(const xAOD::Electron *el) const
{
  TLorentzVector tlv;
  tlv.SetPtEtaPhiE(el->pt(), el->eta(), el->phi(), el->e());
  if(m_defaultElectronFourMomentum) return tlv;

  if(m_DFCommonElectronFourMomentum) {
    std::map<const xAOD::Electron*, unsigned int>::iterator map_itr(m_mapElectrons.find(el));
    if(map_itr==m_mapElectrons.end()) {
      ATH_MSG_WARNING("xAOD::Electron *el is not found in m_mapElectrons");
      return tlv;
    }

    unsigned int el_i = map_itr->second;
    tlv.SetPtEtaPhiM(m_DFCommonElectrons_pt->at(el_i), 
		     m_DFCommonElectrons_eta->at(el_i),
		     m_DFCommonElectrons_phi->at(el_i),
		     el->m());
    return tlv;
  }

  const xAOD::TrackParticle *trackParticle(el->trackParticle());
  if(!trackParticle) {
    ATH_MSG_WARNING("xAOD::TrackParticle pointer is null");
    return tlv;
  }
  const xAOD::CaloCluster *caloCluster(el->caloCluster());
  if(!caloCluster) {
    ATH_MSG_WARNING("xAOD::CaloCluster pointer is null");
    return tlv;
  }
    
  double eta(trackParticle->eta());
  double phi(trackParticle->phi());
  if(fabs(eta)>10.) {
    ATH_MSG_WARNING("fabs(trackParticle->eta()) = " << eta << " > 10.");
    return tlv;
  }
  double e(caloCluster->e());
  double et(e/cosh(eta));
    
  tlv.SetPtEtaPhiE(et, eta, phi, e);
  return tlv;
}

TLorentzVector DerivationFramework::SkimmingToolHIGG2::muonFourMomentum(const xAOD::Muon *mu) const
{
  TLorentzVector tlv;
  tlv.SetPtEtaPhiM(mu->pt(), mu->eta(), mu->phi(), mu->m());
  return tlv;
}

double DerivationFramework::SkimmingToolHIGG2::getDeltaR(const double eta1, const double phi1, const double eta2, const double phi2) const 
{
  double dEta(eta1 - eta2);
  double dPhi(phi1 - phi2);
  if(dPhi>+M_PI) dPhi -= 2.*M_PI;
  if(dPhi<-M_PI) dPhi += 2.*M_PI;
  double dR(sqrt(dEta*dEta + dPhi*dPhi));
  return dR;
}

const double DerivationFramework::SkimmingToolHIGG2::s_MZ = 91187.6*CLHEP::MeV; 
