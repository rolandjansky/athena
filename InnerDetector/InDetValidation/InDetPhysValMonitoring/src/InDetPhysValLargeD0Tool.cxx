/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValLargeD0Tool.cxx
 * @author siin che
**/

#include "InDetPhysValMonitoring/InDetPhysValLargeD0Tool.h"

#include <vector>
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetRttLargeD0Plots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include <limits>
#include <cmath> //to get std::isnan()

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetPhysValMonitoring/IInDetPhysValDecoratorTool.h"

#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"


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
/**
unused here.
  bool truthSelector(const xAOD::TruthParticle &truth){
    const bool pCut = ( truth.pt() >= 500 ); //ORIGINAL SETTING: ( truth.pt() >= 400 );
    const bool stabilityCut = (truth.status() == 1);
    const bool noNeutrals = not (truth.isNeutral());
    return (pCut and stabilityCut and noNeutrals);
  }
**/

}//namespace

///Parametrized constructor
InDetPhysValLargeD0Tool::InDetPhysValLargeD0Tool(const std::string & type, const std::string & name, const IInterface* parent):
    ManagedMonitorToolBase(type, name, parent),
    m_useTrackSelection(false),
    m_onlyInsideOutTracks(false),
    m_truthSelectionTool("TrackTruthSelectionTool/TruthSelectionTool")
{
  declareInterface<IAsgSelectionTool>(this);

  declareProperty("TrackParticleContainerName", m_trkParticleName="InDetTrackParticles");
  declareProperty("TruthParticleContainerName", m_truthParticleName="TruthParticles");
  declareProperty("VertexContainerName", m_vertexContainerName="PrimaryVertices");
  declareProperty("EventInfoContainerName", m_eventInfoContainerName="EventInfo");
  declareProperty("useTrackSelection"       , m_useTrackSelection); //redundant?
  declareProperty("onlyInsideOutTracks"     , m_onlyInsideOutTracks);
  declareProperty("TrackSelectionTool"      , m_trackSelectionTool);
  declareProperty("TruthSelectionTool"      , m_truthSelectionTool);
  declareProperty("SubFolder",m_folder);
  declareProperty("LongLivedParticle",m_LLP="");
}

InDetPhysValLargeD0Tool::~InDetPhysValLargeD0Tool(){
}

StatusCode
InDetPhysValLargeD0Tool::initialize(){
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    //Get the track selector tool only if m_useTrackSelection is true;
    //first check for consistency i.e. that there is a trackSelectionTool if you ask
    //for trackSelection
    if (m_useTrackSelection) {
      if (not m_trackSelectionTool){
        ATH_MSG_ERROR("\033[1;31mYou have chosen to use track selection, but no track selection tool was configured\033[0m\n");
        return StatusCode::FAILURE;
      }
      ATH_CHECK(m_trackSelectionTool.retrieve());
    }
    ATH_CHECK(m_truthSelectionTool.retrieve());
    m_LargeD0Plots = std::move( std::unique_ptr<InDetRttLargeD0Plots> (new InDetRttLargeD0Plots(0,"IDPerformanceMon/"+m_folder)) );

    return StatusCode::SUCCESS;
}

StatusCode
InDetPhysValLargeD0Tool::fillHistograms(){
    ATH_MSG_DEBUG ("Filling hists " << name() << "...");
    //retrieve trackParticle container
    auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    if ((!ptracks) ) return StatusCode::FAILURE;
    //retrieve truthParticle container
    const xAOD::TruthParticleContainer* truthParticles = (!m_truthParticleName.empty() ? getContainer<xAOD::TruthParticleContainer>(m_truthParticleName) : nullptr);
    const unsigned int nTracks(ptracks->size());
    const unsigned int nTruth(truthParticles  ? truthParticles->size() : 0);
    unsigned int nSelectedTracks(0), num_truthmatch_match(0);
    // the truth matching probability must not be <= 0., otherwise the tool will seg fault in case of missing truth (e.g. data):
    const float minProbEffLow(0.50); //if the probability of a match is less than this, we call it a fake
    const float minProbEffHigh(0.80); //if the probability of a match is higher than this, it either feeds the NUM or is a duplicate
    //

    //Main track loop, filling Track-only, Track 'n' Truth with good matching probability (meas, res, & pull), and Fakes
    for(const auto & thisTrack: *ptracks){
      if(m_useTrackSelection){   //0 means z0, d0 cut is wrt beam spot - put in a PV to change this
        if( !(m_trackSelectionTool->accept(*thisTrack, 0))) continue;
      }
      if(m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue; //not an inside-out track
      ++nSelectedTracks;                                                    //increment number of selected tracks
      m_LargeD0Plots->fill(*thisTrack);                                         //Make all the plots requiring only trackParticle
      const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack);
      float prob = getMatchingProbability(*thisTrack);
      //This is where the BMR, Fake, and Really Fake fillers need to go.
      //m_LargeD0Plots->fillBMR_Denom(*thisTrack);
      if(associatedTruth){
        m_LargeD0Plots->fillBMR_Denom(*thisTrack);
        if(prob < minProbEffHigh){
          m_LargeD0Plots->fillBMR_Num(*thisTrack);
          if((prob < minProbEffLow) and (not std::isnan(prob))){
            const bool isFake = (prob<minProbEffLow);
            m_LargeD0Plots->fillFakeRate(*thisTrack, isFake);
            m_LargeD0Plots->fillFake(*thisTrack);
            if(prob < 0.2){
              m_LargeD0Plots->fillRF_Num(*thisTrack);
            }
          }
        }
        if((prob > minProbEffLow)){
	  bool isFake = false;
	  m_LargeD0Plots->fillFakeRate(*thisTrack, isFake);
          m_LargeD0Plots->fill(*thisTrack, *associatedTruth); // filling non-fake track
        }
      } else {
	bool isFake = true;
	m_LargeD0Plots->fillFakeRate(*thisTrack, isFake);
	m_LargeD0Plots->fillFake(*thisTrack); // filling fake tracks
      }
    }


    //This is the beginning of the Nested Loop, built mainly for the Efficiency Plots
    if ( truthParticles ) {
      for ( const auto & thisTruth: *truthParticles ){  //Outer loop over all truth particles
        if ( m_truthSelectionTool -> accept(thisTruth) ){	 
	  bool isReconstruted = false;
	  bool isStandardTrack = false;
	  float bestMatch = 0;
          m_LargeD0Plots->fillTruth(*thisTruth);  //DENOMINATOR
          //std::vector <pair<float, const xAOD::TrackParticle*> > prospects; //Vector of pairs: <truth_matching_probability, track> if prob > minProbEffLow (0.5)
	  
	  if(m_LLP=="Gluino"){	    
	    if( isLLP( thisTruth -> absPdgId()) &&  hasNoLLP( thisTruth ) ) {    
	      for(size_t ii = 0; ii < thisTruth->decayVtx()->nOutgoingParticles(); ii++){
		const xAOD::TruthParticle* p = thisTruth->decayVtx()->outgoingParticle(ii);
		if ( (p != NULL) && (p->status() == 1) && (p->hasProdVtx()) && (!p->hasDecayVtx()) ) {
		  isReconstruted = false;
		  isStandardTrack = false;
		  bestMatch = 0;
		  for (const auto & thisTrack: *ptracks){ //Inner loop over all track particle	    
		    if (m_useTrackSelection){
		      //0 means z0, d0 cut is wrt beam spot - put a PV in to change this
		      if( !(m_trackSelectionTool->accept(*thisTrack, 0)) ) continue;
		    }
		    if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue;  //not an inside-out track
		    const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack); //get the associated truth for this track
		    if(associatedTruth && associatedTruth == p){
		      float prob = getMatchingProbability(*thisTrack);
		      bestMatch=std::max(prob, bestMatch);
		      if (prob > minProbEffLow){
			isReconstruted=true;		
			const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = thisTrack->patternRecoInfo();
			if (patternReco.test(49)) isStandardTrack = false;
			else                      isStandardTrack = true;
		      }
		    }
		  }   
		  m_LargeD0Plots->fillEfficiency(*p,isReconstruted,isStandardTrack);
		}
	      }
	    }
	  }else{
	    for (const auto & thisTrack: *ptracks){ //Inner loop over all track particle	    
	      if (m_useTrackSelection){
		//0 means z0, d0 cut is wrt beam spot - put a PV in to change this
		if( !(m_trackSelectionTool->accept(*thisTrack, 0)) ) continue;
	      }
	      if (m_onlyInsideOutTracks and (not isInsideOut(*thisTrack))) continue;  //not an inside-out track
	      const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack); //get the associated truth for this track
	      if(associatedTruth && associatedTruth == thisTruth){
		float prob = getMatchingProbability(*thisTrack);
		bestMatch=std::max(prob, bestMatch);
		if (prob > minProbEffLow){
		  //		  prospects.push_back(make_pair(prob, thisTrack));
		  isReconstruted=true;		
		  const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = thisTrack->patternRecoInfo();
		  if (patternReco.test(49)) isStandardTrack = false;
		  else                      isStandardTrack = true;
		}
	      }
	    }
	    if( isCameFromLLP(thisTruth) || m_LLP == "" ){
	      m_LargeD0Plots->fillEfficiency(*thisTruth,isReconstruted,isStandardTrack);
	    }
	  }
	  /*   
	       int deg_count = prospects.size();
	       if(bestMatch >= minProbEffHigh){
	       ++num_truthmatch_match;
	       const xAOD::TruthParticle * assoc_Truth = getTruthPtr(*prospects.at(0).second);
	       if (!assoc_Truth) continue;
	       m_LargeD0Plots->fill(*assoc_Truth); //This is filling truth-only plots, NUMERATOR & m_TrackTruthInfoPlots
	       for(int i=0; i<deg_count; i++){
	       if((prospects.at(i).first < bestMatch) and (prospects.at(i).first > minProbEffHigh)){
	       //m_LargeD0Plots->fillDupTrack(prospects.at(i).second); //fill the duplicates plots w/ the tracks
	       }
	       }
	       }
	  */
	}
      }//End of outer truthParticle loop
    }    //This is the end of the Nested Loop approach section
    //
    if (num_truthmatch_match == 0){
      ATH_MSG_DEBUG("NO TRACKS had associated truth.");
    } else {
      ATH_MSG_DEBUG(num_truthmatch_match <<" tracks out of "<<nTracks<<" had associated truth.");
    }
    m_LargeD0Plots->fillCounter(nSelectedTracks, InDetPerfPlot_nTracks::SELECTED);
    m_LargeD0Plots->fillCounter(nTracks, InDetPerfPlot_nTracks::ALL);
    m_LargeD0Plots->fillCounter(nTruth, InDetPerfPlot_nTracks::TRUTH);
    m_LargeD0Plots->fillCounter(num_truthmatch_match, InDetPerfPlot_nTracks::TRUTH_MATCHED);


    ATH_MSG_DEBUG("Filling vertex plots");
    const xAOD::VertexContainer* pvertex = getContainer<xAOD::VertexContainer>(m_vertexContainerName);
    if (pvertex) {
      m_LargeD0Plots->fill(*pvertex);
    } else {
      ATH_MSG_WARNING("Cannot open " << m_vertexContainerName << " vertex container. Skipping vertexing plots.");
    }
    ATH_MSG_DEBUG("Filling vertex/event info monitoring plots");
    const xAOD::EventInfo* pei = getContainer<xAOD::EventInfo>(m_eventInfoContainerName);
    if (pei) {
      if (pvertex) {
         m_LargeD0Plots->fill(*pvertex, *pei);
      }
    } else {
      ATH_MSG_WARNING("Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");
    }
    return StatusCode::SUCCESS;
}


StatusCode
InDetPhysValLargeD0Tool::bookHistograms(){
  ATH_MSG_INFO ("Booking hists " << name() << "...");
  m_LargeD0Plots->setDetailLevel(100); //DEBUG, enable expert histograms
  m_LargeD0Plots->initialize();
  std::vector<HistData> hists = m_LargeD0Plots->retrieveBookedHistograms();
  for (auto hist : hists){
    ATH_CHECK(regHist(hist.first,hist.second,all)); //??
  }
  return StatusCode::SUCCESS;
}


StatusCode
InDetPhysValLargeD0Tool::procHistograms() {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    if (endOfRun){
      m_LargeD0Plots->finalize();
    }
    ATH_MSG_INFO ("Successfully finalized hists");
    return StatusCode::SUCCESS;
  }

bool InDetPhysValLargeD0Tool::isCameFromLLP( const xAOD::TruthParticle* p) {

  std::vector<int> LLP_pdgId;

  if     (m_LLP=="Zprime"){ LLP_pdgId = {32};      }
  else if(m_LLP=="Wino"  ){ LLP_pdgId = {1000023}; }
  else if(m_LLP=="Gluino"){ LLP_pdgId = {1090000,1009000,1000900}; };
  
  const xAOD::TruthParticle *parent = p;
  
  if ( (p != NULL) && (p->status() == 1) && (p->hasProdVtx()) && (!p->hasDecayVtx()) && (p->nParents() > 0)) {  
    while(parent != NULL){
      if(std::find( LLP_pdgId.begin(), LLP_pdgId.end(), parent->pdgId() ) != LLP_pdgId.end()) {
	return true;
      }else{
	parent = parent->parent();
      }
    }
  }
  return false;
}

bool InDetPhysValLargeD0Tool::hasNoLLP( const xAOD::TruthParticle* p ) {
  if( p->hasDecayVtx() ){
    for(size_t ii = 0; ii < p->decayVtx()->nOutgoingParticles(); ii++){
      if(isLLP(p->decayVtx()->outgoingParticle(ii)->pdgId())){
	return false;
      }
    }
  }
  return true;
}


bool InDetPhysValLargeD0Tool::isLLP( const int absPdgId) {


  if     (m_LLP=="Zprime"){ return absPdgId ==      32; }
  else if(m_LLP=="Wino"  ){ return absPdgId == 1000023; }
  else if(m_LLP=="Gluino"){      
    if( (int) ( absPdgId / 10000 ) ==   109 ||
	(int) ( absPdgId /  1000 ) ==  1009 ||
	(int) ( absPdgId /   100 ) == 10009 ) {
      return true;
    }
  }
  
  return false; 
}
