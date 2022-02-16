/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/HelperFunctions.h"

#include "PathResolver/PathResolver.h"
#include "AsgDataHandles/ReadHandle.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

#include <fstream>

using namespace tauRecTools;

//==============================================================================
// class TauTrackRNNClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackRNNClassifier::TauTrackRNNClassifier(const std::string& name)
  : TauRecToolBase(name) {
  declareProperty("classifyLRT", m_classifyLRT = true);
}

//______________________________________________________________________________
TauTrackRNNClassifier::~TauTrackRNNClassifier()
{
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::initialize()
{
  for (const auto& classifier : m_vClassifier){
    ATH_MSG_INFO("Intialize TauTrackRNNClassifier tool : " << classifier );
    ATH_CHECK(classifier.retrieve());
  }

  ATH_CHECK( m_vertexContainerKey.initialize() );
 
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTrackRNNClassifier::executeTrackClassifier(xAOD::TauJet& xTau, xAOD::TauTrackContainer& tauTrackCon) const {

  SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexContainerKey );
  if (!vertexInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << vertexInHandle.key());
      return StatusCode::FAILURE;
   }
  const xAOD::VertexContainer* vertexContainer = vertexInHandle.cptr();

  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, &tauTrackCon);

  for (xAOD::TauTrack* xTrack : vTracks) {
    // reset all track flags and set status to unclassified
    xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedConversion, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedIsolation, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedFake, false);
    xTrack->setFlag(xAOD::TauJetParameters::unclassified, true);
  }

  // don't classify LRTs even if LRTs were associated with taus in TauTrackFinder
  if(!m_classifyLRT) {
    std::vector<xAOD::TauTrack*> vLRTs;
    std::vector<xAOD::TauTrack*>::iterator it = vTracks.begin(); 
    while(it != vTracks.end()) {      
      if((*it)->flag(xAOD::TauJetParameters::LargeRadiusTrack)) {	
	vLRTs.push_back(*it);
        it = vTracks.erase(it);
      }
      else {
	++it;
      }
    }

    // decorate LRTs with default RNN scores
    for (auto classifier : m_vClassifier) {
      ATH_CHECK(classifier->classifyTracks(vLRTs, xTau, vertexContainer, true));
    }
  }

  // classify tracks
  for (auto classifier : m_vClassifier) {
    ATH_CHECK(classifier->classifyTracks(vTracks, xTau, vertexContainer));
  }

  std::vector< ElementLink< xAOD::TauTrackContainer > >& tauTrackLinks(xTau.allTauTrackLinksNonConst());
  std::sort(tauTrackLinks.begin(), tauTrackLinks.end(), sortTracks);
  float charge=0.0;
  for( const xAOD::TauTrack* trk : xTau.tracks(xAOD::TauJetParameters::classifiedCharged) ){
    charge += trk->track()->charge();
  }
  xTau.setCharge(charge);
  xTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) xTau.nTracks());
  xTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) xTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

  // decorations for now, may be turned into Aux
  static const SG::AuxElement::Accessor<int> nTrkConv("nConversionTracks");
  static const SG::AuxElement::Accessor<int> nTrkFake("nFakeTracks");
  nTrkConv(xTau) = (int) xTau.nTracks(xAOD::TauJetParameters::classifiedConversion);
  nTrkFake(xTau) = (int) xTau.nTracks(xAOD::TauJetParameters::classifiedFake);

  //set modifiedIsolationTrack
  for (xAOD::TauTrack* xTrack : vTracks) {
    if( not xTrack->flag(xAOD::TauJetParameters::classifiedCharged) and xTrack->flag(xAOD::TauJetParameters::passTrkSelector) ) {
      xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, true);
    }
    else {
      xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, false);
    }
  }
  xTau.setDetail(xAOD::TauJetParameters::nModifiedIsolationTracks, (int) xTau.nTracks(xAOD::TauJetParameters::modifiedIsolationTrack));

  return StatusCode::SUCCESS;
}

//==============================================================================
// class TrackRNN
//==============================================================================

//______________________________________________________________________________
TrackRNN::TrackRNN(const std::string& name)
  : TauRecToolBase(name)
  , m_inputWeightsPath("")
{
  declareProperty("InputWeightsPath", m_inputWeightsPath = "");
  declareProperty("MaxNtracks", m_nMaxNtracks = 0);
}

//______________________________________________________________________________
TrackRNN::~TrackRNN()
{
}

//______________________________________________________________________________
StatusCode TrackRNN::initialize()
{  
  std::string inputWeightsPath = find_file(m_inputWeightsPath);
  ATH_MSG_INFO("Using calibration file: " << inputWeightsPath);
  
  std::ifstream nn_config_istream(inputWeightsPath);  

  lwtDev::GraphConfig NNconfig = lwtDev::parse_json_graph(nn_config_istream);
  
  m_RNNClassifier = std::make_unique<lwtDev::LightweightGraph>(NNconfig, NNconfig.outputs.begin()->first);
  if(!m_RNNClassifier) {
    ATH_MSG_FATAL("Couldn't configure neural network!");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackRNN::classifyTracks(std::vector<xAOD::TauTrack*>& vTracks,
				    xAOD::TauJet& xTau,
				    const xAOD::VertexContainer* vertexContainer,
				    bool skipTracks) const
{
  if(vTracks.empty()) {
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::Accessor<float> idScoreCharged("rnn_chargedScore");
  static const SG::AuxElement::Accessor<float> idScoreIso("rnn_isolationScore");
  static const SG::AuxElement::Accessor<float> idScoreConv("rnn_conversionScore");
  static const SG::AuxElement::Accessor<float> idScoreFake("rnn_fakeScore");

  // don't classify tracks, set default decorations
  if(skipTracks) {
    for(xAOD::TauTrack* track : vTracks) {
      idScoreCharged(*track) = 0.;
      idScoreConv(*track) = 0.;
      idScoreIso(*track) = 0.;
      idScoreFake(*track) = 0.;
    }
    return StatusCode::SUCCESS;
  }

  std::sort(vTracks.begin(), vTracks.end(), [](const xAOD::TauTrack * a, const xAOD::TauTrack * b) {return a->pt() > b->pt();});

  VectorMap valueMap;
  ATH_CHECK(calulateVars(vTracks, xTau, vertexContainer, valueMap));

  SeqNodeMap seqInput;
  NodeMap nodeInput;
  
  seqInput["input_1"] = valueMap; 

  VectorMap mValue = m_RNNClassifier->scan(nodeInput, seqInput, "time_distributed_2");

  std::vector<double> vClassProb(5);

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

    int iMaxIndex = 0;
    for (unsigned int j = 1; j < vClassProb.size(); ++j){
      if(vClassProb[j] > vClassProb[iMaxIndex]) iMaxIndex = j;
    }

    if(iMaxIndex < 4) {
      vTracks[i]->setFlag(xAOD::TauJetParameters::unclassified, false);
    }

    if(iMaxIndex == 3){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedFake, true);
    }else if(iMaxIndex == 0){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedCharged, true);
    }else if(iMaxIndex == 1){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedConversion, true);
    }else if(iMaxIndex == 2){
      vTracks[i]->setFlag(xAOD::TauJetParameters::classifiedIsolation, true);
    }
  }
  
  return StatusCode::SUCCESS;
}


//______________________________________________________________________________
StatusCode TrackRNN::calulateVars(const std::vector<xAOD::TauTrack*>& vTracks,
				  const xAOD::TauJet& xTau,
				  const xAOD::VertexContainer* vertexContainer,
				  tauRecTools::VectorMap& valueMap) const
{
  // initialize map with values
  valueMap.clear();
  unsigned int n_timeSteps = vTracks.size();
  if(m_nMaxNtracks > 0 && n_timeSteps > m_nMaxNtracks) {
    n_timeSteps = m_nMaxNtracks;
  }

  valueMap["log(trackPt)"] = std::vector<double>(n_timeSteps);
  valueMap["log(jetSeedPt)"] = std::vector<double>(n_timeSteps);
  valueMap["trackPt/tauPtIntermediateAxis"] = std::vector<double>(n_timeSteps);
  valueMap["trackEta"] = std::vector<double>(n_timeSteps);
  valueMap["z0sinthetaTJVA"] = std::vector<double>(n_timeSteps);
  valueMap["z0sinthetaSigTJVA"] = std::vector<double>(n_timeSteps);
  valueMap["log(rConv)"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(rConvII/500)"] = std::vector<double>(n_timeSteps);
  valueMap["dRJetSeedAxis"] = std::vector<double>(n_timeSteps);
  valueMap["dRIntermediateAxis"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(d0SigTJVA/10)"] = std::vector<double>(n_timeSteps);
  valueMap["tanh(d0TJVA/10)"] = std::vector<double>(n_timeSteps);
  valueMap["qOverP*1000"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfInnermostPixelLayerHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfPixelSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfSCTSharedHits"] = std::vector<double>(n_timeSteps);
  valueMap["numberOfTRTHits"] = std::vector<double>(n_timeSteps);
  valueMap["eProbabilityHT"] = std::vector<double>(n_timeSteps);
  valueMap["nPixHits"] = std::vector<double>(n_timeSteps);
  valueMap["nSCTHits"] = std::vector<double>(n_timeSteps);
  valueMap["dz0_TV_PV0"] = std::vector<double>(n_timeSteps);
  valueMap["log_sumpt_TV"] = std::vector<double>(n_timeSteps);
  valueMap["log_sumpt2_TV"] = std::vector<double>(n_timeSteps);
  valueMap["log_sumpt_PV0"] = std::vector<double>(n_timeSteps);
  valueMap["log_sumpt2_PV0"] = std::vector<double>(n_timeSteps);
  valueMap["charge"] = std::vector<double>(n_timeSteps);

  // tau variable
  double log_ptJetSeed = std::log( xTau.ptJetSeed() );

  // vertex variables
  double dz0_TV_PV0 = 0., sumpt_TV = 0., sumpt2_TV = 0., sumpt_PV0 = 0., sumpt2_PV0 = 0.;
  if(vertexContainer != nullptr && !vertexContainer->empty()) {
    dz0_TV_PV0 = xTau.vertex()->z() - vertexContainer->at(0)->z();

    for (const ElementLink<xAOD::TrackParticleContainer>& trk : vertexContainer->at(0)->trackParticleLinks()) {
      sumpt_PV0 += (*trk)->pt();
      sumpt2_PV0 += pow((*trk)->pt(), 2.);
    }
    for (const ElementLink<xAOD::TrackParticleContainer>& trk : xTau.vertex()->trackParticleLinks()) {
      sumpt_TV += (*trk)->pt();
      sumpt2_TV += pow((*trk)->pt(), 2.);
    }
  }

  double log_sumpt_TV = (sumpt_TV>0.) ? std::log(sumpt_TV) : 0.;
  double log_sumpt2_TV = (sumpt2_TV>0.) ? std::log(sumpt2_TV) : 0.;
  double log_sumpt_PV0 = (sumpt_PV0>0.) ? std::log(sumpt_PV0) : 0.;
  double log_sumpt2_PV0 = (sumpt2_PV0>0.) ? std::log(sumpt2_PV0) : 0.;

  // track variables
  unsigned int i = 0;

  for(xAOD::TauTrack* xTrack : vTracks)
    {
      const xAOD::TrackParticle* xTrackParticle = xTrack->track();
      
      uint8_t numberOfInnermostPixelLayerHits = 0; ATH_CHECK( xTrackParticle->summaryValue(numberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits) );
      uint8_t nPixelHits = 0; ATH_CHECK( xTrackParticle->summaryValue(nPixelHits, xAOD::numberOfPixelHits) );
      uint8_t nPixelSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(nPixelSharedHits, xAOD::numberOfPixelSharedHits) );
      uint8_t nPixelDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(nPixelDeadSensors, xAOD::numberOfPixelDeadSensors) );
      uint8_t nSCTHits = 0; ATH_CHECK( xTrackParticle->summaryValue(nSCTHits, xAOD::numberOfSCTHits) );
      uint8_t nSCTSharedHits = 0; ATH_CHECK( xTrackParticle->summaryValue(nSCTSharedHits, xAOD::numberOfSCTSharedHits) );
      uint8_t nSCTDeadSensors = 0; ATH_CHECK( xTrackParticle->summaryValue(nSCTDeadSensors, xAOD::numberOfSCTDeadSensors) );
      uint8_t nTRTHits = 0; ATH_CHECK( xTrackParticle->summaryValue(nTRTHits, xAOD::numberOfTRTHits) );
      float eProbabilityHT; ATH_CHECK( xTrackParticle->summaryValue( eProbabilityHT, xAOD::eProbabilityHT) );

      valueMap["log(trackPt)"][i] = std::log( xTrackParticle->pt() );
      valueMap["log(jetSeedPt)"][i] = log_ptJetSeed;
      valueMap["trackPt/tauPtIntermediateAxis"][i] = xTrackParticle->pt()/xTau.ptIntermediateAxis();
      valueMap["trackEta"][i] = xTrackParticle->eta();
      valueMap["z0sinthetaTJVA"][i] = xTrack->z0sinthetaTJVA();
      valueMap["z0sinthetaSigTJVA"][i] = xTrack->z0sinthetaSigTJVA();
      valueMap["log(rConv)"][i] = std::log( xTrack->rConv() );
      valueMap["tanh(rConvII/500)"][i] = std::tanh( xTrack->rConvII()/500. );
      // there is no seed jets in AOD so dRJetSeedAxis wont work
      valueMap["dRJetSeedAxis"][i] = xTrack->p4().DeltaR(xTau.p4(xAOD::TauJetParameters::JetSeed));
      valueMap["dRIntermediateAxis"][i] = xTrack->p4().DeltaR( xTau.p4(xAOD::TauJetParameters::IntermediateAxis) );
      valueMap["tanh(d0SigTJVA/10)"][i] = std::tanh( xTrack->d0SigTJVA()/10. );
      valueMap["tanh(d0TJVA/10)"][i] = std::tanh( xTrack->d0TJVA()/10. );
      valueMap["qOverP*1000"][i] = xTrackParticle->qOverP()*1000.;
      valueMap["numberOfInnermostPixelLayerHits"][i] = (double) numberOfInnermostPixelLayerHits;
      valueMap["numberOfPixelSharedHits"][i] = (double) nPixelSharedHits;
      valueMap["numberOfSCTSharedHits"][i] = (double) nSCTSharedHits;
      valueMap["numberOfTRTHits"][i] = (double) nTRTHits;
      valueMap["eProbabilityHT"][i] = eProbabilityHT;
      valueMap["nPixHits"][i] = (double) (nPixelHits + nPixelDeadSensors);
      valueMap["nSCTHits"][i] = (double) (nSCTHits + nSCTDeadSensors);
      valueMap["dz0_TV_PV0"][i] = dz0_TV_PV0;
      valueMap["log_sumpt_TV"][i] = log_sumpt_TV;
      valueMap["log_sumpt2_TV"][i] = log_sumpt2_TV;
      valueMap["log_sumpt_PV0"][i] = log_sumpt_PV0;
      valueMap["log_sumpt2_PV0"][i] = log_sumpt2_PV0;
      valueMap["charge"][i] = xTrackParticle->charge();

      ++i;
      if(m_nMaxNtracks > 0 && i >= m_nMaxNtracks) {
	break;
      }
    }

  return StatusCode::SUCCESS;
} 
