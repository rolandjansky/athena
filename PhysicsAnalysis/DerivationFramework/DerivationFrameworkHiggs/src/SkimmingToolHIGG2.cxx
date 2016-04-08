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
  m_npass(0)
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
  declareProperty("TrackContainerKey", m_trackSGKey="InDetTrackParticles");

  declareProperty("NumberOfLeptons", m_nLeptons=2);
  declareProperty("NumberOfElectrons", m_nElectrons=0);
  declareProperty("NumberOfMuons", m_nMuons=0);
  declareProperty("NumberOfJets", m_nJets=0);
  declareProperty("NumberOfMergedJets0", m_nMergedJets[0]=0);
  declareProperty("NumberOfMergedJets1", m_nMergedJets[1]=0);
  declareProperty("NumberOfMergedJets2", m_nMergedJets[2]=0);
  declareProperty("NumberOfPhotons", m_nPhotons=0);
  declareProperty("NumberOfTracks", m_nTracks=0);

  declareProperty("ElectronQuality", m_electronQual="any");
  declareProperty("MuonQuality", m_muonQual="inMS");
  declareProperty("TightElectronQuality", m_tightElectronQual="DFCommonElectronsLHVeryLoose");
  declareProperty("TightMuonQuality", m_tightMuonQual="DFCommonGoodMuon");
  declareProperty("JetQuality", m_jetQual="any");
  declareProperty("MergedJetQuality0", m_mergedJetQual[0]="any");
  declareProperty("MergedJetQuality1", m_mergedJetQual[1]="any");
  declareProperty("MergedJetQuality2", m_mergedJetQual[2]="any");
  declareProperty("PhotonQuality", m_photonQual="any");

  declareProperty("PrimaryElectronQuality4L", m_primaryElectronQual4L="DFCommonElectronsLHLoose");

  declareProperty("Trigger2L", m_trigger2L=std::vector<std::string>()); 
  declareProperty("TriggerTP", m_triggerTP=std::vector<std::string>()); 
  declareProperty("Trigger2L2Q", m_trigger2L2Q=std::vector<std::string>()); 
  declareProperty("TriggerJPSI", m_triggerJPSI=std::vector<std::string>()); 
  declareProperty("TriggerPHI", m_triggerPHI=std::vector<std::string>()); 

  declareProperty("ElectronEtCut", m_electronEtCut=10.*CLHEP::GeV); 
  declareProperty("MuonPtCut", m_muonPtCut=10.*CLHEP::GeV); 
  declareProperty("TightElectronEtCut", m_tightElectronEtCut=15.*CLHEP::GeV); 
  declareProperty("TightMuonPtCut", m_tightMuonPtCut=15.*CLHEP::GeV); 
  declareProperty("JetPtCut", m_jetPtCut=15.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut0", m_mergedJetPtCut[0]=100.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut1", m_mergedJetPtCut[1]=150.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut2", m_mergedJetPtCut[2]=150.*CLHEP::GeV); 
  declareProperty("PhotonPtCut", m_photonPtCut=15.*CLHEP::GeV); 
  declareProperty("TrackPtCut", m_trackPtCut=20.*CLHEP::GeV); 

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
  declareProperty("InvariantMassPhiLowCut", m_invariantMassPhiLowCut=2.*s_MKplus); 
  declareProperty("InvariantMassPhiUpCut", m_invariantMassPhiUpCut=1.2*CLHEP::GeV); 

  declareProperty("LeadingElectronEtCut", m_leadingElectronEtCut=0.*CLHEP::GeV); 
  declareProperty("LeadingMuonPtCut", m_leadingMuonPtCut=0.*CLHEP::GeV); 

  declareProperty("DRElectronJetCut", m_dRElectronJetCut=0.05);   

  declareProperty("CheckLArError", m_checkLArError=true);	
  declareProperty("UseDefaultElectronFourMomentum", m_defaultElectronFourMomentum=true);

  declareProperty("UseDFCommonJetFourMomentum", m_DFCommonJetFourMomentum=true);

  declareProperty("RequireTightLeptons", m_requireTightLeptons=false);
}
  
// Destructor
DerivationFramework::SkimmingToolHIGG2::~SkimmingToolHIGG2() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHIGG2::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // if(m_defaultElectronFourMomentum==true and m_DFCommonElectronFourMomentum==true) {
  //   ATH_MSG_FATAL("Wrong setting both m_defaultElectronFourMomentum and m_DFCommonElectronFourMomentum are true.");
  //   return StatusCode::FAILURE;
  // }

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
  const xAOD::EventInfo *eventInfo(nullptr);
  ATH_CHECK(evtStore()->retrieve(eventInfo)); 

  // LAr error check
  if(m_checkLArError) {
    if(eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) {
      return acceptEvent; 
    }
  }
  
  // Electrons
  m_goodElectrons.clear();
  //  m_mapElectrons.clear();
  if(m_nElectrons>0 or m_nLeptons>0) {
    const xAOD::ElectronContainer *electrons(nullptr); 
    ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey));
    for(auto el: *electrons) {
      if(this->checkElectronQuality(el)) m_goodElectrons.push_back(el);
    } 
  }

  // Muons
  m_goodMuons.clear();
  if(m_nMuons>0 or m_nLeptons>0) {
    const xAOD::MuonContainer *muons(nullptr); 
    ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));
    for(auto mu: *muons) {
      if(this->checkMuonQuality(mu)) m_goodMuons.push_back(mu);
    } 
  }

  // Jets
  m_goodJets.clear();
  if(m_nJets>0) {
    const xAOD::JetContainer *jets(nullptr); 
    ATH_CHECK(evtStore()->retrieve(jets, m_jetSGKey));
    for(auto jet: *jets) {
      if(this->checkJetQuality(jet)) m_goodJets.push_back(jet);
     }
  }
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    m_goodMergedJets[type].clear();
    if(m_nMergedJets[type]>0) {
      const xAOD::JetContainer *jets(nullptr);
      ATH_CHECK(evtStore()->retrieve(jets, m_mergedJetSGKey[type]));
      for(auto jet: *jets) {
	if(this->checkMergedJetQuality(jet, type)) m_goodMergedJets[type].push_back(jet);
      }
    }
  }

  // Photons
  m_goodPhotons.clear();
  if(m_nPhotons>0) {
    const xAOD::PhotonContainer *photons(nullptr); 
    ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey));
    for(auto ph: *photons) {
      if(this->checkPhotonQuality(ph)) m_goodPhotons.push_back(ph);
    }
  }

  // Tracks
  m_goodTracks.clear();
  if(m_nTracks>0) {
    const xAOD::TrackParticleContainer *tracks(nullptr);
    ATH_CHECK(evtStore()->retrieve(tracks, m_trackSGKey));
    for(auto trk: *tracks) {
      if(this->checkTrackQuality(trk)) m_goodTracks.push_back(trk);
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
    if(this->checkJPSI() or this->checkPHI()) acceptEvent = true;
  }

  if(acceptEvent) m_npass++;
  return acceptEvent; 
}  

bool DerivationFramework::SkimmingToolHIGG2::checkElectronQuality(const xAOD::Electron *el, const bool isTight) const 
{
  if(!el) return false;

  const std::string electronQual(isTight ? m_tightElectronQual : m_electronQual);
  const double electronEtCut(isTight ? m_tightElectronEtCut : m_electronEtCut);

  if(electronQual!="any") {
    bool value(false);
    bool defined(false);
    if(electronQual=="DFCommonElectronsLHVeryLoose" or
       electronQual=="DFCommonElectronsLHLoose"     or
       electronQual=="DFCommonElectronsLHMedium"    or
       electronQual=="DFCommonElectronsLHTight"     or
       electronQual=="DFCommonElectronsML") {
      // Use Derivation Framework variable to pickup possible new correction result
      // If the same electron likelihood configuration is used in AOD creation and DAOD creation, 
      // DFCommonElectronsLHLoose and LHLoose are identical.
      if(el->isAvailable<char>(electronQual)) {
	defined = true;
	value = static_cast<bool>(el->auxdata<char>(electronQual));
      }
    } else {
      // Electron menu is defined in http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
      defined = el->passSelection(value, electronQual);
    }
    if(not(value and defined)) return false;
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

  if(et<electronEtCut) return false;
  if(fabs(eta)>m_electronEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHIGG2::checkMuonQuality(const xAOD::Muon *mu, const bool isTight) const 
{
  if(!mu) return false;

  const std::string muonQual(isTight ? m_tightMuonQual : m_muonQual);
  const double muonPtCut(isTight ? m_tightMuonPtCut : m_muonPtCut);

  if(muonQual=="any") {
    // do nothing
  } else if(muonQual=="combined") {
    if(mu->muonType()!=xAOD::Muon::Combined) return false;
  } else if(muonQual=="standalone") {
    if(mu->muonType()!=xAOD::Muon::MuonStandAlone) return false;
  } else if(muonQual=="lowpt") {
    if(mu->muonType()!=xAOD::Muon::SegmentTagged) return false;
  } else if(muonQual=="combined+lowpt") {
    if(mu->muonType()!=xAOD::Muon::Combined and mu->muonType()!=xAOD::Muon::SegmentTagged) return false;
  } else if(muonQual=="inMS") {
    if(mu->muonType()==xAOD::Muon::MuonStandAlone and fabs(fabs(mu->eta())-2.6)>0.12) return false;
  } else if(muonQual=="DFCommonGoodMuon") { // Derivation Framework variable
    if(!mu->isAvailable<char>("DFCommonGoodMuon") or !mu->auxdata<char>("DFCommonGoodMuon")) return false;
  } else if(muonQual=="DFCommonMuonsLoose") { // Derivation Framework variable
    if(!mu->isAvailable<char>("DFCommonMuonsLoose") or !mu->auxdata<char>("DFCommonMuonsLoose")) return false;
  } else if(muonQual=="DFCommonMuonsMedium") { // Derivation Framework variable
    if(!mu->isAvailable<char>("DFCommonMuonsMedium") or !mu->auxdata<char>("DFCommonMuonsMedium")) return false;
  } else if(muonQual=="DFCommonMuonsTight") { // Derivation Framework variable
    if(!mu->isAvailable<char>("DFCommonMuonsTight") or !mu->auxdata<char>("DFCommonMuonsTight")) return false;
  } else {
    ATH_MSG_WARNING("Muon quality " << muonQual << "is not defined");
    return false; 
  }

  if(mu->pt()<muonPtCut) return false;
  if(fabs(mu->eta())>m_muonEtaCut) return false;
  if(muonQual!="DFCommonGoodMuon" and
     (mu->muonType()==xAOD::Muon::CaloTagged and fabs(mu->eta())>m_caloMuonEtaCut)) return false;

  return true;
}
  
bool DerivationFramework::SkimmingToolHIGG2::checkJetQuality(const xAOD::Jet *jet) const 
{
  if(!jet) return false;

  if(m_jetQual!="any") {
    // do something
  }

  TLorentzVector tlv(this->jetFourMomentum(jet));
  if(tlv.Pt()<m_jetPtCut) return false;
  if(fabs(tlv.Eta())>m_jetEtaCut) return false;

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

bool DerivationFramework::SkimmingToolHIGG2::checkTrackQuality(const xAOD::TrackParticle *trk) const 
{
  if(!trk) return false;

  if(trk->pt()<m_trackPtCut) return false;

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

  unsigned int nGoodLeptons(m_goodElectrons.size()+m_goodMuons.size());
  std::vector<TLorentzVector> v_tlv(nGoodLeptons);
  std::vector<bool> v_isElectron(nGoodLeptons);
  std::vector<bool> v_isTight(nGoodLeptons);
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.at(el_i) = tlv;
    v_isElectron.at(el_i) = true;
    v_isTight.at(el_i) = this->checkElectronQuality(el, true);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    unsigned int mu_j(m_goodElectrons.size()+mu_i);
    v_tlv.at(mu_j) = tlv;
    v_isElectron.at(mu_j) = false;
    v_isTight.at(mu_j) = this->checkMuonQuality(mu, true);
  }
  
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      if(m_requireTightLeptons and (not (v_isTight.at(i0) or v_isTight.at(i1)))) continue; 

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

  unsigned int nGoodLeptons(m_goodElectrons.size()+m_goodMuons.size());
  std::vector<TLorentzVector> v_tlv(nGoodLeptons);
  std::vector<bool> v_pid(nGoodLeptons);
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i)); 
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.at(el_i) = tlv;

    bool value(false);
    bool defined(false);
    if(m_primaryElectronQual4L=="") {
      // In the case of no identification requirement
      defined = true;
      value = true;
    } else if(m_primaryElectronQual4L=="DFCommonElectronsLHVeryLoose" or
	      m_primaryElectronQual4L=="DFCommonElectronsLHLoose"     or
	      m_primaryElectronQual4L=="DFCommonElectronsLHMedium"    or
	      m_primaryElectronQual4L=="DFCommonElectronsLHTight"     or
	      m_primaryElectronQual4L=="DFCommonElectronsML") {
      // Use Derivation Framework variable to pickup possible new correction result
      // If the same electron likelihood configuration is used in AOD creation and DAOD creation, 
      // DFCommonElectronsLHLoose and LHLoose are identical.
      if(el->isAvailable<char>(m_primaryElectronQual4L)) {
	defined = true;
	value = static_cast<bool>(el->auxdata<char>(m_primaryElectronQual4L));
      }
    } else {
      // Electron menu is defined in http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/egamma/egammaTools/python/EMPIDBuilderBase.py
      defined = el->passSelection(value, m_primaryElectronQual4L);
    }

    v_pid.at(el_i) = (value and defined);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    unsigned int mu_j(m_goodElectrons.size()+mu_i);
    v_tlv.at(mu_j) = tlv;
    v_pid.at(mu_j) = true;
  }
  
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

  unsigned int nGoodLeptons(m_goodElectrons.size()+m_goodMuons.size());
  std::vector<TLorentzVector> v_tlv(nGoodLeptons);
  std::vector<bool> v_isElectron(nGoodLeptons);
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.at(el_i) = tlv;
    v_isElectron.at(el_i) = true;
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    unsigned int mu_j(m_goodElectrons.size()+mu_i);
    v_tlv.at(mu_j) = tlv;
    v_isElectron.at(mu_j) = false;
  }
  
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

  bool isTriggerFired(m_trigger2L2Q.size()==0 or m_skipTriggerRequirement);
  for(unsigned int i(0); i<m_trigger2L2Q.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_trigger2L2Q.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  if(!isTriggerFired) return false;

  bool checkGoodJets(m_goodJets.size()>=m_nJets and m_nJets>0);
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    if(m_nMergedJets[type]>0) {
      checkGoodJets = (checkGoodJets or (m_goodMergedJets[type].size()>=m_nMergedJets[type]));
    }
  }
  if(!checkGoodJets) return false;

  unsigned int nGoodLeptons(m_goodElectrons.size()+m_goodMuons.size());
  std::vector<TLorentzVector> v_tlv(nGoodLeptons);
  std::vector<bool> v_isElectron(nGoodLeptons);
  std::vector<bool> v_isTight(nGoodLeptons);
  
  for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
    const xAOD::Electron *el(m_goodElectrons.at(el_i));
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_tlv.at(el_i) = tlv;
    v_isElectron.at(el_i) = true;
    v_isTight.at(el_i) = this->checkElectronQuality(el, true);
  }
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    unsigned int mu_j(m_goodElectrons.size()+mu_i);
    v_tlv.at(mu_j) = tlv;
    v_isElectron.at(mu_j) = false;
    v_isTight.at(mu_j) = this->checkMuonQuality(mu, true);
  }
  
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      //      if(v_isElectron.at(i0)!=v_isElectron.at(i1)) continue; // Events with N(e)=1 and N(mu)=1 are kept. 
      if(m_requireTightLeptons and (not (v_isTight.at(i0) or v_isTight.at(i1)))) continue;
      
      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
      // Check two lepton mass
      if(tlv_2lep.M()<m_invariantMassCut) continue;

      // dR(e-jet)>0.05 is required for at least two jets
      if(v_isElectron.at(i0)) {
	unsigned int nGoodJetsWithDRCut(0);
	unsigned int nGoodJets(m_goodJets.size());
	for(unsigned int j(0); j<nGoodJets; j++) {
	  const xAOD::Jet *jet(m_goodJets.at(j));
	  TLorentzVector jet_tlv(this->jetFourMomentum(jet));

	  double dR_0(this->getDeltaR(v_tlv.at(i0).Eta(), v_tlv.at(i0).Phi(), jet_tlv.Eta(), jet_tlv.Phi()));
	  if(dR_0<m_dRElectronJetCut) continue;
	  
	  double dR_1(this->getDeltaR(v_tlv.at(i1).Eta(), v_tlv.at(i1).Phi(), jet_tlv.Eta(), jet_tlv.Phi()));
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

  std::vector<TLorentzVector> v_tlv(m_goodMuons.size());
  
  for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
    const xAOD::Muon *mu(m_goodMuons.at(mu_i));
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.at(mu_i) = tlv;
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

bool DerivationFramework::SkimmingToolHIGG2::checkPHI() const 
{
  // Check if there are candidates
  if(!(m_nTracks>0 and m_goodTracks.size()>=m_nTracks)) return false;
  if(!(m_goodPhotons.size()>=m_nPhotons)) return false;

  // Check if triggers are OK
  bool isTriggerFired(m_triggerPHI.size()==0 or m_skipTriggerRequirement);
  for(unsigned int i(0); i<m_triggerPHI.size(); i++) {
    if(m_trigDecisionTool->isPassed(m_triggerPHI.at(i))) {
      isTriggerFired = true;
      break;
    }
  }
  if(!isTriggerFired) return false; 

  // Get 4-momentum of tracks (=charged kaons)
  std::vector<TLorentzVector> v_tlv[2];
  for(unsigned int trk_i(0); trk_i<m_goodTracks.size(); trk_i++) {
    const xAOD::TrackParticle *trk(m_goodTracks.at(trk_i));
    TLorentzVector tlv;
    tlv.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MKplus); // Kaon is assumed. 
    v_tlv[trk->charge()>0. ? 0 : 1].push_back(tlv); // 0 is positive, 1 is negative
  }
  
  // Check track pair mass
  unsigned int nGoodTracks[2];
  nGoodTracks[0] = v_tlv[0].size();
  nGoodTracks[1] = v_tlv[1].size();
  for(unsigned int i0(0); i0<nGoodTracks[0]; i0++) {
    for(unsigned int i1(0); i1<nGoodTracks[1]; i1++) {
      TLorentzVector tlv_2trk(v_tlv[0].at(i0) + v_tlv[1].at(i1));
      if(tlv_2trk.M()<m_invariantMassPhiLowCut or tlv_2trk.M()>m_invariantMassPhiUpCut) continue;
      return true; // One candidate is found. 
    }
  }

  return false; // No candidate is found. 
}

TLorentzVector DerivationFramework::SkimmingToolHIGG2::electronFourMomentum(const xAOD::Electron *el) const
{
  TLorentzVector tlv;
  tlv.SetPtEtaPhiE(el->pt(), el->eta(), el->phi(), el->e());
  if(m_defaultElectronFourMomentum) return tlv;

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

TLorentzVector DerivationFramework::SkimmingToolHIGG2::jetFourMomentum(const xAOD::Jet *jet) const
{
  TLorentzVector tlv;
  if(m_DFCommonJetFourMomentum) {
    const float& pt =jet->auxdata<float> ("DFCommonJets_Calib_pt");
    const float& eta=jet->auxdata<float> ("DFCommonJets_Calib_eta");
    const float& phi=jet->auxdata<float> ("DFCommonJets_Calib_phi");
    const float& m  =jet->auxdata<float> ("DFCommonJets_Calib_m");
    tlv.SetPtEtaPhiM(pt, eta, phi, m);
  }
  else
    tlv.SetPtEtaPhiM(jet->pt(), jet->eta(), jet->phi(), jet->m());

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

const double DerivationFramework::SkimmingToolHIGG2::s_MZ(91187.6*CLHEP::MeV); 
const double DerivationFramework::SkimmingToolHIGG2::s_MKplus(493.677*CLHEP::MeV); 
