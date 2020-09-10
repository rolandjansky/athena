/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// local include(s)
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/HelperFunctions.h"

#include <fstream>

using namespace tauRecTools;

//==============================================================================
// class TauTrackClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackClassifier::TauTrackClassifier(const std::string& sName)
  : TauRecToolBase(sName)
{
  declareProperty("Classifiers", m_vClassifier );
  declareProperty("ClassifierNames", m_vClassifierNames );
}

//______________________________________________________________________________
TauTrackClassifier::~TauTrackClassifier()
{
}

//______________________________________________________________________________
StatusCode TauTrackClassifier::initialize()
{
  ATH_MSG_DEBUG("intialize classifiers");

  for (auto cClassifier : m_vClassifier){
    ATH_MSG_INFO("TauTrackClassifier tool : " << cClassifier );
    ATH_CHECK(cClassifier.retrieve());
  }
 
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTrackClassifier::executeTrackClassifier(xAOD::TauJet& xTau, xAOD::TauTrackContainer& tauTrackCon) const
{
  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, &tauTrackCon);
  for (xAOD::TauTrack* xTrack : vTracks)
  {
    // reset all track flags and set status to unclassified
    xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedConversion, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedIsolation, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedFake, false);
    xTrack->setFlag(xAOD::TauJetParameters::unclassified, true);

    // execute the bdt track classifier 
    for (auto cClassifier : m_vClassifier)
      ATH_CHECK(cClassifier->classifyTrack(*xTrack, xTau));
  }
  std::vector< ElementLink< xAOD::TauTrackContainer > > &tauTrackLinks(xTau.allTauTrackLinksNonConst());
  std::sort(tauTrackLinks.begin(), tauTrackLinks.end(), sortTracks);
  float charge=0.0;
  for( const xAOD::TauTrack* trk : xTau.tracks(xAOD::TauJetParameters::classifiedCharged) ){
    charge += trk->track()->charge();
  }

  xTau.setCharge(charge);
  xTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) xTau.nTracks());
  xTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) xTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

  //set modifiedIsolationTrack
  for (xAOD::TauTrack* xTrack : vTracks) {
    if( not xTrack->flag(xAOD::TauJetParameters::classifiedCharged) and 
	xTrack->flag(xAOD::TauJetParameters::passTrkSelector) ) xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, true);
    else xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, false);
  }
  xTau.setDetail(xAOD::TauJetParameters::nModifiedIsolationTracks, (int) xTau.nTracks(xAOD::TauJetParameters::modifiedIsolationTrack));


  return StatusCode::SUCCESS;
}

//==============================================================================
// class TrackMVABDT
//==============================================================================

//______________________________________________________________________________
TrackMVABDT::TrackMVABDT(const std::string& sName)
  : TauRecToolBase(sName)
  , m_sInputWeightsPath("")
  , m_fThreshold(0.)
  , m_iSignalType(xAOD::TauJetParameters::classifiedCharged)
  , m_iBackgroundType(xAOD::TauJetParameters::classifiedFake)
  , m_iExpectedFlag(xAOD::TauJetParameters::unclassified)
  , m_rReader(nullptr)
  , m_mAvailableVars({})
{
  declareProperty( "InputWeightsPath", m_sInputWeightsPath );
  declareProperty( "Threshold", m_fThreshold );
  declareProperty( "BackgroundType" , m_iBackgroundType );
  declareProperty( "SignalType", m_iSignalType );
  declareProperty( "ExpectedFlag", m_iExpectedFlag );
}

//______________________________________________________________________________
TrackMVABDT::~TrackMVABDT()
{
}

//______________________________________________________________________________
StatusCode TrackMVABDT::finalize()
{
  for( std::pair<TString, float*> p : m_mAvailableVars ) delete p.second;
  m_mAvailableVars.clear();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::initialize()
{
  m_mAvailableVars={
    {"TracksAuxDyn.tauPt", new float(0)}
    , {"TracksAuxDyn.jetSeedPt", new float(0)}
    , {"TracksAuxDyn.tauEta", new float(0)}
    , {"TracksAuxDyn.trackEta", new float(0)}
    , {"TracksAuxDyn.z0sinThetaTJVA", new float(0)}
    , {"TracksAuxDyn.rConv", new float(0)}
    , {"TracksAuxDyn.rConvII", new float(0)}
    , {"TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII", new float(0)}
    , {"TracksAuxDyn.DRJetSeedAxis", new float(0)}
    , {"TracksAuxDyn.dRJetSeedAxis", new float(0)}
    , {"TracksAux.d0", new float(0)}
    , {"TracksAux.qOverP", new float(0)}
    , {"TracksAux.theta", new float(0)}
    , {"TracksAux.eProbabilityHT", new float(0)}
    , {"TracksAux.numberOfInnermostPixelLayerHits", new float(0)}
    , {"TracksAux.numberOfPixelHits", new float(0)}
    , {"TracksAux.numberOfPixelDeadSensors", new float(0)}
    , {"TracksAux.numberOfPixelSharedHits", new float(0)}
    , {"TracksAux.numberOfSCTHits", new float(0)}
    , {"TracksAux.numberOfSCTDeadSensors", new float(0)}
    , {"TracksAux.numberOfSCTSharedHits", new float(0)}
    , {"TracksAux.numberOfTRTHighThresholdHits", new float(0)}
    , {"TracksAux.numberOfTRTHits", new float(0)}
    , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors", new float(0)}
    , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors", new float(0)}
    
    , {"TauTracksAuxDyn.tauPt", new float(0)}
    , {"TauTracksAuxDyn.jetSeedPt", new float(0)}
    , {"TauTracksAuxDyn.tauEta", new float(0)}
    , {"TauTracksAuxDyn.trackEta", new float(0)}
    , {"TauTracksAuxDyn.z0sinThetaTJVA", new float(0)}
    , {"TauTracksAuxDyn.rConv", new float(0)}
    , {"TauTracksAuxDyn.rConvII", new float(0)}
    , {"TauTracksAuxDyn.dRJetSeedAxis", new float(0)}
    , {"TauTracksAuxDyn.d0", new float(0)}
    , {"TauTracksAuxDyn.qOverP", new float(0)}
    , {"TauTracksAuxDyn.theta", new float(0)}
    , {"TauTracksAuxDyn.eProbabilityHT", new float(0)}
    , {"TauTracksAuxDyn.numberOfInnermostPixelLayerHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelDeadSensors", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelSharedHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfSCTHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfSCTDeadSensors", new float(0)}
    , {"TauTracksAuxDyn.numberOfSCTSharedHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfTRTHighThresholdHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfTRTHits", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors", new float(0)}
    
    
    , {"1/(TauTracksAuxDyn.trackPt)", new float(0)}
    , {"fabs(TauTracksAuxDyn.qOverP)", new float(0)}
    , {"TauTracksAuxDyn.numberOfContribPixelLayers", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors+TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHoles", new float(0)}
    , {"TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
    , {"TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
    , {"TauTracksAux.pt", new float(0)}
  };
    
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  /// If TT/IT gives TT, only run TT/CR; otherwise, run IT/FT 
  if (xTrack.flag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag)==false)
    return StatusCode::SUCCESS;
  
  ATH_CHECK(setVars(xTrack, xTau));
  double dValue = m_rReader->GetClassification();
  
  xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag, false);
  if (m_fThreshold < dValue)
    xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iSignalType, true);
  else 
    xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iBackgroundType, true);

  xTrack.addBdtScore(dValue);

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::addWeightsFile()
{
  m_sInputWeightsPath = find_file(m_sInputWeightsPath);
  ATH_MSG_DEBUG("InputWeightsPath: " << m_sInputWeightsPath);
  
  m_rReader = tauRecTools::configureMVABDT( m_mAvailableVars, m_sInputWeightsPath.c_str() );
  if(m_rReader==nullptr) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::setVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  const xAOD::TrackParticle* xTrackParticle = xTrack.track();
  uint8_t iTracksNumberOfInnermostPixelLayerHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNumberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits) );
  uint8_t iTracksNPixelHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelHits, xAOD::numberOfPixelHits) );
  uint8_t iTracksNPixelSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelSharedHits, xAOD::numberOfPixelSharedHits) );
  uint8_t iTracksNPixelDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNPixelDeadSensors, xAOD::numberOfPixelDeadSensors) );
  uint8_t iTracksNSCTHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTHits, xAOD::numberOfSCTHits) );
  uint8_t iTracksNSCTSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTSharedHits, xAOD::numberOfSCTSharedHits) );
  uint8_t iTracksNSCTDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(iTracksNSCTDeadSensors, xAOD::numberOfSCTDeadSensors) );
  uint8_t iTracksNTRTHighThresholdHits = 0; ATH_CHECK( xTrackParticle->summaryValue( iTracksNTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits) );
  uint8_t iTracksNTRTHits = 0; ATH_CHECK( xTrackParticle->summaryValue( iTracksNTRTHits, xAOD::numberOfTRTHits) );
  uint8_t iNumberOfContribPixelLayers = 0; ATH_CHECK( xTrackParticle->summaryValue(iNumberOfContribPixelLayers, xAOD::numberOfContribPixelLayers) );
  uint8_t iNumberOfPixelHoles = 0; ATH_CHECK( xTrackParticle->summaryValue(iNumberOfPixelHoles, xAOD::numberOfPixelHoles) );
  uint8_t iNumberOfSCTHoles = 0; ATH_CHECK( xTrackParticle->summaryValue(iNumberOfSCTHoles, xAOD::numberOfSCTHoles) );
	
  float fTracksNumberOfInnermostPixelLayerHits = (float)iTracksNumberOfInnermostPixelLayerHits;
  float fTracksNPixelHits = (float)iTracksNPixelHits;
  float fTracksNPixelDeadSensors = (float)iTracksNPixelDeadSensors;
  float fTracksNPixelSharedHits = (float)iTracksNPixelSharedHits;
  float fTracksNSCTHits = (float)iTracksNSCTHits;
  float fTracksNSCTDeadSensors = (float)iTracksNSCTDeadSensors;
  float fTracksNSCTSharedHits = (float)iTracksNSCTSharedHits;
  float fTracksNTRTHighThresholdHits = (float)iTracksNTRTHighThresholdHits;
  float fTracksNTRTHits = (float)iTracksNTRTHits;
	
  float fTracksNPixHits = fTracksNPixelHits + fTracksNPixelDeadSensors;
  float fTracksNSiHits = fTracksNPixelHits + fTracksNPixelDeadSensors + fTracksNSCTHits + fTracksNSCTDeadSensors;

  float fTracksEProbabilityHT; ATH_CHECK( xTrackParticle->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT) );

  float fNumberOfContribPixelLayers = float(iNumberOfContribPixelLayers);
  float fNumberOfPixelHoles = float(iNumberOfPixelHoles);
  float fNumberOfSCTHoles = float(iNumberOfSCTHoles);

  // Could use the same naming convention in the BDT to simplify 
  setVar("TracksAuxDyn.jetSeedPt") = xTau.ptJetSeed();
  setVar("TracksAuxDyn.tauPt") = xTau.ptIntermediateAxis();
  setVar("TracksAuxDyn.tauEta") = xTau.etaIntermediateAxis();
  setVar("TracksAuxDyn.z0sinThetaTJVA") = xTrack.z0sinThetaTJVA(xTau);
  setVar("TracksAuxDyn.rConv") = xTrack.rConv(xTau);
  setVar("TracksAuxDyn.rConvII") = xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII") = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  setVar("TracksAuxDyn.DRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TracksAuxDyn.dRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TracksAuxDyn.trackEta") = xTrackParticle->eta();
  setVar("TracksAux.d0") = xTrackParticle->d0();
  setVar("TracksAux.qOverP") = xTrackParticle->qOverP();
  setVar("TracksAux.theta") = xTrackParticle->theta();
  setVar("TracksAux.eProbabilityHT") = fTracksEProbabilityHT;
  setVar("TracksAux.numberOfInnermostPixelLayerHits") = fTracksNumberOfInnermostPixelLayerHits;
  setVar("TracksAux.numberOfPixelHits") = fTracksNPixelHits;
  setVar("TracksAux.numberOfPixelDeadSensors") = fTracksNPixelDeadSensors;
  setVar("TracksAux.numberOfPixelSharedHits") = fTracksNPixelSharedHits;
  setVar("TracksAux.numberOfSCTHits") = fTracksNSCTHits;
  setVar("TracksAux.numberOfSCTDeadSensors") = fTracksNSCTDeadSensors;
  setVar("TracksAux.numberOfSCTSharedHits") = fTracksNSCTSharedHits;
  setVar("TracksAux.numberOfTRTHighThresholdHits") = fTracksNTRTHighThresholdHits;
  setVar("TracksAux.numberOfTRTHits") = fTracksNTRTHits;
  setVar("TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors") = fTracksNPixHits;
  setVar("TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors") = fTracksNSiHits;

  setVar("TauTracksAuxDyn.jetSeedPt") = xTau.ptJetSeed();
  setVar("TauTracksAuxDyn.tauPt") = xTau.ptIntermediateAxis();
  setVar("TauTracksAuxDyn.tauEta") = xTau.etaIntermediateAxis();
  setVar("TauTracksAuxDyn.z0sinThetaTJVA") = xTrack.z0sinThetaTJVA(xTau);
  setVar("TauTracksAuxDyn.rConv") = xTrack.rConv(xTau);
  setVar("TauTracksAuxDyn.rConvII") = xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII") = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.dRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TauTracksAuxDyn.trackEta") = xTrackParticle->eta();
  setVar("TauTracksAuxDyn.d0") = xTrackParticle->d0();
  setVar("TauTracksAuxDyn.qOverP") = xTrackParticle->qOverP();
  setVar("TauTracksAuxDyn.theta") = xTrackParticle->theta();
  setVar("TauTracksAuxDyn.eProbabilityHT") = fTracksEProbabilityHT;
  setVar("TauTracksAuxDyn.numberOfInnermostPixelLayerHits") = fTracksNumberOfInnermostPixelLayerHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits") = fTracksNPixelHits;
  setVar("TauTracksAuxDyn.numberOfPixelDeadSensors") = fTracksNPixelDeadSensors;
  setVar("TauTracksAuxDyn.numberOfPixelSharedHits") = fTracksNPixelSharedHits;
  setVar("TauTracksAuxDyn.numberOfSCTHits") = fTracksNSCTHits;
  setVar("TauTracksAuxDyn.numberOfSCTDeadSensors") = fTracksNSCTDeadSensors;
  setVar("TauTracksAuxDyn.numberOfSCTSharedHits") = fTracksNSCTSharedHits;
  setVar("TauTracksAuxDyn.numberOfTRTHighThresholdHits") = fTracksNTRTHighThresholdHits;
  setVar("TauTracksAuxDyn.numberOfTRTHits") = fTracksNTRTHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors") = fTracksNPixHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors") = fTracksNSiHits;

  setVar("1/(TauTracksAuxDyn.trackPt)") = 1./xTrackParticle->pt();
  setVar("fabs(TauTracksAuxDyn.qOverP)") = std::abs(xTrackParticle->qOverP());
  setVar("TauTracksAuxDyn.numberOfContribPixelLayers") = fNumberOfContribPixelLayers;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles") = fTracksNPixHits+fNumberOfPixelHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors+TauTracksAuxDyn.numberOfSCTHoles") = fTracksNSiHits+fNumberOfPixelHoles+fNumberOfSCTHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHoles") = fNumberOfPixelHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHoles") = fNumberOfPixelHoles+fNumberOfSCTHoles;
  setVar("TauTracksAuxDyn.numberOfSCTHoles") = fNumberOfSCTHoles;
  setVar("TauTracksAux.pt") = xTrackParticle->pt();

  return StatusCode::SUCCESS;  
}
