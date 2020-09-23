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
  , m_inputVariableNames()
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
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::initialize()
{
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau) const
{
  /// If TT/IT gives TT, only run TT/CR; otherwise, run IT/FT 
  if (xTrack.flag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag)==false)
    return StatusCode::SUCCESS;
  
  std::vector<float> values;
  ATH_CHECK(calculateVariables(xTrack, xTau, values));
  double dValue = m_rReader->GetClassification(values);
  
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
  
  m_rReader = tauRecTools::configureMVABDT( m_inputVariableNames, m_sInputWeightsPath.c_str() );
  if(m_rReader==nullptr) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::calculateVariables(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, std::vector<float>& values) const
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

  std::map<TString, float> valueMap;
  // Could use the same naming convention in the BDT to simplify 
  valueMap["TracksAuxDyn.jetSeedPt"] = xTau.ptJetSeed();
  valueMap["TracksAuxDyn.tauPt"] = xTau.ptIntermediateAxis();
  valueMap["TracksAuxDyn.tauEta"] = xTau.etaIntermediateAxis();
  valueMap["TracksAuxDyn.z0sinThetaTJVA"] = xTrack.z0sinThetaTJVA(xTau);
  valueMap["TracksAuxDyn.rConv"] = xTrack.rConv(xTau);
  valueMap["TracksAuxDyn.rConvII"] = xTrack.rConvII(xTau);
  valueMap["TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII"] = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  valueMap["TracksAuxDyn.DRJetSeedAxis"] = xTrack.dRJetSeedAxis(xTau);
  valueMap["TracksAuxDyn.dRJetSeedAxis"] = xTrack.dRJetSeedAxis(xTau);
  valueMap["TracksAuxDyn.trackEta"] = xTrackParticle->eta();
  valueMap["TracksAux.d0"] = xTrackParticle->d0();
  valueMap["TracksAux.qOverP"] = xTrackParticle->qOverP();
  valueMap["TracksAux.theta"] = xTrackParticle->theta();
  valueMap["TracksAux.eProbabilityHT"] = fTracksEProbabilityHT;
  valueMap["TracksAux.numberOfInnermostPixelLayerHits"] = fTracksNumberOfInnermostPixelLayerHits;
  valueMap["TracksAux.numberOfPixelHits"] = fTracksNPixelHits;
  valueMap["TracksAux.numberOfPixelDeadSensors"] = fTracksNPixelDeadSensors;
  valueMap["TracksAux.numberOfPixelSharedHits"] = fTracksNPixelSharedHits;
  valueMap["TracksAux.numberOfSCTHits"] = fTracksNSCTHits;
  valueMap["TracksAux.numberOfSCTDeadSensors"] = fTracksNSCTDeadSensors;
  valueMap["TracksAux.numberOfSCTSharedHits"] = fTracksNSCTSharedHits;
  valueMap["TracksAux.numberOfTRTHighThresholdHits"] = fTracksNTRTHighThresholdHits;
  valueMap["TracksAux.numberOfTRTHits"] = fTracksNTRTHits;
  valueMap["TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors"] = fTracksNPixHits;
  valueMap["TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors"] = fTracksNSiHits;

  valueMap["TauTracksAuxDyn.jetSeedPt"] = xTau.ptJetSeed();
  valueMap["TauTracksAuxDyn.tauPt"] = xTau.ptIntermediateAxis();
  valueMap["TauTracksAuxDyn.tauEta"] = xTau.etaIntermediateAxis();
  valueMap["TauTracksAuxDyn.z0sinThetaTJVA"] = xTrack.z0sinThetaTJVA(xTau);
  valueMap["TauTracksAuxDyn.rConv"] = xTrack.rConv(xTau);
  valueMap["TauTracksAuxDyn.rConvII"] = xTrack.rConvII(xTau);
  valueMap["TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII"] = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  valueMap["TauTracksAuxDyn.dRJetSeedAxis"] = xTrack.dRJetSeedAxis(xTau);
  valueMap["TauTracksAuxDyn.trackEta"] = xTrackParticle->eta();
  valueMap["TauTracksAuxDyn.d0"] = xTrackParticle->d0();
  valueMap["TauTracksAuxDyn.qOverP"] = xTrackParticle->qOverP();
  valueMap["TauTracksAuxDyn.theta"] = xTrackParticle->theta();
  valueMap["TauTracksAuxDyn.eProbabilityHT"] = fTracksEProbabilityHT;
  valueMap["TauTracksAuxDyn.numberOfInnermostPixelLayerHits"] = fTracksNumberOfInnermostPixelLayerHits;
  valueMap["TauTracksAuxDyn.numberOfPixelHits"] = fTracksNPixelHits;
  valueMap["TauTracksAuxDyn.numberOfPixelDeadSensors"] = fTracksNPixelDeadSensors;
  valueMap["TauTracksAuxDyn.numberOfPixelSharedHits"] = fTracksNPixelSharedHits;
  valueMap["TauTracksAuxDyn.numberOfSCTHits"] = fTracksNSCTHits;
  valueMap["TauTracksAuxDyn.numberOfSCTDeadSensors"] = fTracksNSCTDeadSensors;
  valueMap["TauTracksAuxDyn.numberOfSCTSharedHits"] = fTracksNSCTSharedHits;
  valueMap["TauTracksAuxDyn.numberOfTRTHighThresholdHits"] = fTracksNTRTHighThresholdHits;
  valueMap["TauTracksAuxDyn.numberOfTRTHits"] = fTracksNTRTHits;
  valueMap["TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors"] = fTracksNPixHits;
  valueMap["TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors"] = fTracksNSiHits;

  valueMap["1/(TauTracksAuxDyn.trackPt)"] = 1./xTrackParticle->pt();
  valueMap["fabs(TauTracksAuxDyn.qOverP)"] = std::abs(xTrackParticle->qOverP());
  valueMap["TauTracksAuxDyn.numberOfContribPixelLayers"] = fNumberOfContribPixelLayers;
  valueMap["TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles"] = fTracksNPixHits+fNumberOfPixelHoles;
  valueMap["TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors+TauTracksAuxDyn.numberOfSCTHoles"] = fTracksNSiHits+fNumberOfPixelHoles+fNumberOfSCTHoles;
  valueMap["TauTracksAuxDyn.numberOfPixelHoles"] = fNumberOfPixelHoles;
  valueMap["TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHoles"] = fNumberOfPixelHoles+fNumberOfSCTHoles;
  valueMap["TauTracksAuxDyn.numberOfSCTHoles"] = fNumberOfSCTHoles;
  valueMap["TauTracksAux.pt"] = xTrackParticle->pt();

  values.clear();
  values.reserve(m_inputVariableNames.size());
  for (auto varName : m_inputVariableNames) {
    values.push_back(valueMap[varName]);
  }

  return StatusCode::SUCCESS;  
}
