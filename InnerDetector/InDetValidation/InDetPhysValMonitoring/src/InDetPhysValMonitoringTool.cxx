/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoringTool.cxx
 * @author shaun roe
**/

#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"

#include <vector>
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h" 
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODJet/JetContainer.h" 
#include <limits>
#include <cmath> //to get std::isnan()

//#include <unordered_map>

using std::pair;
using std::make_pair;

namespace { //utility functions used here
  //get truth particle associated with a track particle
  const xAOD::TruthParticle * getTruthPtr(const xAOD::TrackParticle & trackParticle){
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle * result(nullptr);
		//0. is there any truth?
		if (trackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
			//1. ..then get link
			const ElementTruthLink_t ptruthContainer= trackParticle.auxdata<ElementTruthLink_t>("truthParticleLink" );
			if (ptruthContainer.isValid()){
				result= *ptruthContainer;
			}
		}
    return result;
  }
  
  //get truth/track matching probability
  float getMatchingProbability(const xAOD::TrackParticle & trackParticle){
  	float result(std::numeric_limits<float>::quiet_NaN());
  	if (trackParticle.isAvailable<float>("truthMatchProbability")){
  		result = trackParticle.auxdata<float>("truthMatchProbability" );
  	}
  	return result;
  }
  
  bool isInsideOut(const xAOD::TrackParticle &track){
    std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
    return patternInfo.test(0);
  }
  
  bool truthSelector(const xAOD::TruthParticle &truth){
    const bool pCut = ( truth.pt() >= 500 ); //ORIGINAL SETTING: ( truth.pt() >= 400 );
    const bool stabilityCut = (truth.status() == 1);
    const bool noNeutrals = not (truth.isNeutral());
    return (pCut and stabilityCut and noNeutrals);
  }
  
  bool passJetCuts( const xAOD::Jet& jet ) {
		float etaMin = -2.5;
		float etaMax = 2.5;
		float jetPtMin = 100;  // in GeV
		float jetPtMax = 1000; // in GeV
		float jetPt = jet.pt()/1e3; // GeV
		if( jetPt < jetPtMin ) { return false; }
		if( jetPt > jetPtMax ) { return false; }
		float eta = jet.eta();
		if( eta < etaMin ) { return false; }
		if( eta > etaMax ) { return false; }

		return true;
	}

}//namespace

///Parametrized constructor
InDetPhysValMonitoringTool::InDetPhysValMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent):
    ManagedMonitorToolBase(type, name, parent),
    m_useTrackSelection(false),
    m_onlyInsideOutTracks(false),
    m_truthSelectionTool("TrackTruthSelectionTool/TruthSelectionTool"),
    m_fillTIDEPlots(true),
    m_fillExtraTIDEPlots(false)
{
  declareProperty("TrackParticleContainerName", m_trkParticleName="InDetTrackParticles"); 
  declareProperty("TruthParticleContainerName", m_truthParticleName="TruthParticles"); 
  declareProperty("VertexContainerName", m_vertexContainerName="PrimaryVertices");
  declareProperty("EventInfoContainerName", m_eventInfoContainerName="EventInfo");
  declareProperty("useTrackSelection"       , m_useTrackSelection); //redundant?
  declareProperty("onlyInsideOutTracks"     , m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool"      , m_trackSelectionTool);
  declareProperty("TruthSelectionTool"      , m_truthSelectionTool);
  declareProperty("FillTrackInJetPlots"     , m_fillTIDEPlots);
  declareProperty("FillExtraTrackInJetPlots"     , m_fillExtraTIDEPlots);
  declareProperty("jetContainerName", m_jetContainerName="AntiKt4TruthJets");
  declareProperty("maxTrkJetDR", m_maxTrkJetDR=0.4);
  declareProperty("SubFolder",m_folder);
}

InDetPhysValMonitoringTool::~InDetPhysValMonitoringTool(){
}

StatusCode 
InDetPhysValMonitoringTool::initialize(){
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    //Get the track selector tool only if m_useTrackSelection is true
    if (m_useTrackSelection) ATH_CHECK(m_trackSelectionTool.retrieve());
    ATH_CHECK(m_truthSelectionTool.retrieve());
    m_monPlots = std::move( std::unique_ptr<InDetRttPlots> (new InDetRttPlots(0,"IDPerformanceMon/"+m_folder)) );
    m_monPlots->SetFillExtraTIDEPlots( m_fillExtraTIDEPlots );

    return StatusCode::SUCCESS;
}

StatusCode 
InDetPhysValMonitoringTool::fillHistograms(){
    ATH_MSG_DEBUG ("Filling hists " << name() << "...");
    //retrieve trackParticle container
    auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    if ((!ptracks) ) return StatusCode::FAILURE;
    //retrieve truthParticle container
    const xAOD::TruthParticleContainer* truthParticles = (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
    // if ((!truthParticles) ) return StatusCode::FAILURE;

    const unsigned int nTracks(ptracks->size());
    const unsigned int nTruth(truthParticles  ? truthParticles->size() : 0);
    unsigned int nSelectedTracks(0), num_truthmatch_match(0);
    // the trutht matching probability must not be <= 0., otherwise the tool will seg fault in case of missing truth (e.g. data):
    const float minProbEffLow(0.50); //if the probability of a match is less than this, we call it a fake
    const float minProbEffHigh(0.80); //if the probability of a match is higher than this, it either feeds the NUM or is a duplicate

    //Main track loop, filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes     
    for(const auto & thisTrack: *ptracks){
      if(m_useTrackSelection){   //0 means z0, d0 cut is wrt beam spot - put in a PV to change this   
        if( !(m_trackSelectionTool->accept(*thisTrack, 0))) continue;
      }
      if(m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue; //not an inside-out track                              
      ++nSelectedTracks;                                                    //increment number of selected tracks            
      m_monPlots->fill(*thisTrack);                                         //Make all the plots requiring only trackParticle          
      const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack);
      float prob = getMatchingProbability(*thisTrack);
      //This is where the BMR, Fake, and Really Fake fillers need to go.  
      //m_monPlots->fillBMR_Denom(*thisTrack);
      if(associatedTruth){
	m_monPlots->fillBMR_Denom(*thisTrack);
	if(prob < minProbEffHigh){
	  m_monPlots->fillBMR_Num(*thisTrack);
	  if((prob < minProbEffLow) and (not std::isnan(prob))){
	    const bool isFake = (prob<minProbEffLow);
	    m_monPlots->fillFakeRate(*thisTrack, isFake);
	    if(prob < 0.2){
	      m_monPlots->fillRF_Num(*thisTrack);
	    }
	  }
	}
	if((prob > minProbEffLow)){
	  m_monPlots->fill(*thisTrack, *associatedTruth); //Make all plots requiring both truth & track (meas, res, & pull)                
	}   
      }   
    }

    //This is the beginning of the Nested Loop, built mainly for the Efficiency Plots
    if (truthParticles) {
    for (const auto & thisTruth: *truthParticles){  //Outer loop over all truth particles
      if (m_truthSelectionTool->accept(thisTruth)){
	m_monPlots->fillTruth(*thisTruth);  //DENOMINATOR
	float Big_Match = 0;
	std::vector <pair<float, const xAOD::TrackParticle*> > prospects; //Vector of pairs: <truth_matching_probability, track> if prob > minProbEffLow (0.5)
	for (const auto & thisTrack: *ptracks){ //Inner loop over all track particle
	  if (m_useTrackSelection){
	    //0 means z0, d0 cut is wrt beam spot - put a PV in to change this
	    if( !(m_trackSelectionTool->accept(*thisTrack, 0)) ) continue; 
	  }
	  if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue;  //not an inside-out track
	  const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack); //get the associated truth for this track
	  if(associatedTruth && associatedTruth == thisTruth){
	    float prob = getMatchingProbability(*thisTrack);
	    if(prob > Big_Match) Big_Match = prob;
	    if(prob > minProbEffLow){
	      prospects.push_back(make_pair(prob, thisTrack));
	    }
	  }
	}
	int deg_count = prospects.size();
	if(Big_Match >= minProbEffHigh){
	  ++num_truthmatch_match;
	  const xAOD::TruthParticle * assoc_Truth = getTruthPtr(*prospects.at(0).second);
          if (!assoc_Truth) continue;
          
	  m_monPlots->fill(*assoc_Truth); //This is filling truth-only plots, NUMERATOR & m_TrackTruthInfoPlots 
	  for(int i=0; i<deg_count; i++){
	    if((prospects.at(i).first < Big_Match) and (prospects.at(i).first > minProbEffHigh)){
	      //m_monPlots->fillDupTrack(prospects.at(i).second); //fill the duplicates plots w/ the tracks	     
	    }
	  }
	}
      }  
      
    }//End of Big truthParticle loop    
    //This is the end of the Nested Loop approach section
    }

    if (num_truthmatch_match == 0){
      ATH_MSG_DEBUG("NO TRACKS had associated truth.");
    } else {
      ATH_MSG_DEBUG(num_truthmatch_match <<" tracks out of "<<nTracks<<" had associated truth.");
    }
    m_monPlots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
    m_monPlots->fillCounter(nTracks, InDetPerfPlot_nTracks::ALL);
    m_monPlots->fillCounter(nTruth, InDetPerfPlot_nTracks::TRUTH);
    m_monPlots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);   


    ATH_MSG_DEBUG("Filling vertex plots");
    const xAOD::VertexContainer* pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
    if (pvertex) {
      m_monPlots->fill(*pvertex);
    } else {
      ATH_MSG_WARNING("Cannot open " << m_vertexContainerName << " vertex container. Skipping vertexing plots.");
    }
    ATH_MSG_DEBUG("Filling vertex/event info monitoring plots");
    const xAOD::EventInfo* pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
    if (pei) {
      if (pvertex) {
         m_monPlots->fill(*pvertex, *pei);
      }
    } else {
      ATH_MSG_WARNING("Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");      
    }
    
    // do all jet stuff here - easier to loop over jets first
    // some duplication of code below - "cleanest" way of adding this information
    // and only calling this tool 1x
    // G. Facini

    std::string type = "old"; //designates whether this is old-style or new-style

    if(type == "new"){
      if(m_fillTIDEPlots && !m_jetContainerName.empty()){
	ATH_MSG_DEBUG("Getting jet Container");
	const xAOD::JetContainer* jets = getContainer<xAOD::JetContainer>(m_jetContainerName);
	ATH_MSG_DEBUG("Getting Truth Container");
        //truthParticles already retrieved above 
        //const xAOD::TruthParticleContainer* truthParticles = (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);

	if( !jets){
	  ATH_MSG_WARNING("Cannot open "<< m_jetContainerName <<" jet container. Skipping jet plots.");
        }
        else {
          // if (!truthParticles) ATH_MSG_INFO("Cannot open "<<m_truthContainerName<<" truth container.");
	  ATH_MSG_DEBUG("All systems normal");
	  for( const auto & thisJet: *jets){          //The big jets loop
	    if(not passJetCuts(*thisJet)) continue;
	    for (const auto thisTrack: *ptracks){     //The beginning of the track loop
	      if (m_useTrackSelection){
		//0 means z0, d0 cut is wrt beam spot - put a primary vertex in to change this
		if( !(m_trackSelectionTool->accept(*thisTrack, 0)) ) continue;
	      }
	      if(m_onlyInsideOutTracks and !(isInsideOut(*thisTrack))) continue; //not an inside out track
	      if(thisJet->p4().DeltaR( thisTrack->p4() ) > m_maxTrkJetDR ) continue;

	      if(m_monPlots->filltrkInJetPlot(*thisTrack, *thisJet)){                            //Attempt to duplicate "pass" filter

		m_monPlots->fillSimpleJetPlots(*thisTrack);                                      //Fill all the simple jet plots
	      
		const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack);           //Get the associated truth for this track
                if (associatedTruth) {
		   m_monPlots->fillJetResPlots(*thisTrack, *associatedTruth, *thisJet);             //Fill jet pull & resolution plots
		
		   if( m_truthSelectionTool->accept(associatedTruth) ){                             //Fill the Jet plots with "Eff" in the name
		     m_monPlots->fillJetEffPlots(*associatedTruth, *thisJet);
		   }
                }
	      }
	    } //end of track loop
	    //fill in things like sum jet pT in dR bins - need all tracks in the jet first
	    m_monPlots->fillJetPlotCounter(*thisJet);
	    for (const auto & thisTruth: *truthParticles){
	      //for primary tracks we want an efficiency as a function of track jet dR
	      if( !(m_truthSelectionTool->accept(thisTruth)) ) continue;
	      if( thisJet->p4().DeltaR( thisTruth->p4() ) > m_maxTrkJetDR ) continue;
	      m_monPlots->fillJetTrkTruth(*thisTruth, *thisJet);
	    }
	    m_monPlots->fillJetTrkTruthCounter(*thisJet);
	  } //loop over jets
	}   // if have collections needed
      }     // if TIDE
    }else if(type == "old"){
      if(m_fillTIDEPlots  && !m_jetContainerName.empty()) { 
	ATH_MSG_DEBUG("Getting Jet Container");
	const xAOD::JetContainer* jets = getContainer<xAOD::JetContainer>(m_jetContainerName);
	ATH_MSG_DEBUG("Getting Truth Container");
        // truth particles already retrieved above 
	//std::string m_truthContainerName = "TruthParticles";
	//const xAOD::TruthParticleContainer* truthParticles = getContainer<xAOD::TruthParticleContainer>(m_truthContainerName);
	if( !jets) {
	  ATH_MSG_WARNING("Cannot open " << m_jetContainerName << " jet container. Skipping jet plots.");
	} else {
	  // if (!truthParticles) ATH_MSG_INFO("Cannot open " << m_truthContainerName << " jet container. Skipping jet plots.");
	  for (const auto & thisJet: *jets){
	    if (not passJetCuts(*thisJet))  continue; 
	    for (const auto thisTrack: *ptracks){
	      // would be easier if decorated ...
	      if (m_useTrackSelection ) {
		// 0 means z0, d0 cut is wrt beam spot - put a PV in to change this
		if( ! (m_trackSelectionTool->accept(*thisTrack, 0)) ) continue; 
	      }
	      if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue; // not an inside-out track
	      //
	      if( thisJet->p4().DeltaR( thisTrack->p4() ) > m_maxTrkJetDR ) { continue; }
	      m_monPlots->fillJetPlot(*thisTrack,*thisJet);
	    } // loop over tracks
	    // fill in things like sum jet pT in dR bins - need all tracks in the jet first
	    m_monPlots->fillJetPlotCounter(*thisJet);
	    for (const auto & thisTruth: *truthParticles){
	      // for primary tracks want an efficiency as a function of track jet dR
	      if (not truthSelector(*thisTruth)) continue;
	      if( thisJet->p4().DeltaR( thisTruth->p4() ) > m_maxTrkJetDR ) { continue; }
	      m_monPlots->fillJetTrkTruth(*thisTruth,*thisJet);
	    } // loop over truth
	    m_monPlots->fillJetTrkTruthCounter(*thisJet);
	  } // loop over jets
	} // if have collections needed
      } // if TIDE
    }
    return StatusCode::SUCCESS;
}


StatusCode
InDetPhysValMonitoringTool::bookHistograms(){
  ATH_MSG_INFO ("Booking hists " << name() << "...");
  m_monPlots->setDetailLevel(100); //DEBUG, enable expert histograms
  m_monPlots->initialize();
  std::vector<HistData> hists = m_monPlots->retrieveBookedHistograms();
  for (auto hist : hists){
    ATH_CHECK(regHist(hist.first,hist.second,all)); //??
  }
  return StatusCode::SUCCESS;
}


StatusCode 
InDetPhysValMonitoringTool::procHistograms() {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    if (endOfRun){
      m_monPlots->finalize();
    }
    ATH_MSG_INFO ("Successfully finalized hists");
    return StatusCode::SUCCESS;
  }
  
