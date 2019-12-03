/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ASG include(s)
#include "PathResolver/PathResolver.h"

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// local include(s)
#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/HelperFunctions.h"

#include <fstream>

using namespace tauRecTools;

//==============================================================================
// class TauTrackRNNClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackRNNClassifier::TauTrackRNNClassifier(const std::string& sName)
  : TauRecToolBase(sName)
{
  declareProperty("Classifiers", m_vClassifier );
  declareProperty("ClassifierNames", m_vClassifierNames );
  declareProperty("TauTrackContainerName", m_tauTrackConName="TauTracks");
}

//______________________________________________________________________________
TauTrackRNNClassifier::~TauTrackRNNClassifier()
{
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::initialize()
{
  ATH_MSG_DEBUG("intialize classifiers");

  for (auto cClassifier : m_vClassifier){
    ATH_MSG_INFO("TauTrackRNNClassifier tool : " << cClassifier );
    ATH_CHECK(cClassifier.retrieve());
  }
 
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::execute(xAOD::TauJet& xTau)
{
  // Get track container via link from tau - instead of using read handle (not written to store yet) 
  // Check that size > 0
  ElementLink< xAOD::TauTrackContainer > link;
  xAOD::TauTrackContainer* tauTrackCon = nullptr;
  if (xTau.allTauTrackLinks().size() > 0) {
    link = xTau.allTauTrackLinks().at(0); //we don't care about this specific link, just the container
    tauTrackCon = link.getDataNonConstPtr();
  }  

  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, tauTrackCon);

  for (xAOD::TauTrack* xTrack : vTracks)
  {
    // reset all track flags and set status to unclassified
    xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedConversion, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedIsolation, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedFake, false);
    xTrack->setFlag(xAOD::TauJetParameters::unclassified, true);
  }
  
  for (auto cClassifier : m_vClassifier) {
    ATH_CHECK(cClassifier->classifyTracks(vTracks, xTau));
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
// class TrackRNN
//==============================================================================

//______________________________________________________________________________
TrackRNN::TrackRNN(const std::string& sName)
  : TauRecToolBase(sName)
  , m_sInputWeightsPath("")
{
  // for conversion compatibility cast nTracks 
  int nMaxNtracks = 0;
  declareProperty( "InputWeightsPath", m_sInputWeightsPath );
  declareProperty( "MaxNtracks",  nMaxNtracks);
  m_nMaxNtracks = (unsigned int)nMaxNtracks;
}

//______________________________________________________________________________
TrackRNN::~TrackRNN()
{
}

//______________________________________________________________________________
StatusCode TrackRNN::finalize()
{
  delete m_RNNClassifier;
  m_RNNClassifier = nullptr;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::initialize()
{
  m_vClassProb = std::vector<double>(5);   
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::classifyTracks(std::vector<xAOD::TauTrack*> vTracks, xAOD::TauJet& xTau)
{
  std::sort(vTracks.begin(), vTracks.end(), [](const xAOD::TauTrack * a, const xAOD::TauTrack * b) {return a->pt() > b->pt();});

  if(vTracks.size() == 0)
    return StatusCode::SUCCESS;

  SG::AuxElement::Accessor<float> idScoreCharged("rnn_chargedScore");
  SG::AuxElement::Accessor<float> idScoreIso("rnn_isolationScore");
  SG::AuxElement::Accessor<float> idScoreConv("rnn_conversionScore");
  SG::AuxElement::Accessor<float> idScoreFake("rnn_fakeScore");
  SG::AuxElement::Accessor<float> idScoreUncl("rnn_unclassifiedScore");

  SG::AuxElement::Accessor<int> nTrkCharged("nRnnChargedTracks");
  SG::AuxElement::Accessor<int> nTrkIso("nRnnIsolationTracks");
  SG::AuxElement::Accessor<int> nTrkConv("nRnnConvertionTracks");
  SG::AuxElement::Accessor<int> nTrkFake("nRnnFakeTracks");
  SG::AuxElement::Accessor<int> nTrkUncl("nRnnUnclassifiedTracks");

  ATH_CHECK(setVars(vTracks, xTau));

  SeqNodeMap seqInput;
  NodeMap nodeInput;
  
  seqInput["node_0"] = m_valueMap; 

  VectorMap mValue = m_RNNClassifier->scan(nodeInput, seqInput, "time_distributed_3_0");
  ValueMap propValue = m_RNNClassifier->compute(nodeInput, seqInput, "dense_5_0");

  int nChargedTracks = 0;
  int nIsoTracks = 0;
  int nConvTracks = 0;
  int nFakeTracks = 0;
  int nUnclTracks = 0;
  for (unsigned int i = 0; i < vTracks.size(); ++i){

    if(i >= m_nMaxNtracks && m_nMaxNtracks > 0){
      m_vClassProb[0] = 0.0;
      m_vClassProb[1] = 0.0;
      m_vClassProb[2] = 0.0;
      m_vClassProb[3] = 0.0;
      m_vClassProb[4] = 1.0;
    }else{
      m_vClassProb[0] = mValue["type_1"][i];
      m_vClassProb[1] = mValue["type_2"][i];
      m_vClassProb[2] = mValue["type_3"][i];
      m_vClassProb[3] = mValue["type_4"][i];
      m_vClassProb[4] = 0.0;
    }

    idScoreCharged(*vTracks[i]) = m_vClassProb[0];
    idScoreConv(*vTracks[i]) = m_vClassProb[1];
    idScoreIso(*vTracks[i]) = m_vClassProb[2];
    idScoreFake(*vTracks[i]) = m_vClassProb[3];
    idScoreUncl(*vTracks[i]) = m_vClassProb[4];

    int iMaxIndex = 3; // for safty reasons set this to FT to circumvent bias
    for (unsigned int j = 0; j < m_vClassProb.size(); ++j){
      if(m_vClassProb[j] > m_vClassProb[iMaxIndex]) iMaxIndex = j;
    }

    if(iMaxIndex == 3){
        vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedFake, true);
        nFakeTracks++;
      }else if(iMaxIndex == 0){
        vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedCharged, true);
        nChargedTracks++;
      }else if(iMaxIndex == 1){
        vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedConversion, true);
        nConvTracks++;
      }else if(iMaxIndex == 2){
        vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedIsolation, true);
        nIsoTracks++;
      }else if(iMaxIndex == 4){
        vTracks[i]->setFlag(xAOD::TauJetParameters::unclassified, true);
        nUnclTracks++;
      }
    }

    nTrkCharged(xTau) = nChargedTracks;
    nTrkIso(xTau) = nIsoTracks;
    nTrkConv(xTau) = nConvTracks;
    nTrkFake(xTau) = nFakeTracks;
    nTrkUncl(xTau) = nUnclTracks;


    ATH_CHECK(resetVars());
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
//StatusCode TrackRNN::classifyTriggerTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track, double mu)
//{
//  // NOT YET IMPLEMENTED 
//  return StatusCode::SUCCESS;
//}

//______________________________________________________________________________
StatusCode TrackRNN::addWeightsFile()
{

  std::string sInputWeightsPath = find_file(m_sInputWeightsPath);
  ATH_MSG_DEBUG("InputWeightsPath: " << sInputWeightsPath);

  std::ifstream nn_config_istream(sInputWeightsPath);
  
  m_NNconfig = lwtDev::parse_json_graph(nn_config_istream);
  
  m_RNNClassifier = new lwtDev::LightweightGraph(m_NNconfig, m_NNconfig.outputs.begin()->first);

  if(m_RNNClassifier==0) {
    ATH_MSG_FATAL("Couldn't configure neural network!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::setVars(const std::vector<xAOD::TauTrack*> vTracks, const xAOD::TauJet& xTau)
{

  // initialize m_valueMap
  unsigned int n_timeSteps = vTracks.size();
  if(m_nMaxNtracks > 0 && n_timeSteps>m_nMaxNtracks)
    n_timeSteps = m_nMaxNtracks;


  m_valueMap["log(tautracks.trackPt)"] = std::vector<double>(n_timeSteps);
  m_valueMap["log(tautracks.jetSeedPt)"] = std::vector<double>(n_timeSteps);
  m_valueMap["log(tautracks.trackPt/tautracks.jetSeedPt)"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.trackEta"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.z0sinThetaTJVA"] = std::vector<double>(n_timeSteps);
  m_valueMap["log(tautracks.rConv)"] = std::vector<double>(n_timeSteps);
  m_valueMap["tanh(tautracks.rConvII/500)"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.dRJetSeedAxis"] = std::vector<double>(n_timeSteps);
  m_valueMap["tanh(tautracks.d0/10)"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.qOverP*1000"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.numberOfInnermostPixelLayerHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.numberOfPixelSharedHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.numberOfSCTSharedHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.numberOfTRTHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.eProbabilityHT"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.nPixHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.nSiHits"] = std::vector<double>(n_timeSteps);
  m_valueMap["tautracks.charge"] = std::vector<double>(n_timeSteps);


  unsigned int i = 0;
  for(xAOD::TauTrack* xTrack : vTracks)
  {
    const xAOD::TrackParticle* xTrackParticle = xTrack->track();

    double fTauSeedPt = xTau.ptJetSeed();
    double fTrackPt = xTrackParticle->pt();
    double fTrackEta = xTrackParticle->eta();
    double fTrackCharge = xTrackParticle->charge();
    double fZ0SinthetaTJVA = xTrack->z0sinThetaTJVA(xTau);
    double fRConv = xTrack->rConv(xTau);
    double fRConvII = xTrack->rConvII(xTau);
    double fDRJetSeedAxis = xTrack->dRJetSeedAxis(xTau);
    double fD0 = xTrackParticle->d0();
    double fQoverP = xTrackParticle->qOverP();

    uint8_t iTracksNumberOfInnermostPixelLayerHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNumberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits), StatusCode::FAILURE );
    uint8_t iTracksNPixelHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelHits, xAOD::numberOfPixelHits), StatusCode::FAILURE );
    uint8_t iTracksNPixelSharedHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelSharedHits, xAOD::numberOfPixelSharedHits), StatusCode::FAILURE );
    uint8_t iTracksNPixelDeadSensors = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelDeadSensors, xAOD::numberOfPixelDeadSensors), StatusCode::FAILURE );
    uint8_t iTracksNSCTHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTHits, xAOD::numberOfSCTHits), StatusCode::FAILURE );
    uint8_t iTracksNSCTSharedHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTSharedHits, xAOD::numberOfSCTSharedHits), StatusCode::FAILURE );
    uint8_t iTracksNSCTDeadSensors = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTDeadSensors, xAOD::numberOfSCTDeadSensors), StatusCode::FAILURE );
    uint8_t iTracksNTRTHighThresholdHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue( iTracksNTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits), StatusCode::FAILURE );
    uint8_t iTracksNTRTHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue( iTracksNTRTHits, xAOD::numberOfTRTHits), StatusCode::FAILURE );
    uint8_t iNumberOfContribPixelLayers = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfContribPixelLayers, xAOD::numberOfContribPixelLayers), StatusCode::FAILURE );
    uint8_t iNumberOfPixelHoles = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfPixelHoles, xAOD::numberOfPixelHoles), StatusCode::FAILURE );
    uint8_t iNumberOfSCTHoles = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfSCTHoles, xAOD::numberOfSCTHoles), StatusCode::FAILURE );
  
    float fTracksNumberOfInnermostPixelLayerHits = (float)iTracksNumberOfInnermostPixelLayerHits;
    float fTracksNPixelHits = (float)iTracksNPixelHits;
    float fTracksNPixelDeadSensors = (float)iTracksNPixelDeadSensors;
    float fTracksNPixelSharedHits = (float)iTracksNPixelSharedHits;
    float fTracksNSCTHits = (float)iTracksNSCTHits;
    float fTracksNSCTDeadSensors = (float)iTracksNSCTDeadSensors;
    float fTracksNSCTSharedHits = (float)iTracksNSCTSharedHits;
    //float fTracksNTRTHighThresholdHits = (float)iTracksNTRTHighThresholdHits;
    float fTracksNTRTHits = (float)iTracksNTRTHits;
  
    float fTracksNPixHits = fTracksNPixelHits + fTracksNPixelDeadSensors;
    float fTracksNSiHits = fTracksNPixelHits + fTracksNPixelDeadSensors + fTracksNSCTHits + fTracksNSCTDeadSensors;
  
    float fTracksEProbabilityHT; TRT_CHECK_BOOL( xTrackParticle->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT), StatusCode::FAILURE );
  
    //float fNumberOfContribPixelLayers = float(iNumberOfContribPixelLayers);
    //float fNumberOfPixelHoles = float(iNumberOfPixelHoles);
    //float fNumberOfSCTHoles = float(iNumberOfSCTHoles);

    m_valueMap["log(tautracks.trackPt)"][i] = log(fTrackPt);
    m_valueMap["log(tautracks.jetSeedPt)"][i] = log(fTauSeedPt);
    m_valueMap["log(tautracks.trackPt/tautracks.jetSeedPt)"][i] = log(fTrackPt/fTauSeedPt);
    m_valueMap["tautracks.trackEta"][i] = fTrackEta;
    m_valueMap["tautracks.z0sinThetaTJVA"][i] = fZ0SinthetaTJVA;
    m_valueMap["log(tautracks.rConv)"][i] = log(fRConv);
    m_valueMap["tanh(tautracks.rConvII/500)"][i] = tanh(fRConvII/500.0);
    m_valueMap["tautracks.dRJetSeedAxis"][i] = fDRJetSeedAxis;
    m_valueMap["tanh(tautracks.d0/10)"][i] = tanh(fD0/10);
    m_valueMap["tautracks.qOverP*1000"][i] = fQoverP*1000.0;
    m_valueMap["tautracks.numberOfInnermostPixelLayerHits"][i] = fTracksNumberOfInnermostPixelLayerHits;
    m_valueMap["tautracks.numberOfPixelSharedHits"][i] = fTracksNPixelSharedHits;
    m_valueMap["tautracks.numberOfSCTSharedHits"][i] = fTracksNSCTSharedHits;
    m_valueMap["tautracks.numberOfTRTHits"][i] = fTracksNTRTHits;
    m_valueMap["tautracks.eProbabilityHT"][i] = fTracksEProbabilityHT;
    m_valueMap["tautracks.nPixHits"][i] = fTracksNPixHits;
    m_valueMap["tautracks.nSiHits"][i] = fTracksNSiHits;
    m_valueMap["tautracks.charge"][i] = fTrackCharge;

    ++i;
    if(m_nMaxNtracks > 0 && i >= m_nMaxNtracks)
      break;

  }

  return StatusCode::SUCCESS;
} 

StatusCode TrackRNN::resetVars(){

  for(auto varIt=m_valueMap.begin(); varIt!=m_valueMap.end(); varIt++)
    (varIt->second).clear();

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
//StatusCode TrackRNN::setTriggerVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track)
//{
//  // NOT YET IMPLEMENTED
//
//  return StatusCode::SUCCESS;
//}
