/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ROOT
#include "TH1.h"

// xAOD
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

// Local
#include "DecoratePromptLeptonRNN.h"
#include "PromptUtils.h"

using namespace std;

//=============================================================================
Prompt::DecoratePromptLeptonRNN::DecoratePromptLeptonRNN(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_toolRNN   (this),
  m_histSvc   ("THistSvc/THistSvc", name),
  m_countEvent(0)
{

  declareProperty("inputContainerLepton",          m_inputContainerLepton,          "Container's name of the lepton that you want to decorate");
  declareProperty("inputContainerTrack",           m_inputContainerTrack,           "Track container name");
  declareProperty("inputContainerTrackJet",        m_inputContainerTrackJet,        "Track Jet container name");
  declareProperty("inputContainerPrimaryVertices", m_inputContainerPrimaryVertices, "Primary vertex container name");

  declareProperty("outputStream",                  m_outputStream,                  "Path of the ROOT output directory of the histograms for RNN debug");  
  declareProperty("decorationPrefixRNN",           m_decorationPrefixRNN,           "Prefix of the name for the decorator of RNN to the lepton");

  declareProperty("minTrackpT",                    m_minTrackpT            = 500.0, "pT cut config for the input tracks to RNN" );
  declareProperty("maxTrackEta",                   m_maxTrackEta           = 2.5,   "abs(eta) cut config for the input tracks to RNN" );
  declareProperty("maxTrackZ0Sin",                 m_maxTrackZ0Sin         = 1.0,   "Z0sin cut config for the input tracks to RNN" );
  declareProperty("minTrackLeptonDR",              m_minTrackLeptonDR      = 1.0e-6,"Delta R between lepton and track cut config for the input tracks to RNN" );
  declareProperty("maxTrackLeptonDR",              m_maxTrackLeptonDR      = 0.4,   "Delta R between lepton and track cut config for the input tracks to RNN" );
  declareProperty("minTrackSilHits",               m_minTrackSilHits       = 6.5,   "track sillicon detector hits cut config for the input tracks to RNN" );
  declareProperty("maxTrackSharedSilHits",         m_maxTrackSharedSilHits = 1.5,   "track shared si hits cut config for the input tracks to RNN" );
  declareProperty("maxTrackSilHoles",              m_maxTrackSilHoles      = 2.5,   "track holes cut config for the input tracks to RNN" );
  declareProperty("maxTrackPixHoles",              m_maxTrackPixHoles      = 1.5,   "track pixel holes cut config for the input tracks to RNN" );

  declareProperty("maxLepTrackJetDR",              m_maxLepTrackJetDR      = 0.4,   "Maximum distance between lepton and track jet for track jet matching");

  declareProperty("toolRNN",                       m_toolRNN,                       "Dedicated tool for RNN prediction");
  declareProperty("debug",                         m_debug     = false,             "debug statement");  
  declareProperty("printTime",                     m_printTime = false,             "print running time, for debug");  
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonRNN::initialize()
{  
  //
  // Initialize tools and services
  //
  ATH_CHECK( m_histSvc.retrieve() );

  for(const std::string &label: m_toolRNN->getOutputLabels()) {
    const std::string key = m_decorationPrefixRNN + label;

    ATH_MSG_INFO("Add output RNN label: \"" << key << "\"");

    m_decoratorMap.insert(decoratorFloatMap_t::value_type(key, std::make_unique<decoratorFloat_t>(key)));
  }

  ATH_MSG_INFO("inputContainerMuon=\"" << m_inputContainerLepton << "\"");
  
  //
  // Instantiate Muon quality accessors
  //
  m_accessQuality = std::make_unique<SG::AuxElement::ConstAccessor<unsigned char> >("quality");

  m_timerEvent.Reset();

  return StatusCode::SUCCESS;

}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonRNN::execute()
{
  //
  // Process current event
  //
  TimerScopeHelper timer(m_timerEvent);

  if(m_debug) {
    ATH_MSG_DEBUG("execute() - begin...");
  }

  m_countEvent++;

  //
  // Retrieve object containers and event info
  //
  const xAOD::IParticleContainer     *leptons   = nullptr;
  const xAOD::TrackParticleContainer *tracks    = nullptr;
  const xAOD::JetContainer           *trackJets = nullptr;
  const xAOD::VertexContainer        *vertices  = nullptr;
  const DataHandle<xAOD::EventInfo>   event_handle;
  
  CHECK(evtStore()->retrieve(leptons,   m_inputContainerLepton));
  CHECK(evtStore()->retrieve(tracks,    m_inputContainerTrack));
  CHECK(evtStore()->retrieve(trackJets, m_inputContainerTrackJet));
  CHECK(evtStore()->retrieve(vertices,  m_inputContainerPrimaryVertices));
  CHECK(evtStore()->retrieve(event_handle));

  ATH_MSG_DEBUG("Size of LeptonContainer: " << leptons->size());

  //
  // Find default Primary Vertex
  //
  const xAOD::Vertex *primaryVertex = nullptr;

  for(const xAOD::Vertex *vertex: *vertices) {
    if(vertex->vertexType() == 1) {
      primaryVertex = vertex;
      break;
    } 
  } 

  //
  // Collect tracks
  //
  for(const xAOD::IParticle *lepton: *leptons) {
    //
    // Select lepton track
    //
    const xAOD::TrackParticle *trackLep = nullptr;
    const xAOD::Electron      *elec     = dynamic_cast<const xAOD::Electron*>(lepton);
    const xAOD::Muon          *muon     = dynamic_cast<const xAOD::Muon    *>(lepton);
    
    if(elec) {
      const xAOD::TrackParticle *bestmatchedGSFElTrack = elec->trackParticle(0);
      if(bestmatchedGSFElTrack) {
        trackLep = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(bestmatchedGSFElTrack);
      } 
    }
    else if (muon) {
      trackLep = findMuonTrack(muon);
    }
    else {
      ATH_MSG_WARNING("execute - failed to find electron or muon: should never happen!");          
    }

    //
    // Find closest track jet
    //
    const xAOD::Jet *trackJet = findClosestTrackJet(trackLep, *trackJets);

    if(!trackLep || !trackJet) {
      compDummy(*lepton, m_decorationPrefixRNN);
      continue;
    }
    
    //
    // Select tracks within cone around lepton track. 
    //
    std::vector<Prompt::VarHolder > select_tracks;
    
    Prompt::VarHolder lepton_obj;

    if(!prepTrackObject(lepton_obj, *trackLep, *trackLep, *trackJet, primaryVertex, *event_handle)) { 
      continue;
    }
    
    // 
    // Add lepton track as one of the cone tracks.
    //
    select_tracks.push_back(lepton_obj);

    for(const xAOD::TrackParticle *track: *tracks) {

      if(!track) {
        ATH_MSG_WARNING("Prompt::DecoratePromptLeptonRNN::execute - skip null track pointer - should never happen");
        continue;
      }   

      Prompt::VarHolder track_obj;
      
      if(!prepTrackObject(track_obj, *track, *trackLep, *trackJet, primaryVertex, *event_handle)) {
	continue;
      }
      
      if(passTrack(track_obj)) {
	select_tracks.push_back(track_obj);

	ATH_MSG_DEBUG("Prompt::DecoratePromptLeptonRNN::execute - passed track pT= " << track->pt());	
      }
    }

    //
    // Sort tracks by DR distance to lepton
    //
    std::sort(select_tracks.begin(), select_tracks.end(), Prompt::SortObjectByVar(Def::LepTrackDR, msg()));
    
    //
    // Compute RNN
    //
    compScore(*lepton, select_tracks, m_decorationPrefixRNN);

    if(m_debug) {
      ATH_MSG_DEBUG("DecoratePromptLeptonRNN::CompScore - " << std::endl
                      << "lepton pT= " << lepton->pt()
		      << ", number of tracks: " << select_tracks.size());
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonRNN::finalize()
{
  //
  // Finalize output
  //
  if(m_printTime) {
    //  
    // Print full time stopwatch
    //  
    ATH_MSG_INFO("DecoratePromptLeptonRNN - total time:   " << PrintResetStopWatch(m_timerEvent));
    ATH_MSG_INFO("DecoratePromptLeptonRNN - processed "     << m_countEvent << " events.");
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
const xAOD::TrackParticle* Prompt::DecoratePromptLeptonRNN::findMuonTrack(const xAOD::Muon *muon)
{
  //
  // Process muon - return true if all information present for RNN
  //
  if(muon->muonType() != xAOD::Muon::Combined || !muon->inDetTrackParticleLink().isValid()) {
    return 0;
  }

  const unsigned char quality = (*m_accessQuality)(*muon);
  
  ATH_MSG_DEBUG("muon pT=" << muon->pt() << " quality=" << int(quality) << " medium=" << int(xAOD::Muon::Medium));
  
  const xAOD::TrackParticle *trackLep = *(muon->inDetTrackParticleLink());
  
  return trackLep;
}

//=============================================================================
const xAOD::Jet* Prompt::DecoratePromptLeptonRNN::findClosestTrackJet(const xAOD::TrackParticle *particle,
						                      const xAOD::JetContainer &trackJets)
{
  //
  // Find track jet closest to IParticle and within fixed cone
  //
  if(!particle) {
    return 0;
  }

  const xAOD::Jet *trackJet = 0;    
  double currTrackJetDR = -1.0;
  
  //
  // Select track jet within cone around muon
  //   
  for(const xAOD::Jet *jet: trackJets) {
    const double dr = particle->p4().DeltaR(jet->p4());
    
    if(currTrackJetDR < 0.0 || dr < currTrackJetDR) {
      trackJet       = jet;
      currTrackJetDR = dr;	
    }     
  } 

  if(trackJet && currTrackJetDR < m_maxLepTrackJetDR) {
    return trackJet;
  }

  return 0;
}

//=============================================================================
bool Prompt::DecoratePromptLeptonRNN::prepTrackObject(Prompt::VarHolder         &p,
			                              const xAOD::TrackParticle &track,
			                              const xAOD::TrackParticle &lepton,
			                              const xAOD::Jet           &trackJet,
			                              const xAOD::Vertex        *priVtx,
						      const xAOD::EventInfo     &event)
{ 
  //
  // Add xAOD::TrackParticle variables to the track object VarHolder
  //
  uint8_t numberOfPixelHits       = 0;
  uint8_t numberOfSCTHits         = 0;
  uint8_t numberOfPixelHoles      = 0;
  uint8_t numberOfSCTHoles        = 0;  
  uint8_t numberOfPixelSharedHits = 0;
  uint8_t numberOfSCTSharedHits   = 0;
  
  if(!(track.summaryValue(numberOfPixelHits,       xAOD::numberOfPixelHits)))       return false;
  if(!(track.summaryValue(numberOfSCTHits,         xAOD::numberOfSCTHits)))         return false;
  if(!(track.summaryValue(numberOfPixelHoles,      xAOD::numberOfPixelHoles)))      return false;
  if(!(track.summaryValue(numberOfSCTHoles,        xAOD::numberOfSCTHoles)))        return false;
  if(!(track.summaryValue(numberOfPixelSharedHits, xAOD::numberOfPixelSharedHits))) return false;
  if(!(track.summaryValue(numberOfSCTSharedHits,   xAOD::numberOfSCTSharedHits)))   return false;
  
  const uint8_t NSiHits   = numberOfPixelHits  + numberOfSCTHits;
  const uint8_t NSiHoles  = numberOfPixelHoles + numberOfSCTHoles;
  const float   NSiShHits = float(numberOfPixelSharedHits) + float(numberOfSCTSharedHits)/2.0;

  p.AddVar(Def::Pt,                    track.pt());
  p.AddVar(Def::AbsEta,                std::fabs(track.eta()));
  p.AddVar(Def::NumberOfPIXHits,       numberOfPixelHits);
  p.AddVar(Def::NumberOfSCTHits,       numberOfSCTHits);  
  p.AddVar(Def::NumberOfSiHits,        NSiHits);
  p.AddVar(Def::NumberOfSharedSiHits,  NSiShHits);  
  p.AddVar(Def::NumberOfSiHoles,       NSiHoles);
  p.AddVar(Def::NumberOfPixelHoles,    numberOfPixelHoles); 

  //
  // Add lepton - jet variables to VarHolder
  //
  double PtFrac = -99.;

  if(track.pt() > 0.0 && trackJet.pt() > 0.0) {
    PtFrac = track.pt() / trackJet.pt();
  }

  p.AddVar(Def::TrackPtOverTrackJetPt, PtFrac);
  p.AddVar(Def::TrackJetDR,            track.p4().DeltaR(trackJet.p4())); 
  p.AddVar(Def::LepTrackDR,            track.p4().DeltaR(lepton.p4()));
  
  //
  // Add Impact Parameters
  //
  double d0_significance = -99.;
  double Z0Sin           = 0.0;

  if(track.definingParametersCovMatrixVec().size() > 0 && track.definingParametersCovMatrixVec().at(0) > 0.0) {
    d0_significance = xAOD::TrackingHelpers::d0significance(&track, 
							    event.beamPosSigmaX(),
							    event.beamPosSigmaY(), 
							    event.beamPosSigmaXY());
  }

  if(priVtx) {
    const double deltaZ0  = track.z0() + track.vz() - priVtx->z();
    Z0Sin                 = deltaZ0*std::sin(track.theta());
  }
  
  p.AddVar(Def::Z0Sin,  Z0Sin);
  p.AddVar(Def::D0Sig,  d0_significance);

  return true;   
}

//=============================================================================
bool Prompt::DecoratePromptLeptonRNN::passTrack(Prompt::VarHolder &p)
{ 
  //
  // Select cone tracks
  //
  if(p.GetVar(Def::LepTrackDR)           < m_minTrackLeptonDR)     return false;
  if(p.GetVar(Def::LepTrackDR)           > m_maxTrackLeptonDR)     return false;
  
  //  
  // Kinematic track selection
  //  
  if(p.GetVar(Def::Pt)                   < m_minTrackpT)           return false;
  if(p.GetVar(Def::AbsEta)               > m_maxTrackEta)          return false;
  if(std::fabs(p.GetVar(Def::Z0Sin))     > m_maxTrackZ0Sin)        return false;

  //  
  // Hit quality track selection
  //  
  if(p.GetVar(Def::NumberOfSiHits)       < m_minTrackSilHits)       return false;
  if(p.GetVar(Def::NumberOfSharedSiHits) > m_maxTrackSharedSilHits) return false;
  if(p.GetVar(Def::NumberOfSiHoles)      > m_maxTrackSilHoles)      return false;
  if(p.GetVar(Def::NumberOfPixelHoles)   > m_maxTrackPixHoles)      return false;

  return true;
}

//=============================================================================
bool Prompt::DecoratePromptLeptonRNN::compScore(const xAOD::IParticle &particle, 
			                        const std::vector<Prompt::VarHolder> &tracks,
                        			const std::string &prefix)
{ 
  //
  // Call the RNN tool to get the RNN prediction for the leptons and decorate the lepton with those RNN scores.
  //
  if(m_debug) {
    ATH_MSG_DEBUG("compScore - number of tracks: " << tracks.size());

    for(const Prompt::VarHolder &o: tracks) {
      ATH_MSG_DEBUG("compScore - track: LepTrackDR = " << o.GetVar(Def::LepTrackDR) 
		    << ", TrackJetDR = " << o.GetVar(Def::TrackJetDR)
		    << ", D0Sig = " << o.GetVar(Def::D0Sig) 
		    << ", Z0Sin = " << o.GetVar(Def::Z0Sin) 
		    << ", NumberOfPIXHits = " << o.GetVar(Def::NumberOfPIXHits) 
		    << ", NumberOfSCTHits = " << o.GetVar(Def::NumberOfSCTHits) 
		    << ", PtFrac = " << o.GetVar(Def::TrackPtOverTrackJetPt) );
    }
  }

  const std::map<std::string, double> results = m_toolRNN->computeRNNOutput(tracks);

  for(const std::map<std::string, double>::value_type v: results) {
    //
    // Decorate muon
    //
    const string dkey = prefix + v.first;

    ATH_MSG_DEBUG("DecoratePromptLeptonRNN compScore - " << v.first << " = " << v.second );

    decoratorFloatMap_t::iterator dit = m_decoratorMap.find(dkey);
    
    if(dit != m_decoratorMap.end()) {
      (*dit->second)(particle) = v.second;
    }
    else {
      ATH_MSG_WARNING("CompScore - unknown output label=\"" << dkey << "\"");
    }

    if(m_debug) {
      std::map<std::string, TH1*>::iterator hit = m_hists.find(v.first);
      
      if(hit == m_hists.end()) {
	TH1* h = 0;

	CHECK( makeHist(h, v.first, 100, 0.0, 1.0) );
	
	hit = m_hists.insert(std::map<std::string, TH1*>::value_type(v.first, h)).first;
      }
      
      if(hit->second) {
	hit->second->Fill(v.second);
      }
    }
  }
  
  return true;
}

//=============================================================================
bool Prompt::DecoratePromptLeptonRNN::compDummy(const xAOD::IParticle &particle, 
			                        const std::string &prefix)
{
  //
  // Fill dummy values for RNN outputs 
  //
  for(const decoratorFloatMap_t::value_type &v: m_decoratorMap) {
    //
    // Decorate muon
    //
    const string dkey = prefix + v.first;
    
    (*v.second)(particle) = -1.0;
  }
  
  return true;
}

//=============================================================================
StatusCode Prompt::DecoratePromptLeptonRNN::makeHist(TH1 *&h, const std::string &key, int nbin, double xmin, double xmax)
{
  //
  // Initiliase histogram pointer. If configured to run in validation mode, then create and register histogram
  //
  h = 0;

  if(m_outputStream.empty() || key.empty()) {    
    return StatusCode::SUCCESS;
  }

  const std::string hname    = name() + "_" + key;
  const std::string hist_key = "/"+m_outputStream+"/"+hname;

  h = new TH1D(hname.c_str(), hname.c_str(), nbin, xmin, xmax);
  h->SetDirectory(0);

  return m_histSvc->regHist(hist_key, h);
}
