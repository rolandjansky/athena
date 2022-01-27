/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHDBS2.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Susumu Oda (Susumu.Oda@cern.ch)
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkHDBS/SkimmingToolHDBS2.h"
#include "StoreGate/ReadHandle.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::SkimmingToolHDBS2::SkimmingToolHDBS2(const std::string& t,
							  const std::string& n,
							  const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_iVertexFitter(),
  m_VKVFitter(nullptr),
  m_ntot(0),
  m_npass(0),
  m_eventInfoKey(std::string("EventInfo"))
{
  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("SkipTriggerRequirement", m_skipTriggerRequirement=false);
  declareProperty("DoVertexing", m_doVertexing=false);
  declareProperty("DoVertexing_el_withOrigInDetTrack", m_doVertexing_el_withOrigInDetTrack=true);
  declareProperty("DoVertexing_mu_withInDetForwTrack", m_doVertexing_mu_withInDetForwTrack=true);
  declareProperty("LeptonVertexContainerKey", m_vxContainerName="FourLeptonVertices");

  declareProperty("FilterType", m_filterType="2L");

  declareProperty("ElectronContainerKey", m_electronSGKey=std::string("Electrons"));
  declareProperty("MuonContainerKey", m_muonSGKey=std::string("Muons"));
  declareProperty("JetContainerKey", m_jetSGKey=std::string("AntiKt4EMTopoJets"));
  declareProperty("MergedJetContainerKey0", m_mergedJetSGKey[0]=std::string("AntiKt4EMTopoJets"));
  declareProperty("MergedJetContainerKey1", m_mergedJetSGKey[1]=std::string("AntiKt10LCTopoJets"));
  declareProperty("MergedJetContainerKey2", m_mergedJetSGKey[2]=std::string("CamKt12LCTopoJets"));
  declareProperty("PhotonContainerKey", m_photonSGKey=std::string("Photons"));
  declareProperty("TrackContainerKey", m_trackSGKey=std::string("InDetTrackParticles"));

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
  declareProperty("MuonQuality", m_muonQual="any");
  declareProperty("TightElectronQuality", m_tightElectronQual="DFCommonElectronsLHLoose");
  declareProperty("TightMuonQuality", m_tightMuonQual="DFCommonGoodMuon");
  declareProperty("VertexingElectronQuality", m_vertexingElectronQual="any");
  declareProperty("VertexingMuonQuality", m_vertexingMuonQual="inMS");
  declareProperty("JetQuality", m_jetQual="any");
  declareProperty("MergedJetQuality0", m_mergedJetQual[0]="any");
  declareProperty("MergedJetQuality1", m_mergedJetQual[1]="any");
  declareProperty("MergedJetQuality2", m_mergedJetQual[2]="any");
  declareProperty("PhotonQuality", m_photonQual="any");

  declareProperty("PrimaryElectronQuality4L", m_primaryElectronQual4L="DFCommonElectronsLHLoose");

  declareProperty("Trigger2L", m_trigger2L=std::vector<std::string>()); 
  declareProperty("Trigger4L", m_trigger4L=std::vector<std::string>()); 
  declareProperty("TriggerTP", m_triggerTP=std::vector<std::string>()); 
  declareProperty("Trigger2L2Q", m_trigger2L2Q=std::vector<std::string>()); 
  declareProperty("TriggerJPSI", m_triggerJPSI=std::vector<std::string>()); 
  declareProperty("TriggerPHI", m_triggerPHI=std::vector<std::string>()); 
  declareProperty("TriggerRHO", m_triggerRHO=std::vector<std::string>()); 
  declareProperty("TriggerWPIGAMMA", m_triggerWPIGAMMA=std::vector<std::string>()); 

  declareProperty("ElectronEtCut", m_electronEtCut=10.*CLHEP::GeV); 
  declareProperty("ElectronPtCut", m_electronPtCut=6.*CLHEP::GeV); 
  declareProperty("MuonPtCut", m_muonPtCut=10.*CLHEP::GeV); 
  declareProperty("TightElectronEtCut", m_tightElectronEtCut=15.*CLHEP::GeV); 
  declareProperty("TightMuonPtCut", m_tightMuonPtCut=15.*CLHEP::GeV); 
  declareProperty("VertexingElectronEtCut", m_vertexingElectronEtCut=5.*CLHEP::GeV); 
  declareProperty("VertexingMuonPtCut", m_vertexingMuonPtCut=4.*CLHEP::GeV); 
  declareProperty("JetPtCut", m_jetPtCut=15.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut0", m_mergedJetPtCut[0]=100.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut1", m_mergedJetPtCut[1]=150.*CLHEP::GeV); 
  declareProperty("MergedJetPtCut2", m_mergedJetPtCut[2]=150.*CLHEP::GeV); 
  declareProperty("PhotonPtCut", m_photonPtCut=15.*CLHEP::GeV); 
  declareProperty("TrackPtCut", m_trackPtCut=20.0*CLHEP::GeV); 
  declareProperty("TrackPtCutLoose", m_trackPtCutLoose=10.*CLHEP::GeV); 
  declareProperty("DiTrackPtCut", m_diTrackPtCut=20.*CLHEP::GeV); 
  declareProperty("TrackPtForWCut", m_trackPtForWCut=30.*CLHEP::GeV); 
  declareProperty("PhotonPtForWCut", m_photonPtForWCut=30.*CLHEP::GeV); 

  declareProperty("ElectronEtaCut", m_electronEtaCut=9999.); 
  declareProperty("MuonEtaCut", m_muonEtaCut=9999.); 
  declareProperty("CaloMuonEtaCut", m_caloMuonEtaCut=0.2); 
  declareProperty("JetEtaCut", m_jetEtaCut=2.6); 
  declareProperty("MergedJetEtaCut0", m_mergedJetEtaCut[0]=2.6); 
  declareProperty("MergedJetEtaCut1", m_mergedJetEtaCut[1]=2.6); 
  declareProperty("MergedJetEtaCut2", m_mergedJetEtaCut[2]=2.6); 
  declareProperty("PhotonEtaCut", m_photonEtaCut=2.5); 

  declareProperty("InvariantMassCut", m_invariantMassCut=0.*CLHEP::GeV); 
  declareProperty("InvariantMassa0LowCut", m_invariantMassa0LowCut=0.0 * Gaudi::Units::GeV); 
  declareProperty("InvariantMassa0UpCut", m_invariantMassa0UpCut=1.5 * Gaudi::Units::GeV); 
  declareProperty("InvariantMassHLowCut", m_invariantMassHLowCut=100 * Gaudi::Units::GeV); 
  declareProperty("InvariantMassZLowCut", m_invariantMassZLowCut=71 * Gaudi::Units::GeV);
  declareProperty("InvariantMassZUpCut", m_invariantMassZUpCut=111 * Gaudi::Units::GeV); 
  declareProperty("InvariantMassJpsiLowCut", m_invariantMassJpsiLowCut=2.0*CLHEP::GeV); 
  declareProperty("InvariantMassJpsiUpCut", m_invariantMassJpsiUpCut=4.3*CLHEP::GeV); 
  declareProperty("InvariantMassUpsilonLowCut",m_invariantMassUpsilonLowCut=8.0*CLHEP::GeV); 
  declareProperty("InvariantMassUpsilonUpCut", m_invariantMassUpsilonUpCut=12.0*CLHEP::GeV); 
  declareProperty("InvariantMassPhiLowCut",    m_invariantMassPhiLowCut=2.*s_MKplus); 
  declareProperty("InvariantMassPhiUpCut",     m_invariantMassPhiUpCut=1.2*CLHEP::GeV); 
  declareProperty("InvariantMassRhoLowCut",    m_invariantMassRhoLowCut=2.*s_MPiplus);
  declareProperty("InvariantMassRhoUpCut",     m_invariantMassRhoUpCut=1.4*CLHEP::GeV);
  declareProperty("InvariantMassKshortLowCut", m_invariantMassKshortLowCut=0.400*CLHEP::GeV);
  declareProperty("InvariantMassKshortUpCut",  m_invariantMassKshortUpCut=0.600*CLHEP::GeV);
  declareProperty("InvariantMassKstarLowCut",  m_invariantMassKstarLowCut=s_MPiplus+s_MKplus);
  declareProperty("InvariantMassKstarUpCut",   m_invariantMassKstarUpCut=1.5*CLHEP::GeV);
  declareProperty("InvariantMassD0LowCut",     m_invariantMassD0LowCut=1.5*CLHEP::GeV);
  declareProperty("InvariantMassD0UpCut",      m_invariantMassD0UpCut=2.2*CLHEP::GeV);
  declareProperty("InvariantMassWLowCut",      m_invariantMassWLowCut=50.*CLHEP::GeV);

  declareProperty("LeadingElectronEtCut", m_leadingElectronEtCut=0.*CLHEP::GeV); 
  declareProperty("LeadingElectronPtCut", m_leadingElectronPtCut=0.*CLHEP::GeV);
  declareProperty("LeadingMuonPtCut", m_leadingMuonPtCut=0.*CLHEP::GeV); 
  declareProperty("DiMuonPtCut", m_diMuonPtCut=20.*CLHEP::GeV); 

  declareProperty("DRElectronJetCut", m_dRElectronJetCut=0.05);   

  declareProperty("CheckEventError", m_checkEventError=true);	
  declareProperty("UseDefaultElectronFourMomentum", m_defaultElectronFourMomentum=true);

  declareProperty("UseDFCommonJetFourMomentum", m_DFCommonJetFourMomentum=true);

  declareProperty("RequireTightLeptons", m_requireTightLeptons=false);

  declareProperty("VertexFitter", m_iVertexFitter);
}
  
// Destructor
DerivationFramework::SkimmingToolHDBS2::~SkimmingToolHDBS2() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHDBS2::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // trigger decision tool
  if(m_trigDecisionTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);

  if(m_doVertexing) {
    if(m_iVertexFitter.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_iVertexFitter);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool: " << m_iVertexFitter);
    m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVertexFitter));
  }

  // Read Handle Key
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_electronSGKey.initialize());//m_nElectrons>0 or m_nLeptons>0));
  ATH_CHECK(m_muonSGKey.initialize());//m_nMuons>0 or m_nLeptons>0));
  ATH_CHECK(m_jetSGKey.initialize());//m_nJets>0));
  ATH_CHECK(m_mergedJetSGKey[0].initialize());//m_nMergedJets[0]>0));
  ATH_CHECK(m_mergedJetSGKey[1].initialize());//m_nMergedJets[1]>0));
  ATH_CHECK(m_mergedJetSGKey[2].initialize());//m_nMergedJets[2]>0));
  ATH_CHECK(m_photonSGKey.initialize());//m_nPhotons>0));
  ATH_CHECK(m_trackSGKey.initialize());//m_nTracks>0));
  
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolHDBS2::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHDBS2::eventPassesFilter() const
{
  m_ntot++;
  bool acceptEvent(false);

  // Retrieve EventInfo
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  		
  // Event error check
  // https://twiki.cern.ch/twiki/bin/view/Atlas/DataPreparationCheckListForPhysicsAnalysis#Rejection_of_bad_corrupted_event
  // https://twiki.cern.ch/twiki/bin/view/Atlas/DataPreparationCheckListForPhysicsAnalysis#CoreFlags_Checks_in_Run_2
  if(m_checkEventError and eventInfo.isValid()) {
    if(eventInfo->errorState(xAOD::EventInfo::LAr) ==xAOD::EventInfo::Error // or
       //       eventInfo->errorState(xAOD::EventInfo::Tile)==xAOD::EventInfo::Error or
       //       eventInfo->errorState(xAOD::EventInfo::SCT) ==xAOD::EventInfo::Error or
       //       (eventInfo->eventFlags(xAOD::EventInfo::Core) & 0x40000)
       ) {
      return acceptEvent;
    }
  }
  
  // Electrons
  this->fillElectrons();
  // Muons
  this->fillMuons();
  // Jets
  this->fillJets();
  this->fillMergedJets();
  // Photons
  this->fillPhotons();
  // Tracks
  this->fillTracks();

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
    if(this->checkJPSI() or this->checkPHI() or this->checkRHO() 
       or this->checkWPIGAMMA()) acceptEvent = true;
  } else if(m_filterType=="2L2TRK") { 
    if(this->check2L2TRK()) acceptEvent = true;
  }
  
  if(acceptEvent) m_npass++;

  return acceptEvent; 
}  

StatusCode DerivationFramework::SkimmingToolHDBS2::addBranches() const 
{
  StatusCode sc(StatusCode::SUCCESS, true);

  if(not m_doVertexing) return sc;

  if(evtStore()->contains<xAOD::VertexContainer>(m_vxContainerName) or 
     evtStore()->contains<xAOD::VertexAuxContainer>(m_vxContainerName+"Aux.")) {
    ATH_MSG_DEBUG(m_vxContainerName << " is already available");
    return sc;
  }

  // Electrons
  this->fillElectrons();
  // Muons
  this->fillMuons();

  // Check conditions of filters
  if(m_filterType=="2L") {
    this->check2L(true);
  } else if(m_filterType=="4L") {
    this->check4L(true);
  }

  return sc; 
}

void DerivationFramework::SkimmingToolHDBS2::fillElectrons() const
{
  m_goodElectrons.clear();
  if(m_nElectrons>0 or m_nLeptons>0) {
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronSGKey);
    if(electrons.isValid()) {
      for(auto el: *electrons) {
	if(this->checkElectronQuality(el)) m_goodElectrons.push_back(el);
      }
    }
  }
}

void DerivationFramework::SkimmingToolHDBS2::fillMuons() const
{
  m_goodMuons.clear();
  if(m_nMuons>0 or m_nLeptons>0) {
    SG::ReadHandle<xAOD::MuonContainer> muons(m_muonSGKey);
    if(muons.isValid()) {
      for(auto mu: *muons) {
	if(this->checkMuonQuality(mu)) m_goodMuons.push_back(mu);
      }
    }
  }
}

void DerivationFramework::SkimmingToolHDBS2::fillJets() const
{
  m_goodJets.clear();
  if(m_nJets>0) {
    SG::ReadHandle<xAOD::JetContainer> jets(m_jetSGKey);
    if(jets.isValid()) {
      for(auto jet: *jets) {
	if(this->checkJetQuality(jet)) m_goodJets.push_back(jet);
      }
    }
  }
}

void DerivationFramework::SkimmingToolHDBS2::fillMergedJets() const
{
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    m_goodMergedJets[type].clear();
    if(m_nMergedJets[type]>0) {
      SG::ReadHandle<xAOD::JetContainer> jets(m_mergedJetSGKey[type]);
      if(jets.isValid()) {
	for(auto jet: *jets) {
	  if(this->checkMergedJetQuality(jet, type)) m_goodMergedJets[type].push_back(jet);
	}
      }
    }
  }
}

void DerivationFramework::SkimmingToolHDBS2::fillPhotons() const
{
  m_goodPhotons.clear();
  m_goodPhotonsForW.clear();
  if(m_nPhotons>0) {
    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonSGKey);
    if(photons.isValid()) {
      for(auto ph: *photons) {
	if(this->checkPhotonQuality(ph)) m_goodPhotons.push_back(ph); 
	if(this->checkPhotonQualityForW(ph)) m_goodPhotonsForW.push_back(ph);
     }
    }
  }
}

void DerivationFramework::SkimmingToolHDBS2::fillTracks() const
{
  m_goodTracks.clear();
  m_goodTracksLoose.clear();
  m_goodTracksForW.clear();
  if(m_nTracks>0) {
    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_trackSGKey);
    if(tracks.isValid()) {
      for(auto trk: *tracks) {
	if(this->checkTrackQuality(trk)) m_goodTracks.push_back(trk);
	if(this->checkTrackQualityLoose(trk)) m_goodTracksLoose.push_back(trk);
	if(this->checkTrackQualityForW(trk)) m_goodTracksForW.push_back(trk);
      }
    }
  }
}

bool DerivationFramework::SkimmingToolHDBS2::checkElectronQuality(const xAOD::Electron *el, const bool isTight, const bool isVertexing) const 
{
  if(!el) return false;

  const std::string electronQual(isTight ? m_tightElectronQual : (isVertexing ? m_vertexingElectronQual : m_electronQual));
  const double electronEtCut(isTight ? m_tightElectronEtCut : (isVertexing ? m_vertexingElectronEtCut : m_electronEtCut));

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
  if(not trackParticle->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits)) {
    ATH_MSG_WARNING("xAOD::TrackParticle does not give summaryValue correctly for xAOD::numberOfPixelHits");
    return false;
  }
  uint8_t numberOfSCTHits(0);
  if(not trackParticle->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits)) {
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

bool DerivationFramework::SkimmingToolHDBS2::checkMuonQuality(const xAOD::Muon *mu, const bool isTight, const bool isVertexing) const 
{
  if(!mu) return false;

  const std::string muonQual(isTight ? m_tightMuonQual : (isVertexing ? m_vertexingMuonQual : m_muonQual));
  const double muonPtCut(isTight ? m_tightMuonPtCut : (isVertexing ? m_vertexingMuonPtCut : m_muonPtCut));

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
  } else if(muonQual=="DFCommonMuonsPreselection") { // Derivation Framework variable
    if(!mu->isAvailable<char>("DFCommonMuonsPreselection") or !mu->auxdata<char>("DFCommonMuonsPreselection")) return false;
  } else {
    ATH_MSG_WARNING("Muon quality " << muonQual << "is not defined");
    return false; 
  }

  const xAOD::TrackParticle* tp(mu->trackParticle(xAOD::Muon::Primary));
  if(mu->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon) {
    tp = mu->trackParticle(xAOD::Muon::CombinedTrackParticle); //hack needed because primary track link is wrong for Si-assoc. muons.
  }
  if(!tp) return false;
  if(tp->pt()<muonPtCut) return false;
  if(fabs(tp->eta())>m_muonEtaCut) return false;
  if((muonQual!="DFCommonGoodMuon" and muonQual!="DFCommonMuonsPreselection") and
     (mu->muonType()==xAOD::Muon::CaloTagged and fabs(mu->eta())>m_caloMuonEtaCut)) return false;
  
  return true;
}
  
bool DerivationFramework::SkimmingToolHDBS2::checkJetQuality(const xAOD::Jet *jet) const 
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

bool DerivationFramework::SkimmingToolHDBS2::checkMergedJetQuality(const xAOD::Jet *jet, const unsigned int type) const 
{
  if(!jet) return false;

  if(m_mergedJetQual[type]!="any") {
    // do something
  }

  if(jet->pt()<m_mergedJetPtCut[type]) return false;
  if(fabs(jet->eta())>m_mergedJetEtaCut[type]) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::checkPhotonQuality(const xAOD::Photon *ph) const 
{
  if(!ph) return false;

  if(m_photonQual!="any") {
    bool value(false);
    if(!ph->passSelection(value, m_photonQual)) {
      ATH_MSG_WARNING("xAOD::Photon does not have menu of " << m_photonQual);
      return false;
    } 
    if(not value) return false;
  }

  if(ph->pt()<m_photonPtCut) return false;
  if(fabs(ph->eta())>m_photonEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::checkPhotonQualityForW(const xAOD::Photon *ph) const 
{
  if(!ph) return false;

  if(m_photonQual!="any") {
    bool value(false);
    if(!ph->passSelection(value, m_photonQual)) {
      ATH_MSG_WARNING("xAOD::Photon does not have menu of " << m_photonQual);
      return false;
    } 
    if(not value) return false;
  }

  if(ph->pt()<m_photonPtForWCut) return false;
  if(fabs(ph->eta())>m_photonEtaCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::checkTrackQualityLoose(const xAOD::TrackParticle *trk) const 
{
  if(!trk) return false;

  if(trk->pt()<m_trackPtCutLoose) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::checkTrackQuality(const xAOD::TrackParticle *trk) const 
{
  if(!trk) return false;

  if(trk->pt()<m_trackPtCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::checkTrackQualityForW(const xAOD::TrackParticle *trk) const 
{
  if(!trk) return false;
  if(trk->pt()<m_trackPtForWCut) return false;

  return true;
}

bool DerivationFramework::SkimmingToolHDBS2::check2L(const bool onlyVertexing) const 
{
  if(not onlyVertexing) {
    if(not (m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;
    if(not (m_goodJets.size()>=m_nJets and m_goodPhotons.size()>=m_nPhotons)) return false;

    bool isTriggerFired(m_trigger2L.size()==0 or m_skipTriggerRequirement);
    for(auto trigger: m_trigger2L) {
      if(m_trigDecisionTool->isPassed(trigger)) {
	isTriggerFired = true;
	break;
      }
    }
    if(not isTriggerFired) return false; 
  }

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

  bool passMassCut(onlyVertexing);
  if(not onlyVertexing) {
    for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
      for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
	if(m_requireTightLeptons and (not (v_isTight.at(i0) or v_isTight.at(i1)))) continue; 

	TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
	// Check two lepton mass
	if(tlv_2lep.M()>m_invariantMassCut) {
	  passMassCut = true;
	  break;
	}
      }
    }
  }

  if(onlyVertexing or passMassCut) {
    // Select good leptons for quad vertexing 
    std::vector<const xAOD::IParticle*> goodLeptons;
  
    // Electrons
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronSGKey);
    if(electrons.isValid()) {
      for(auto el: *electrons) {
	if(this->checkElectronQuality(el, false, true)) goodLeptons.push_back(el);
      }
    }
    // Muons
    SG::ReadHandle<xAOD::MuonContainer> muons(m_muonSGKey);
    if(muons.isValid()) {
      for(auto mu: *muons) {
	if(this->checkMuonQuality(mu, false, true)) goodLeptons.push_back(mu);
      }
    }
    std::vector<std::vector<const xAOD::IParticle*>*>* quads(new std::vector<std::vector<const xAOD::IParticle*>*>);
    if(this->formQuads(&goodLeptons, true, quads)) ATH_MSG_DEBUG("Found " << quads->size() << " lepton quads");
    this->doVertexing(quads);
    for(auto q: *quads) delete q;
    quads->clear();
    delete quads;
  }
  
  return passMassCut;
}

bool DerivationFramework::SkimmingToolHDBS2::check4L(const bool onlyVertexing) const 
{
  if(not onlyVertexing) {
    if(not (m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;
    if(not (m_goodJets.size()>=m_nJets and m_goodPhotons.size()>=m_nPhotons)) return false;
    
    bool isTriggerFired(m_trigger4L.size()==0 or m_skipTriggerRequirement);
    for(auto trigger: m_trigger4L) {
      if(m_trigDecisionTool->isPassed(trigger)) {
	isTriggerFired = true;
	break;
      }
    }
    if(not isTriggerFired) return false; 
  }

  std::vector<const xAOD::IParticle*> goodLeptons;
  goodLeptons.reserve(m_goodElectrons.size()+m_goodMuons.size());
  goodLeptons.insert(goodLeptons.end(), m_goodElectrons.begin(), m_goodElectrons.end());
  goodLeptons.insert(goodLeptons.end(), m_goodMuons.begin(), m_goodMuons.end());

  bool acceptEvent(onlyVertexing);
  if(not onlyVertexing) {
    acceptEvent = this->formQuads(&goodLeptons, false);
  }

  if(onlyVertexing or acceptEvent) {
    std::vector<std::vector<const xAOD::IParticle*>*>* quads(new std::vector<std::vector<const xAOD::IParticle*>*>);
    this->formQuads(&goodLeptons, true, quads);
    this->doVertexing(quads);
    for(auto q: *quads) delete q;
    quads->clear();
    delete quads;
  }
 
  return acceptEvent;
}

bool DerivationFramework::SkimmingToolHDBS2::checkTP() const 
{
  if(not (m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;

  bool isTriggerFired(m_triggerTP.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_triggerTP) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false; 

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

bool DerivationFramework::SkimmingToolHDBS2::check2L2Q() const 
{
  if(not (m_nLeptons>0 and m_goodElectrons.size()+m_goodMuons.size()>=m_nLeptons)) return false;

  bool isTriggerFired(m_trigger2L2Q.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_trigger2L2Q) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false;

  bool checkGoodJets(m_goodJets.size()>=m_nJets and m_nJets>0);
  for(unsigned int type(0); type<NUMBER_OF_MERGED_JET_TYPES; type++) {
    if(m_nMergedJets[type]>0) {
      checkGoodJets = (checkGoodJets or (m_goodMergedJets[type].size()>=m_nMergedJets[type]));
    }
  }
  if(not checkGoodJets) return false;

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
	if(not checkGoodJetsWithDRCut) return false; 
      }

      // Leading lepton ET/pT cut
      if((v_tlv.at(i0).Pt()>v_tlv.at(i1).Pt() ? v_tlv.at(i0).Pt() : v_tlv.at(i1).Pt()) > (v_isElectron.at(i0) ? m_leadingElectronEtCut : m_leadingMuonPtCut)) return true;
    }
  }

  return false;
}

bool DerivationFramework::SkimmingToolHDBS2::check2L2TRK() const
{
  // check if there are candidates 
  if(not (m_nElectrons>0 and m_goodElectrons.size() >= m_nElectrons) and not (m_nMuons>0 and m_goodMuons.size() >= m_nMuons)) return false;
  if(not (m_nTracks>0 and m_goodTracks.size()>=m_nTracks)) return false;

  std::vector<TLorentzVector> v_electrons;
  std::vector<int> eleCharge;
  std::vector<TLorentzVector> v_muons;
  std::vector<int> muCharge;
  
  //for(unsigned int el_i(0); el_i<m_goodElectrons.size(); el_i++) {
  for(auto el_i: m_goodElectrons){
    const xAOD::Electron *el(el_i);
    if (el->pt() < m_electronPtCut ) continue;
    TLorentzVector tlv(this->electronFourMomentum(el));
    v_electrons.push_back(tlv);
    eleCharge.push_back(el->charge());
  }
  
  //for(unsigned int mu_i(0); mu_i<m_goodMuons.size(); mu_i++) {
  for(auto mu_i: m_goodMuons){
    const xAOD::Muon *mu(mu_i);
    if (mu->pt() < m_muonPtCut ) continue;
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_muons.push_back(tlv);
    muCharge.push_back(mu->charge());
  }

  // Get 4-momentum of tracks (=charged pions)
  std::vector<TLorentzVector> v_tlv_trk;
  for(auto trk: m_goodTracks) {
    TLorentzVector tlv1;
    tlv1.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MPiplus); // Pion is assumed. 
    if(trk->pt() < m_trackPtCut) continue;
    v_tlv_trk.push_back(tlv1);
  }

  bool muons_pass_lead_pt=false;
  bool muons_pass_Z=false;
  bool electrons_pass_Z=false;
  bool electrons_pass_lead_pt=false;
  bool tracks_pass_a0=false;
  bool H_pass_mass=false;
  std::vector<TLorentzVector> v_muonsPass;
  std::vector<TLorentzVector> v_electronsPass;
  std::vector<TLorentzVector> v_ditracksPass;

  for(unsigned int t0(0); t0< v_tlv_trk.size(); t0++) {
    for(unsigned int t1(t0+1); t1<v_tlv_trk.size(); t1++) {
      TLorentzVector tlv_2trk(v_tlv_trk.at(t0) + v_tlv_trk.at(t1));
      if(tlv_2trk.M()<m_invariantMassa0LowCut or tlv_2trk.M()>m_invariantMassa0UpCut) continue;
      tracks_pass_a0=true;
      v_ditracksPass.push_back(tlv_2trk);
    }
  }

  for(unsigned int i0(0); i0<v_electrons.size(); i0++) {
    v_electronsPass.push_back(v_electrons.at(i0));
    for(unsigned int i1(i0+1); i1<v_electrons.size(); i1++) {
      if(v_electrons.at(i1).Pt() < m_leadingElectronPtCut and v_electrons.at(i0).Pt() < m_leadingElectronPtCut) continue;
      electrons_pass_lead_pt=true;
      if(eleCharge.at(i0)*eleCharge.at(i1)>0) continue;
      TLorentzVector tlv_2ele(v_electrons.at(i0) + v_electrons.at(i1));
      if(tlv_2ele.M() < m_invariantMassZLowCut or tlv_2ele.M() > m_invariantMassZUpCut) continue;
      electrons_pass_Z=true;
      for(unsigned int t0(0); t0<v_ditracksPass.size(); t0++) {
        TLorentzVector v_H(v_ditracksPass.at(t0) + v_electrons.at(i0) + v_electrons.at(i1));
        if (v_H.M() < m_invariantMassHLowCut) continue; 
        H_pass_mass = true;
      }
    }
  }

  for(unsigned int i0(0); i0<v_muons.size(); i0++) {
    v_muonsPass.push_back(v_muons.at(i0));
    for(unsigned int i1(i0+1); i1<v_muons.size(); i1++) {
      if(v_muons.at(i1).Pt() < m_leadingMuonPtCut and v_muons.at(i0).Pt() < m_leadingMuonPtCut) continue;
      muons_pass_lead_pt=true;
      if(muCharge.at(i0)*muCharge.at(i1)>0) continue;
      TLorentzVector tlv_2mu(v_muons.at(i0) + v_muons.at(i1));
      if(tlv_2mu.M() < m_invariantMassZLowCut or tlv_2mu.M() > m_invariantMassZUpCut) continue;
      muons_pass_Z=true;
      //for(unsigned int t0(0); t0<v_ditracksPass.size(); t0++) {
      for(auto t0: v_ditracksPass){
        TLorentzVector v_H(t0 + v_muons.at(i0) + v_muons.at(i1));
        if (v_H.M() < m_invariantMassHLowCut)  continue;
        H_pass_mass = true;
      }
    }
  }

  bool acceptEvent = false;
  bool electrons_pass=false;
  bool muons_pass=false;
  if(electrons_pass_lead_pt and electrons_pass_Z) electrons_pass=true;
  if(muons_pass_lead_pt and muons_pass_Z) muons_pass=true;

  bool tracks_pass=false;
  if(v_ditracksPass.size() > 0 and tracks_pass_a0) tracks_pass=true; 

  if((electrons_pass or muons_pass) and tracks_pass and H_pass_mass) {acceptEvent=true;}    

  return acceptEvent;
}

bool DerivationFramework::SkimmingToolHDBS2::checkJPSI() const 
{
  if(not (m_nMuons>0 and m_goodMuons.size()>=m_nMuons)) return false;
  if(not (m_goodPhotons.size()>=m_nPhotons)) return false;

  bool isTriggerFired(m_triggerJPSI.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_triggerJPSI) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false; 

  std::vector<TLorentzVector> v_tlv(m_goodMuons.size());
  
  for(auto mu: m_goodMuons) {
    TLorentzVector tlv(this->muonFourMomentum(mu));
    v_tlv.push_back(tlv);
  }
  
  unsigned int nGoodLeptons(v_tlv.size());
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {

      // Check leading muon pT
      if(v_tlv.at(i0).Pt()< m_leadingMuonPtCut and v_tlv.at(i1).Pt()<m_leadingMuonPtCut)
	continue;

      TLorentzVector tlv_2lep(v_tlv.at(i0) + v_tlv.at(i1));
      // Check di-muon pT
      if(tlv_2lep.Pt()< m_diMuonPtCut)
	continue;

      // Check di-muon mass
      if((tlv_2lep.M()<m_invariantMassJpsiLowCut    or tlv_2lep.M()>m_invariantMassJpsiUpCut) and
	 (tlv_2lep.M()<m_invariantMassUpsilonLowCut or tlv_2lep.M()>m_invariantMassUpsilonUpCut)) 
	continue;

      return true;

    }
  }

  return false;
}

bool DerivationFramework::SkimmingToolHDBS2::checkPHI() const 
{
  // Check if there are candidates
  if(not (m_nTracks>0 and m_goodTracks.size()>=m_nTracks)) return false;
  if(not (m_goodPhotons.size()>=m_nPhotons)) return false;

  // Check if triggers are OK
  bool isTriggerFired(m_triggerPHI.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_triggerPHI) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false; 

  // Get 4-momentum of tracks (=charged kaons)
  std::vector<TLorentzVector> v_tlv;
  v_tlv.reserve(m_goodTracks.size());
  for(auto trk: m_goodTracks) {
    TLorentzVector tlv;
    tlv.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MKplus); // Kaon is assumed. 
    v_tlv.push_back(tlv);
  }
  
  // Check track pair mass
  unsigned int nGoodTracks;
  nGoodTracks = v_tlv.size();
  for(unsigned int i0(0); i0<nGoodTracks; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodTracks; i1++) {
      TLorentzVector tlv_2trk(v_tlv.at(i0) + v_tlv.at(i1));
      if(tlv_2trk.M()<m_invariantMassPhiLowCut or tlv_2trk.M()>m_invariantMassPhiUpCut) continue;
      return true; // One candidate is found. 
    }
  }

  return false; // No candidate is found. 
}

bool DerivationFramework::SkimmingToolHDBS2::checkRHO() const 
{
  // Check if there are candidates
  if(not (m_nTracks>0 and m_goodTracksLoose.size()>=m_nTracks)) return false;
  if(not (m_goodPhotons.size()>=m_nPhotons)) return false;

  // Check if triggers are OK
  bool isTriggerFired(m_triggerRHO.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_triggerRHO) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false; 

  // Get 4-momentum of tracks (=charged pions)
  std::vector<TLorentzVector> v_tlv1,v_tlv2;
  v_tlv1.reserve(m_goodTracksLoose.size());
  v_tlv2.reserve(m_goodTracksLoose.size());
  for(auto trk: m_goodTracksLoose) {
    TLorentzVector tlv1;
    tlv1.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MPiplus); // Pion is assumed. 
    v_tlv1.push_back(tlv1);
    TLorentzVector tlv2;
    tlv2.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MKplus); // Pion is assumed. 
    v_tlv2.push_back(tlv2);
  }
  
  // Check track pair mass
  unsigned int nGoodTracksLoose;
  nGoodTracksLoose = v_tlv1.size();
  for(unsigned int i0(0); i0<nGoodTracksLoose; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodTracksLoose; i1++) {
      // require at least one high-pT track
      if(v_tlv1.at(i0).Pt()< m_trackPtCut and v_tlv1.at(i1).Pt()< m_trackPtCut)
	continue;

      TLorentzVector tlv_2trkRho(v_tlv1.at(i0) + v_tlv1.at(i1));
      // require the di-track pt 
      if(tlv_2trkRho.Pt()<m_diTrackPtCut)
	continue;

      if((tlv_2trkRho.M()<m_invariantMassRhoLowCut or tlv_2trkRho.M()>m_invariantMassRhoUpCut) and
	 (tlv_2trkRho.M()<m_invariantMassKshortLowCut or tlv_2trkRho.M()>m_invariantMassKshortUpCut))
	continue;

      TLorentzVector tlv_2trkA(v_tlv1.at(i0) + v_tlv2.at(i1));
      TLorentzVector tlv_2trkB(v_tlv2.at(i0) + v_tlv1.at(i1));
      if((tlv_2trkA.M()<m_invariantMassKstarLowCut or tlv_2trkA.M()>m_invariantMassKstarUpCut) and 
	 (tlv_2trkB.M()<m_invariantMassKstarLowCut or tlv_2trkB.M()>m_invariantMassKstarUpCut))
	continue;

      if((tlv_2trkA.M()<m_invariantMassD0LowCut or tlv_2trkA.M()>m_invariantMassD0UpCut) and 
	 (tlv_2trkB.M()<m_invariantMassD0LowCut or tlv_2trkB.M()>m_invariantMassD0UpCut))
	continue;

      return true; // One candidate is found. 
    }
  }
  return false; // No candidate is found. 
}
 
bool DerivationFramework::SkimmingToolHDBS2::checkWPIGAMMA() const 
{
  // Check if there are candidates
  if(not (m_nTracks>0 and m_goodTracksForW.size()>0)) return false;
  if(not (m_goodPhotonsForW.size()>=m_nPhotons)) return false;

  // Check if triggers are OK
  bool isTriggerFired(m_triggerWPIGAMMA.size()==0 or m_skipTriggerRequirement);
  for(auto trigger: m_triggerWPIGAMMA) {
    if(m_trigDecisionTool->isPassed(trigger)) {
      isTriggerFired = true;
      break;
    }
  }
  if(not isTriggerFired) return false; 

  // Get 4-momentum of tracks (=charged pions)
  std::vector<TLorentzVector> v_tlv;
  v_tlv.reserve(m_goodTracksForW.size());
  for(auto trk: m_goodTracksForW) {
    TLorentzVector tlv;
    tlv.SetPtEtaPhiM(trk->pt(), trk->eta(), trk->phi(), s_MPiplus); // Pion is assumed. 
    v_tlv.push_back(tlv);
  }

  // Get 4-momentum of photons
  std::vector<TLorentzVector> p_tlv;
  p_tlv.reserve(m_goodPhotonsForW.size());
  for(auto phot: m_goodPhotonsForW) {
    TLorentzVector tlv;
    tlv.SetPtEtaPhiM(phot->pt(), phot->eta(), phot->phi(), 0.);
    p_tlv.push_back(tlv);
  }

  // Check track +photon pair mass
  unsigned int nGoodTracks;
  nGoodTracks = v_tlv.size();
  unsigned int nGoodPhotons;
  nGoodPhotons= p_tlv.size();
  for(unsigned int i0(0); i0<nGoodTracks; i0++) {
    for(unsigned int i1(0); i1<nGoodPhotons; i1++) {
      
      // remove if photon+track closer than 40 degrees in DPHI
      if(cos(v_tlv.at(i0).DeltaPhi(p_tlv.at(i1)))>0.766)
	continue;

      TLorentzVector tlv_piGamma(v_tlv.at(i0) + p_tlv.at(i1));
      if(tlv_piGamma.M()<m_invariantMassWLowCut)
	continue;
      return true; // One candidate is found. 
    }
  }
  return false; // No candidate is found. 
}

TLorentzVector DerivationFramework::SkimmingToolHDBS2::electronFourMomentum(const xAOD::Electron *el) const
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

TLorentzVector DerivationFramework::SkimmingToolHDBS2::muonFourMomentum(const xAOD::Muon *mu) const
{
  TLorentzVector tlv;
  const xAOD::TrackParticle* tp(mu->trackParticle(xAOD::Muon::Primary));
  if(mu->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon) {
    tp = mu->trackParticle(xAOD::Muon::CombinedTrackParticle); //hack needed because primary track link is wrong for Si-assoc. muons.
  }
  tlv.SetPtEtaPhiM(tp->pt(), tp->eta(), tp->phi(), mu->m());
  return tlv;
}

TLorentzVector DerivationFramework::SkimmingToolHDBS2::jetFourMomentum(const xAOD::Jet *jet) const
{
  TLorentzVector tlv;
  if(m_DFCommonJetFourMomentum) {
    const float& pt(jet->auxdata<float>("DFCommonJets_Calib_pt"));
    const float& eta(jet->auxdata<float>("DFCommonJets_Calib_eta"));
    const float& phi(jet->auxdata<float>("DFCommonJets_Calib_phi"));
    const float& m(jet->auxdata<float>("DFCommonJets_Calib_m"));
    tlv.SetPtEtaPhiM(pt, eta, phi, m);
  } else {
    tlv.SetPtEtaPhiM(jet->pt(), jet->eta(), jet->phi(), jet->m());
  }

  return tlv;
}

double DerivationFramework::SkimmingToolHDBS2::getDeltaR(const double eta1, const double phi1, const double eta2, const double phi2) const 
{
  double dEta(eta1 - eta2);
  double dPhi(phi1 - phi2);
  if(dPhi>+M_PI) dPhi -= 2.*M_PI;
  if(dPhi<-M_PI) dPhi += 2.*M_PI;
  double dR(sqrt(dEta*dEta + dPhi*dPhi));
  return dR;
}

bool DerivationFramework::SkimmingToolHDBS2::formQuads(std::vector<const xAOD::IParticle*> *goodLeptons, bool doAllCombinations, std::vector<std::vector<const xAOD::IParticle*> *>* quads) const
{
  ATH_MSG_DEBUG("formQuads with leptons: " << goodLeptons->size());
  unsigned int nGoodLeptons(goodLeptons->size());
  if(nGoodLeptons<4) return false;

  std::vector<TLorentzVector> v_tlv(nGoodLeptons);
  std::vector<bool> v_pid(nGoodLeptons);

  for(unsigned int i(0); i<nGoodLeptons; i++) {
    if(goodLeptons->at(i)->type()==xAOD::Type::Electron) {
      const xAOD::Electron* el(static_cast<const xAOD::Electron*>(goodLeptons->at(i)));
      TLorentzVector tlv(this->electronFourMomentum(el));
      v_tlv.at(i) = tlv;

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
      
      v_pid.at(i) = (value and defined);
    } else if(goodLeptons->at(i)->type()==xAOD::Type::Muon) {
      const xAOD::Muon *mu(static_cast<const xAOD::Muon*>(goodLeptons->at(i)));
      TLorentzVector tlv(this->muonFourMomentum(mu));
      v_tlv.at(i) = tlv;
      v_pid.at(i) = true;
    } else {
      ATH_MSG_ERROR("unknown particle type while expecting a lepton: "<< goodLeptons->at(i)->type());
    }
  }
  
  for(unsigned int i0(0); i0<nGoodLeptons; i0++) {
    for(unsigned int i1(i0+1); i1<nGoodLeptons; i1++) {
      for(unsigned int i2(i1+1); i2<nGoodLeptons; i2++) {
	for(unsigned int i3(i2+1); i3<nGoodLeptons; i3++) {
	  TLorentzVector tlv_4lep(v_tlv.at(i0) + v_tlv.at(i1) + v_tlv.at(i2) + v_tlv.at(i3));
	  // Check four lepton mass
	  if(tlv_4lep.M()<m_invariantMassCut) continue;

	  // Check primary dilepton's electron PID (muon PID is always true)
	  bool hasSameFlavZ1(false);
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i1)).M()-s_MZ)<fabs((v_tlv.at(i2)+v_tlv.at(i3)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i1)) hasSameFlavZ1 = true; 
	  } else {
	    if(v_pid.at(i2) and v_pid.at(i3)) hasSameFlavZ1 = true;
	  }
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i2)).M()-s_MZ)<fabs((v_tlv.at(i1)+v_tlv.at(i3)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i2)) hasSameFlavZ1 = true;
	  } else {
	    if(v_pid.at(i1) and v_pid.at(i3)) hasSameFlavZ1 = true;
	  }
	  if(fabs((v_tlv.at(i0)+v_tlv.at(i3)).M()-s_MZ)<fabs((v_tlv.at(i1)+v_tlv.at(i2)).M()-s_MZ)) {
	    if(v_pid.at(i0) and v_pid.at(i3)) hasSameFlavZ1 = true;
	  } else {
	    if(v_pid.at(i1) and v_pid.at(i2)) hasSameFlavZ1 = true;
	  }
	  if(not hasSameFlavZ1) continue; //not a good quad
	  if(not doAllCombinations) {
	    return true; //got a good quad	  
	  } else { //keep all quads (e.g. for vertexing)
	    std::vector<const xAOD::IParticle*> *quad(new std::vector<const xAOD::IParticle*>);
	    quad->push_back(goodLeptons->at(i0));
	    quad->push_back(goodLeptons->at(i1));
	    quad->push_back(goodLeptons->at(i2));
	    quad->push_back(goodLeptons->at(i3));
	    quads->push_back(quad);
	  }	  
	}
      }
    }
  }
  
  return (doAllCombinations) ? (quads->size()>0) : false;
}

void DerivationFramework::SkimmingToolHDBS2::doVertexing(std::vector< std::vector<const xAOD::IParticle*> *> *quads) const 
{
  ATH_MSG_DEBUG("fitting vertices for quads: " << quads->size());

  if(evtStore()->contains<xAOD::VertexContainer>(m_vxContainerName) or 
     evtStore()->contains<xAOD::VertexAuxContainer>(m_vxContainerName+"Aux.")) {
    ATH_MSG_DEBUG(m_vxContainerName << " is already available");
    return;
  }

  xAOD::VertexContainer* vxContainer(new xAOD::VertexContainer);
  xAOD::VertexAuxContainer* vxAuxContainer(new xAOD::VertexAuxContainer);
  vxContainer->setStore(vxAuxContainer);

  for(auto quad: *quads) {
    if(quad->size()!=4) ATH_MSG_ERROR("This quad does not have four leptons: " << quad->size());
    //prepare input for vertexing
    std::vector<const xAOD::TrackParticle*> inputTracks(0); //input to vertex fit
    std::vector<ElementLink<xAOD::TrackParticleContainer> > inputTrackLinks(0); //to store in the vertex object later
    for(const auto part: *quad) {
      if(part->type()==xAOD::Type::Muon) {
	const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(part);
	const xAOD::TrackParticle* tp(0);
	//get the InDet track, if there is one
	if(mu->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon && m_doVertexing_mu_withInDetForwTrack==false) {
	  tp = 0; // neglect Si-assoc forward muons
	} else {
	  tp = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	}
	if(tp) {
	  inputTracks.push_back(tp);
	  inputTrackLinks.push_back(mu->inDetTrackParticleLink());
	}
      } else {
	const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(part);
	if(m_doVertexing_el_withOrigInDetTrack) {
	  inputTracks.push_back(xAOD::EgammaHelpers::getOriginalTrackParticle(el));
	} else {
	  inputTracks.push_back(el->trackParticle()); //use GSF track
	}
	inputTrackLinks.push_back(el->trackParticleLink()); //this is used only for matching, not for vertexing
      }    
    }
    if(inputTracks.size()<3) {
      ATH_MSG_DEBUG("require at least 3 ID tracks in the quad, skipping this one");
      continue;
    }

    // do a fast fit to get an approximate vertex
    Amg::Vector3D appVertex;    
    m_VKVFitter->setMomCovCalc(0); //No total momentum and its covariance matrix
    
    if(m_VKVFitter->VKalVrtFitFast(inputTracks, appVertex).isSuccess()) {
      m_VKVFitter->setMomCovCalc(1);  //Total momentum and its covariance matrix are calculated
      xAOD::Vertex *myVxCandidate(0);
      
      myVxCandidate = m_VKVFitter->fit(inputTracks, appVertex);      
      if(!myVxCandidate) {
	ATH_MSG_DEBUG("Vertexing failed");
      } else {
	myVxCandidate->clearTracks();
	for(const auto tpLink : inputTrackLinks) {
	  myVxCandidate->addTrackAtVertex(tpLink);
	}
	ATH_MSG_DEBUG("Vertex with chi2: " << myVxCandidate->chiSquared() << "  ntracks: " << myVxCandidate->vxTrackAtVertex().size());
	vxContainer->push_back(myVxCandidate);
      }
    } else {
      ATH_MSG_DEBUG("Fast vertexing failed");
    }
  }
  
  (evtStore()->record(vxContainer, m_vxContainerName)).ignore();
  (evtStore()->record(vxAuxContainer, m_vxContainerName+"Aux.")).ignore();
  
  ATH_MSG_DEBUG("Vertex container size: " << vxContainer->size());
}

const double DerivationFramework::SkimmingToolHDBS2::s_MZ(91187.6*CLHEP::MeV);
const double DerivationFramework::SkimmingToolHDBS2::s_MKplus(493.677*CLHEP::MeV);
const double DerivationFramework::SkimmingToolHDBS2::s_MPiplus(139.570*CLHEP::MeV);
