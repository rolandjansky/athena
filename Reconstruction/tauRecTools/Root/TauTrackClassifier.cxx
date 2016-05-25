/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ASG include(s)
#include "PathResolver/PathResolver.h"

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// local include(s)
#include "tauRecTools/TauTrackClassifier.h"

using namespace tauRecTools;

//==============================================================================
// class TauTrackClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackClassifier::TauTrackClassifier(const std::string& sName)
  : TauRecToolBase(sName)
{
  declareProperty("Classifiers", m_vClassifier );
  declareProperty("TauTrackContainerName", m_tauTrackConName="TauTracks");
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
StatusCode TauTrackClassifier::execute(xAOD::TauJet& xTau)
{
  xAOD::TauTrackContainer* tauTrackCon = 0;
  ATH_CHECK(evtStore()->retrieve(tauTrackCon, m_tauTrackConName));
  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, tauTrackCon);
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
      CHECK(cClassifier->classifyTrack(*xTrack, xTau));
  }
  std::vector< ElementLink< xAOD::TauTrackContainer > > &tauTrackLinks(xTau.tauTrackLinksNonConst());
  std::sort(tauTrackLinks.begin(), tauTrackLinks.end(), sortTracks);
  float charge=0.0;
  for( const xAOD::TauTrack* trk : xTau.tracks(xAOD::TauJetParameters::classifiedCharged) ){
    charge += trk->track()->charge();
  }
  xTau.setCharge(charge);
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
  , m_rReader(0)
{
  declareProperty( "InputWeightsPath", m_sInputWeightsPath );
  declareProperty( "Threshold", m_fThreshold );
  declareProperty( "BackgroundType" , m_iBackgroundType );
  declareProperty( "SignalType", m_iSignalType );
  declareProperty( "ExpectedFlag", m_iExpectedFlag );
  m_mParsedVarsBDT.clear();
}

//______________________________________________________________________________
TrackMVABDT::~TrackMVABDT()
{
  delete m_rReader;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::initialize()
{
  m_rReader = new TMVA::Reader( "!Silent" );
  m_mAvailableVars=
    {
      {"TracksAuxDyn.tauPt",tauPt}
      , {"TracksAuxDyn.tauEta",tauEta}
      , {"TracksAuxDyn.trackEta",trackEta}
      , {"TracksAuxDyn.z0sinThetaTJVA",z0sinThetaTJVA}
      , {"TracksAuxDyn.rConv",rConv}
      , {"TracksAuxDyn.rConvII",rConvII}
      , {"TracksAuxDyn.DRJetSeedAxis",DRJetSeedAxis}
      , {"TracksAux.d0",d0}
      , {"TracksAux.qOverP",qOverP}
      , {"TracksAux.theta",theta}
      , {"TracksAux.numberOfInnermostPixelLayerHits", numberOfInnermostPixelLayerHits}
      , {"TracksAux.numberOfPixelHits",numberOfPixelHits}
      , {"TracksAux.numberOfPixelDeadSensors",numberOfPixelDeadSensors}
      , {"TracksAux.numberOfPixelSharedHits",numberOfPixelSharedHits}
      , {"TracksAux.numberOfSCTHits",numberOfSCTHits}
      , {"TracksAux.numberOfSCTDeadSensors",numberOfSCTDeadSensors}
      , {"TracksAux.numberOfSCTSharedHits",numberOfSCTSharedHits}
      , {"TracksAux.numberOfTRTHighThresholdHits",numberOfTRTHighThresholdHits}
      , {"TracksAux.numberOfTRTHits",numberOfTRTHits}
      , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors", nPixHits}
      , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors", nSiHits}
    };

  for (auto fVar : m_mAvailableVars)
    fVar.second = 0.;
    
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  setVars(xTrack, xTau);

  //why?
  if (xTrack.flag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag)==false)
    return StatusCode::SUCCESS;
  
  double dValue = m_rReader->EvaluateMVA( m_sInputWeightsPath );

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
  // better to replace that part at some point
  m_sInputWeightsPath = PathResolverFindCalibFile(m_sInputWeightsPath);
  ATH_MSG_DEBUG("InputWeightsPath: " << m_sInputWeightsPath);
  
  if (m_mParsedVarsBDT.empty())
  {
    ATH_CHECK(parseVariableContent());
      
    for (auto i : m_mParsedVarsBDT)
      ATH_MSG_DEBUG(i.first<<" "<<i.second);
    
    for (size_t i = 0; i < m_mParsedVarsBDT.size(); i++){
      std::string sVarName = m_mParsedVarsBDT[i];
      m_rReader->AddVariable( sVarName, &(m_mAvailableVars[sVarName]));
    }
  }
    
  m_rReader->BookMVA( m_sInputWeightsPath, m_sInputWeightsPath );
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::parseVariableContent()
{
  // example     <Variable VarIndex="0" Expression="TracksAuxDyn.tauPt" Label="TracksAuxDyn.tauPt" Title="tauPt" Unit="" Internal="TracksAuxDyn.tauPt" Type="F" Min="1.50000762e+04" Max="5.32858625e+05"/>
  std::string sLine;
  std::ifstream sFileStream (m_sInputWeightsPath);
  if (sFileStream.is_open())
  {
    while ( getline (sFileStream,sLine) )
    {
      size_t iPosVarindex = sLine.find("VarIndex=");
	
      if ( iPosVarindex == std::string::npos )
        continue;
	
      iPosVarindex += 10;
	
      size_t iPosVarindexEnd = sLine.find("\"",iPosVarindex);
      size_t iPosExpression = sLine.find("Expression=")+12;
      size_t iPosExpressionEnd = sLine.find("\"",iPosExpression);
	
      int iVarIndex = std::stoi(sLine.substr(iPosVarindex, iPosVarindexEnd-iPosVarindex));
      std::string sExpression = sLine.substr(iPosExpression, iPosExpressionEnd-iPosExpression);
	
      m_mParsedVarsBDT.insert(std::pair<int, std::string >(iVarIndex,sExpression));
    }
    sFileStream.close();
    return StatusCode::SUCCESS;
  }

  ATH_MSG_ERROR("Unable to open file "<<m_sInputWeightsPath);
  return StatusCode::FAILURE;
}

//______________________________________________________________________________
void TrackMVABDT::setVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  const xAOD::TrackParticle* xTrackParticle = xTrack.track();
  uint8_t iTracksNumberOfInnermostPixelLayerHits = 0; xTrackParticle->summaryValue(iTracksNumberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t iTracksNPixelHits = 0; xTrackParticle->summaryValue(iTracksNPixelHits, xAOD::numberOfPixelHits);
  uint8_t iTracksNPixelSharedHits = 0; xTrackParticle->summaryValue(iTracksNPixelSharedHits, xAOD::numberOfPixelSharedHits);
  uint8_t iTracksNPixelDeadSensors = 0; xTrackParticle->summaryValue(iTracksNPixelDeadSensors, xAOD::numberOfPixelDeadSensors);
  uint8_t iTracksNSCTHits = 0; xTrackParticle->summaryValue(iTracksNSCTHits, xAOD::numberOfSCTHits);
  uint8_t iTracksNSCTSharedHits = 0; xTrackParticle->summaryValue(iTracksNSCTSharedHits, xAOD::numberOfSCTSharedHits);
  uint8_t iTracksNSCTDeadSensors = 0; xTrackParticle->summaryValue(iTracksNSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
  uint8_t iTracksNTRTHighThresholdHits = 0; xTrackParticle->summaryValue( iTracksNTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits);
  uint8_t iTracksNTRTHits = 0; xTrackParticle->summaryValue( iTracksNTRTHits, xAOD::numberOfTRTHits);
	
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
  
  
  m_mAvailableVars["TracksAuxDyn.tauPt"] = xTau.pt();
  m_mAvailableVars["TracksAuxDyn.tauEta"] = xTau.eta();
  m_mAvailableVars["TracksAuxDyn.z0sinThetaTJVA"] = xTrack.z0sinThetaTJVA(xTau);
  m_mAvailableVars["TracksAuxDyn.rConv"] = xTrack.rConv(xTau);
  m_mAvailableVars["TracksAuxDyn.rConvII"] = xTrack.rConvII(xTau);
  m_mAvailableVars["TracksAuxDyn.DRJetSeedAxis"] = xTrack.dRJetSeedAxis(xTau);
  m_mAvailableVars["TracksAuxDyn.trackEta"] = xTrackParticle->d0();
  m_mAvailableVars["TracksAux.d0"] = xTrackParticle->d0();
  m_mAvailableVars["TracksAux.qOverP"] = xTrackParticle->qOverP();
  m_mAvailableVars["TracksAux.theta"] = xTrackParticle->theta();
  m_mAvailableVars["TracksAux.numberOfInnermostPixelLayerHits"] = fTracksNumberOfInnermostPixelLayerHits;
  m_mAvailableVars["TracksAux.numberOfPixelHits"] = fTracksNPixelHits;
  m_mAvailableVars["TracksAux.numberOfPixelDeadSensors"] = fTracksNPixelDeadSensors;
  m_mAvailableVars["TracksAux.numberOfPixelSharedHits"] = fTracksNPixelSharedHits;
  m_mAvailableVars["TracksAux.numberOfSCTHits"] = fTracksNSCTHits;
  m_mAvailableVars["TracksAux.numberOfSCTDeadSensors"] = fTracksNSCTDeadSensors;
  m_mAvailableVars["TracksAux.numberOfSCTSharedHits"] = fTracksNSCTSharedHits;
  m_mAvailableVars["TracksAux.numberOfTRTHighThresholdHits"] = fTracksNTRTHighThresholdHits;
  m_mAvailableVars["TracksAux.numberOfTRTHits"] = fTracksNTRTHits;
  m_mAvailableVars["TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors"] = fTracksNPixHits;
  m_mAvailableVars["TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors"] = fTracksNSiHits;
}
