/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  : TauRecToolBase(sName) {
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
StatusCode TauTrackRNNClassifier::executeRNNTrackClassifier(xAOD::TauJet& xTau, xAOD::TauTrackContainer& tauTrackCon) const {

  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, &tauTrackCon);

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
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::classifyTracks(std::vector<xAOD::TauTrack*>& vTracks, xAOD::TauJet& xTau) const
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

  VectorMap valueMap;
  ATH_CHECK(calulateVars(vTracks, xTau, valueMap));

  SeqNodeMap seqInput;
  NodeMap nodeInput;
  
  seqInput["input_1"] = valueMap; 

  VectorMap mValue = m_RNNClassifier->scan(nodeInput, seqInput, "time_distributed_2");

  std::vector<double> vClassProb(5);

  int nChargedTracks = 0;
  int nIsoTracks = 0;
  int nConvTracks = 0;
  int nFakeTracks = 0;
  int nUnclTracks = 0;
  for (unsigned int i = 0; i < vTracks.size(); ++i){

    if(i >= m_nMaxNtracks && m_nMaxNtracks > 0){
      vClassProb[0] = 0.0;
      vClassProb[1] = 0.0;
      vClassProb[2] = 0.0;
      vClassProb[3] = 0.0;
      vClassProb[4] = 1.0;
    }else{
      vClassProb[0] = mValue["type_0"][i];
      vClassProb[1] = mValue["type_1"][i];
      vClassProb[2] = mValue["type_2"][i];
      vClassProb[3] = mValue["type_3"][i];
      vClassProb[4] = 0.0;
    }

    idScoreCharged(*vTracks[i]) = vClassProb[0];
    idScoreConv(*vTracks[i]) = vClassProb[1];
    idScoreIso(*vTracks[i]) = vClassProb[2];
    idScoreFake(*vTracks[i]) = vClassProb[3];
    idScoreUncl(*vTracks[i]) = vClassProb[4];

    int iMaxIndex = 3; // for safty reasons set this to FT to circumvent bias
    for (unsigned int j = 0; j < vClassProb.size(); ++j){
      if(vClassProb[j] > vClassProb[iMaxIndex]) iMaxIndex = j;
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
StatusCode TrackRNN::calulateVars(const std::vector<xAOD::TauTrack*>& vTracks, const xAOD::TauJet& xTau, tauRecTools::VectorMap& valueMap) const
{

  // initialize map with values
  valueMap.clear();
  unsigned int n_timeSteps = vTracks.size();
  if(m_nMaxNtracks > 0 && n_timeSteps>m_nMaxNtracks)
    n_timeSteps = m_nMaxNtracks;

  valueMap["log(trackPt)"] = std::vector<double>(n_timeSteps);
  valueMap["log(jetSeedPt)"] = std::vector<double>(n_timeSteps);
  valueMap["(trackPt/jetSeedPt[0])"] = std::vector<double>(n_timeSteps);
  valueMap["trackEta"] = std::vector<double>(n_timeSteps);
  valueMap["z0sinThetaTJVA"] = std::vector<double>(n_timeSteps);
  valueMap["log(rConv)"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(rConvII/500)"] = std::vector<double>(n_timeSteps);
  valueMap["dRJetSeedAxis"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(d0/10)"] = std::vector<double>(n_timeSteps);
  valueMap["qOverP*1000"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfInnermostPixelLayerHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfPixelSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfSCTSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfTRTHits"] = std::vector<double>(n_timeSteps);
  valueMap["eProbabilityHT"] = std::vector<double>(n_timeSteps);
  valueMap["nPixHits"] = std::vector<double>(n_timeSteps);
  valueMap["nSiHits"] = std::vector<double>(n_timeSteps);
  valueMap["charge"] = std::vector<double>(n_timeSteps);


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
    //float fTracksNTRTHighThresholdHits = (float)iTracksNTRTHighThresholdHits;
    float fTracksNTRTHits = (float)iTracksNTRTHits;
  
    float fTracksNPixHits = fTracksNPixelHits + fTracksNPixelDeadSensors;
    float fTracksNSiHits = fTracksNPixelHits + fTracksNPixelDeadSensors + fTracksNSCTHits + fTracksNSCTDeadSensors;
  
    float fTracksEProbabilityHT; ATH_CHECK( xTrackParticle->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT) );
  
    //float fNumberOfContribPixelLayers = float(iNumberOfContribPixelLayers);
    //float fNumberOfPixelHoles = float(iNumberOfPixelHoles);
    //float fNumberOfSCTHoles = float(iNumberOfSCTHoles);

    valueMap["log(trackPt)"][i] = log(fTrackPt);
    valueMap["log(jetSeedPt)"][i] = log(fTauSeedPt);
    valueMap["(trackPt/jetSeedPt[0])"][i] = (fTrackPt/fTauSeedPt);
    valueMap["trackEta"][i] = fTrackEta;
    valueMap["z0sinThetaTJVA"][i] = fZ0SinthetaTJVA;
    valueMap["log(rConv)"][i] = log(fRConv);
    valueMap["tanh(rConvII/500)"][i] = tanh(fRConvII/500.0);
    valueMap["dRJetSeedAxis"][i] = fDRJetSeedAxis;
    valueMap["tanh(d0/10)"][i] = tanh(fD0/10);
    valueMap["qOverP*1000"][i] = fQoverP*1000.0;
    valueMap["numberOfInnermostPixelLayerHits"][i] = fTracksNumberOfInnermostPixelLayerHits;
    valueMap["numberOfPixelSharedHits"][i] = fTracksNPixelSharedHits;
    valueMap["numberOfSCTSharedHits"][i] = fTracksNSCTSharedHits;
    valueMap["numberOfTRTHits"][i] = fTracksNTRTHits;
    valueMap["eProbabilityHT"][i] = fTracksEProbabilityHT;
    valueMap["nPixHits"][i] = fTracksNPixHits;
    valueMap["nSiHits"][i] = fTracksNSiHits;
    valueMap["charge"][i] = fTrackCharge;

    ++i;
    if(m_nMaxNtracks > 0 && i >= m_nMaxNtracks)
      break;
  }

  return StatusCode::SUCCESS;
} 

//______________________________________________________________________________
//StatusCode TrackRNN::setTriggerVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track)
//{
//  // NOT YET IMPLEMENTED
//
//  return StatusCode::SUCCESS;
//}
