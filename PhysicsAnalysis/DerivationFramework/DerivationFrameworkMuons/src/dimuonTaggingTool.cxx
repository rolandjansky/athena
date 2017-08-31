/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// dimuonTaggingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dongliang Zhang (dongliang.zhang@cern.ch)
#include "DerivationFrameworkMuons/dimuonTaggingTool.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>
#include <memory>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
<<<<<<< HEAD
=======
#include "DerivationFrameworkMuons/IMuonTPExtrapolationTool.h"
#include "DerivationFrameworkMuons/IIDTrackCaloDepositsDecoratorTool.h"
>>>>>>> Moved muonTP files into derivation framework

// Constructor
DerivationFramework::dimuonTaggingTool::dimuonTaggingTool(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p),
  m_matchTool( "Trig::TrigMuonMatching/TrigMuonMatching" ),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool to access the trigger decision");
  declareProperty("OrTrigs", m_orTrigs=std::vector< std::string >());
  declareProperty("AndTrigs", m_andTrigs=std::vector< std::string >());

  declareProperty("TrigMatchTool", m_matchTool, "Tool for trigger matching");
  declareProperty("TriggerMatchDeltaR", m_triggerMatchDeltaR = 0.1);

  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
  declareProperty("Mu1PtMin", m_mu1PtMin=-1);
  declareProperty("Mu1AbsEtaMax", m_mu1AbsEtaMax=9999.);
  declareProperty("Mu1Types", m_mu1Types=std::vector< int >());
  declareProperty("Mu1Trigs", m_mu1Trigs=std::vector< std::string >());
  declareProperty("Mu1IsoCuts", m_mu1IsoCuts=std::map< int, double >());

  declareProperty("Mu2PtMin", m_mu2PtMin=-1);
  declareProperty("Mu2AbsEtaMax", m_mu2AbsEtaMax=9999.);
  declareProperty("Mu2Types", m_mu2Types=std::vector< int >());
  declareProperty("Mu2Trigs", m_mu2Trigs=std::vector< std::string >());
  declareProperty("Mu2IsoCuts", m_mu1IsoCuts=std::map< int, double >());

  declareProperty("UseTrackProbe", m_useTrackProbe=true);
  declareProperty("TrackContainerKey", m_trackSGKey="InDetTrackParticles");

  declareProperty("OppositeCharge", m_requireOS=true); 
  declareProperty("PairDPhiMin", m_dPhiMin=-1); 
  declareProperty("InvariantMassLow", m_invariantMassLow=2.0*CLHEP::GeV); 
  declareProperty("InvariantMassHigh", m_invariantMassHigh=4.3*CLHEP::GeV); 

  declareProperty("IDTrackThinningConeSize", m_thinningConeSize=0.4);
  declareProperty("BranchPrefix", m_br_prefix="");
}
  
// Destructor
DerivationFramework::dimuonTaggingTool::~dimuonTaggingTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::dimuonTaggingTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // trigger decision tool, needed when there is trigger requirement
  if(m_orTrigs.size()>0 || m_andTrigs.size()>0){
    if(m_trigDecisionTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
  }

  // load the matching tool
  if( ! m_matchTool.empty() ) {
     CHECK( m_matchTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the TrigMatchTool!" );
  } else {
     REPORT_ERROR( StatusCode::FAILURE ) << "Could not retrive the TrigMatchTool as it "
                                         << "was not specified!";
     return StatusCode::FAILURE;
  }

  m_invariantMassLow2 = m_invariantMassLow*fabs(m_invariantMassLow);
  m_invariantMassHigh2 = m_invariantMassHigh*fabs(m_invariantMassHigh);
  m_thinningConeSize2 = m_thinningConeSize*fabs(m_thinningConeSize);

  return StatusCode::SUCCESS;
}

bool DerivationFramework::dimuonTaggingTool::checkTrigMatch(const xAOD::Muon *mu, const std::vector< std::string >& Trigs) const
{
  for(auto t: Trigs) {if(m_matchTool->match(mu, t)) return true;}
  return 0==Trigs.size();
}

StatusCode DerivationFramework::dimuonTaggingTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::dimuonTaggingTool::addBranches() const
{
  std::unique_ptr< int > keepEvent( new int(0) );
  std::unique_ptr< std::vector<int> > trkStatus( new std::vector<int>() );
  ATH_CHECK(fillInfo(keepEvent.get(), *trkStatus));

  std::string DIMU_pass(m_br_prefix+"DIMU_pass");
  if (!evtStore()->contains< int >(DIMU_pass)) {
     CHECK(evtStore()->record(std::move( keepEvent ), DIMU_pass));
  }
  std::string DIMU_trkStatus(m_br_prefix+"DIMU_trkStatus");
  if (!evtStore()->contains< std::vector< int > >(DIMU_trkStatus)) {
     CHECK(evtStore()->record(std::move( trkStatus ), DIMU_trkStatus));
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::dimuonTaggingTool::fillInfo(int* keepEvent, std::vector<int>& trackMask) const
{
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));
  bool isMC = eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION );

  const xAOD::TrackParticleContainer *tracks(0); 
  ATH_CHECK(evtStore()->retrieve(tracks, m_trackSGKey));
  const unsigned int NTRACKS = tracks->size();
  trackMask.assign(NTRACKS,0);

  //// check Or triggers
  for(unsigned int i=0; i<m_orTrigs.size(); i++){if(m_trigDecisionTool->isPassed(m_orTrigs[i])) *keepEvent = 100;}

  //// check "and" triggers if didn't pass "Or" triggers
  if(*keepEvent == 0 && m_andTrigs.size()>0){
    bool passAndTriggers = false;
    for(unsigned int i=0; i<m_andTrigs.size(); i++){
      if(m_trigDecisionTool->isPassed(m_andTrigs[i])){ passAndTriggers = true; break;}}
    if(!passAndTriggers) return StatusCode::SUCCESS;
   }

  /// muon selection
  const xAOD::MuonContainer *muons(0);
  ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey));
  for(auto mu_itr1: *muons) {
    if(!passMuonCuts(mu_itr1, m_mu1PtMin, m_mu1AbsEtaMax, m_mu1Types, m_mu1Trigs, m_mu1IsoCuts)) continue;
    for(auto mu_itr2: *muons) {
      if(mu_itr2==mu_itr1) continue;
      if(!passMuonCuts(mu_itr2, m_mu2PtMin, m_mu2AbsEtaMax, m_mu2Types, m_mu2Trigs, m_mu2IsoCuts)) continue;
      if(!muonPairCheck(mu_itr1, mu_itr2->charge(), mu_itr2->p4())) continue;
      (*keepEvent)++;
    }
    if(m_useTrackProbe){
      for(auto mu_itr2: *tracks) {
        if(mu_itr2 == mu_itr1->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
        if(!passKinematicCuts(mu_itr2, m_mu2PtMin, m_mu2AbsEtaMax)) continue;
        if(!muonPairCheck(mu_itr1, mu_itr2->charge(), mu_itr2->p4())) continue;
        (*keepEvent)++;
        trackMask[mu_itr2->index()]+=100;
        maskNearbyIDtracks(mu_itr2, trackMask, tracks);
      }
    }
  }

  /// also mask tracks around truth muons.
  if(isMC){
    const xAOD::TruthParticleContainer *truth(0); 
    ATH_CHECK(evtStore()->retrieve(truth, "MuonTruthParticles"));
    for(auto mu_itr2: *truth) maskNearbyIDtracks(mu_itr2, trackMask, tracks);
  }

  static SG::AuxElement::Decorator< int > deco_trkFlag(m_br_prefix+"DIMU_Status");
  int i(0);
  for(auto mu_itr2: *tracks) {
    int code = trackMask[i++];
    deco_trkFlag(*mu_itr2) = code>=100?1000:code;
  }

  return StatusCode::SUCCESS;
}

void DerivationFramework::dimuonTaggingTool::maskNearbyIDtracks(const xAOD::IParticle* par1, std::vector<int>& trackMask, const xAOD::TrackParticleContainer* tracks) const
{
  unsigned int i(0);
  for(auto trk1: *tracks) {
   if(trackMask[i]==0){
     float dEta = fabs(par1->eta()-trk1->eta());
//      if(dEta>m_thinningConeSize) continue;
     float dPhi = fabs(par1->phi()-trk1->phi());
     if(dPhi>TMath::Pi()) dPhi = TMath::TwoPi()-dPhi;
//      if(dPhi>m_thinningConeSize) continue;
     if(dEta*dEta+dPhi*dPhi < m_thinningConeSize2) trackMask[i]++;
    }
   ++i; 
  }
  return;
}

bool DerivationFramework::dimuonTaggingTool::passKinematicCuts(const xAOD::IParticle *mu, float ptMin, float absEtaMax) const
{
  if(!mu) return false;
  if(mu->pt()<ptMin) return false;
  if(fabs(mu->eta())>absEtaMax) return false;

  return true;
}

  /// so it will also work with tracks.
bool DerivationFramework::dimuonTaggingTool::muonPairCheck(const xAOD::Muon *mu1, float charge2, const TLorentzVector& mu2) const
{
  if(m_requireOS && mu1->charge()*charge2>0.) return false;
  if(m_dPhiMin>0 && (mu1->p4()).DeltaPhi(mu2)<m_dPhiMin) return false;
  float mass2 = (mu2+mu1->p4()).M2();
  if(mass2<m_invariantMassLow2 || (m_invariantMassHigh>0. && mass2>m_invariantMassHigh2)) return false;

  return true;
}

bool DerivationFramework::dimuonTaggingTool::passMuonCuts(const xAOD::Muon *mu, const float ptMin, const float absEtaMax, const std::vector< int >& types, const std::vector< std::string >& trigs, const std::map< int, double > muIsoCuts) const
{
  /// the object should exist
  if(!mu) return false;

  /// simple pt, eta cuts
  if(mu->pt()<ptMin) return false;
  if(fabs(mu->eta())>absEtaMax) return false;

  /// Muon type check
  if(types.size()>0){
    bool passTypeCut = false;
    int type = mu->muonType();
    for(unsigned int i=0; i<types.size(); ++i) {if(types[i]==type) {passTypeCut=true; break;}}
    if(!passTypeCut) return false;
   }

  /// isolation cuts. Mutiple cuts allowed and return the logical AND results.
  for(std::map<int,double>::const_iterator it=muIsoCuts.begin(); it!=muIsoCuts.end(); ++it){
    float isoValue=0;
    const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(it->first);
    if(!(mu->isolation(isoValue, isoType)) || isoValue > it->second) return false;
  }

  /// do trigger matching
  if(trigs.size()>0 && !checkTrigMatch(mu, trigs)) return false;

  return true;
}

bool DerivationFramework::dimuonTaggingTool::passMuonCuts(const xAOD::Muon *mu, const float ptMin, const float absEtaMax, const std::vector< int >& types, const std::vector< std::string >& trigs) const
{ 
  if(!mu) return false;
  if(mu->pt()<ptMin) return false;
  if(fabs(mu->eta())>absEtaMax) return false;

  if(types.size()>0){
    bool passTypeCut = false;
    int type = mu->muonType();
    for(unsigned int i=0; i<types.size(); ++i) {if(types[i]==type) {passTypeCut=true; break;}}
    if(!passTypeCut) return false;
   }

  if(trigs.size()>0 && !checkTrigMatch(mu, trigs)) return false;

  return true;
}
