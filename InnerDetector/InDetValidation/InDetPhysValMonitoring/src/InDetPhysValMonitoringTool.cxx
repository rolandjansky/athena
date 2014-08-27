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
#include "InDetPhysValTruthDecoratorTool.h"
#include "ParameterErrDecoratorTool.h"
#include "InDetPhysHitDecoratorTool.h"
#include "TruthClassDecoratorTool.h"
#include "InDetRttPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "TrkTrack/TrackCollection.h"
#include <limits>



namespace { //utility functions used here
  //get truth particle associated with a track particle
  const xAOD::TruthParticle * getTruthPtr(const xAOD::TrackParticle & trackParticle){
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    const xAOD::TruthParticle * nullPtr(0);
    const xAOD::TruthParticle * result(nullPtr);
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
  
  //is a number a NaN?
  template <class T>
  bool
  is_nan(const T & aNumber){
  	return (aNumber != aNumber);
  }
  
  
}//namespace

///Parametrized constructor
InDetPhysValMonitoringTool::InDetPhysValMonitoringTool(const std::string & type, const std::string & name, const IInterface* parent):
    ManagedMonitorToolBase(type, name, parent), m_monPlots(0),
    m_trackSelectorTool("InDet::InDetDetailedTrackSelectorTool"),
    m_useTrackSelection(false),
    m_truthDecoratorTool("InDetPhysValTruthDecoratorTool"),
    m_hitDecoratorTool("InDetPhysHitDecoratorTool"),
    m_errDecoratorTool("ParameterErrDecoratorTool"),
    m_truthClassDecoratorTool("TruthClassDecoratorTool"){
      declareProperty("TrackParticleContainerName", m_trkParticleName="InDetTrackParticles"); 
      declareProperty("TruthParticleContainerName", m_truthParticleName="TruthParticle"); 
      declareProperty("VertexContainerName", m_vertexContainerName="PrimaryVertices");
      declareProperty("EventInfoContainerName", m_eventInfoContainerName="EventInfo");
      declareProperty("useTrackSelection"       , m_useTrackSelection);
      declareProperty("TrackSelectorTool"       , m_trackSelectorTool);      
    m_monPlots = new InDetRttPlots(0,"IDPerformanceMon/");
}

InDetPhysValMonitoringTool::~InDetPhysValMonitoringTool(){
  delete m_monPlots;m_monPlots=0;
}

StatusCode 
InDetPhysValMonitoringTool::initialize(){
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ATH_CHECK(ManagedMonitorToolBase::initialize());
    //Get the track selector tool only if m_useTrackSelection is true
    if (m_useTrackSelection and  m_trackSelectorTool.retrieve().isFailure() ) {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSelectorTool << endreq;
        return StatusCode::FAILURE;
    }
    
    if (m_truthDecoratorTool.retrieve().isFailure()){
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthDecoratorTool << endreq;
      return StatusCode::FAILURE;
    }
    
    if (m_hitDecoratorTool.retrieve().isFailure()){
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_hitDecoratorTool << endreq;
      return StatusCode::FAILURE;
    }
    
     if (m_errDecoratorTool.retrieve().isFailure()){
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_errDecoratorTool << endreq;
      return StatusCode::FAILURE;
    }
    if (m_truthClassDecoratorTool.retrieve().isFailure()){
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthClassDecoratorTool << endreq;
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode 
InDetPhysValMonitoringTool::fillHistograms(){
    ATH_MSG_DEBUG ("Filling hists " << name() << "...");
    //retrieve trackParticle container
    auto ptracks = getContainer<xAOD::TrackParticleContainer>(m_trkParticleName);
    if ((!ptracks) ) return StatusCode::FAILURE;
    //retrieve truthParticle container
    auto ptruth = getContainer<xAOD::TruthParticleContainer>(m_truthParticleName);
    if ((!ptruth) ) return StatusCode::FAILURE;
    
    //
    //Loop over reconstructed tracks
    const unsigned int nTracks(ptracks->size());
    const unsigned int nTruth(ptruth->size());
    unsigned int nSelectedTracks(0), num_truthmatch_match	(0);
    const float minProbEffLow(0.5); //if the probability of match is less than this, we call it a fake
    for (const auto thisTrack: *ptracks){
      //Select only good tracks, if selection is turned on
      if (m_useTrackSelection and not (m_trackSelectorTool->decision(*thisTrack)) ) continue;
      ++nSelectedTracks; //increment number of selected tracks
      const xAOD::TruthParticle * associatedTruth = getTruthPtr(*thisTrack); //get the associated truth      
      //decorate the track here, if necessary <insert code>
      //bool successfulTrackDecoration = m_hitDecoratorTool->decorateTrack(*thisTrack);
      //if (not successfulTrackDecoration) ATH_MSG_DEBUG ("Could not get hit info for the track particle.");
      bool successfulTrackDecoration = m_errDecoratorTool->decorateTrack(*thisTrack);
      if (not successfulTrackDecoration) ATH_MSG_WARNING ("Could not get err info for the track particle.");
      //
      m_monPlots->fill(*thisTrack);//Make all the plots requiring only trackParticle
      if (associatedTruth){
        ++num_truthmatch_match;
        float prob=getMatchingProbability(*thisTrack);
        if (not is_nan(prob)) {
        	const bool isFake=(prob<minProbEffLow);
        	m_monPlots->fillFakeRate(*thisTrack, isFake);
        }
       /**
        * Decorate the particles with information 
        * before plotting those variables below
        **/
        bool successfulDecoration = m_truthDecoratorTool->decorateTruth(*associatedTruth);
        if (not successfulDecoration) ATH_MSG_WARNING ("Could not retrieve some information for the truth particle.");
        successfulDecoration = m_truthClassDecoratorTool->decorateTruth(*associatedTruth);
        if (not successfulDecoration) ATH_MSG_WARNING ("The truth particle could not be assigned a type");
        //should explicitly decide whether to continue or not in the case of unsuccessful addition of information...
        m_monPlots->fill(*thisTrack, *associatedTruth); //Make all the plots requiring both truth and track
        // following line causes crash because there is no truthType or truthOrigin decoration
        m_monPlots->fill(*associatedTruth);//Make all the plots requiring  truth only (if any) 
      } 
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
      m_monPlots->fill(*pvertex, *pei);
    } else {
      ATH_MSG_WARNING("Cannot open " << m_eventInfoContainerName << " EventInfo container. Skipping vertexing plots using EventInfo.");      
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
    return StatusCode::SUCCESS;
  }
  
