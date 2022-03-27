/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////////
/// Author: Peter V. Loscutoff (ploscutoff@gmail.com
/// February 2009
/////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

//#include "StoreGate/DataHandle.h"

/// head file
#include "InDetEventSplitter.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

InDet::InDetEventSplitter::InDetEventSplitter(const std::string& name,
  ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_isOdd(false),
  m_addToVx(0),
  m_eventN(0){

  /// switches to control the analysis through job options
  declareProperty("TPBContainerName", m_tpbContainerName = "TrackParticleCandidate");
  declareProperty("TrackContainerName", m_trackContainerName = "Tracks");
  declareProperty("MaxVertexNumber", m_maxVtx = 1); //this should not be changed until a more robust handling of multi vertices is implemented
  declareProperty("PrimaryOnly",m_priOnly = true);  //this should not be changed presently
  declareProperty("UseTrackParticleBase",m_savetpb = true); //is this needed?
  declareProperty("RandomSeed",m_rndSeed = 7);
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

InDet::InDetEventSplitter::~InDetEventSplitter() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate

StatusCode InDet::InDetEventSplitter::initialize() {
  
  std::srand(m_rndSeed);
  m_isOdd = false;
  m_addToVx = 1;
  m_eventN = 0;

  for (int i = 1; i <=m_maxVtx; i++){
    std::stringstream ss;
    ss << "odd_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str());
    ss.str("");
    ss << "even_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str()); 
    ss.str("");
    ss << "all_" << i << "_Tracks";
    m_trackKeys.push_back(ss.str());
    ss.str("");
  }

  ATH_MSG_INFO ("Initializing InDetEventSplitter");
  
  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode InDet::InDetEventSplitter::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode InDet::InDetEventSplitter::execute() {

  ATH_MSG_DEBUG("in execute()");

  StatusCode sc = StatusCode::SUCCESS;

  sc = split_vertices();
  if (sc.isFailure()) {
     ATH_MSG_ERROR("InDetEventSplitter Failed");
     return sc;
  }

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode InDet::InDetEventSplitter::split_vertices() {

  ATH_MSG_DEBUG("in split_vertices()");

  StatusCode sc = StatusCode::SUCCESS;

  const Rec::TrackParticleContainer* tpbTES{};
  const TrackCollection* trkTES{};

  if (m_savetpb){
    sc=evtStore()->retrieve( tpbTES, m_tpbContainerName);
    if( sc.isFailure()  ||  !tpbTES ) {
      ATH_MSG_WARNING("No TrackParticleBase container found in TDS tried " << m_tpbContainerName);
      return StatusCode::SUCCESS;
    }  
    ATH_MSG_DEBUG("TrackParticleCandidate Collection successfully retrieved");
  }
  else {
    sc=evtStore()->retrieve( trkTES, m_trackContainerName);
    if( sc.isFailure()  ||  !trkTES ) {
      ATH_MSG_WARNING("No TrackCollection container found in TDS tried " << m_trackContainerName);
      return StatusCode::SUCCESS;
    }  
    ATH_MSG_DEBUG("TrackParticleCandidate Collection successfully retrieved");
  }  

  std::map<std::string,TrackCollection*> trackmap;
  std::map<std::string,Trk::TrackParticleBaseCollection*> tpbmap;

//  We need to create every container for each event, even if we don't write to them

  for (const auto & key : m_trackKeys){
    TrackCollection* tempTracks{};
    trackmap[key] = tempTracks;
    if (evtStore()->contains<TrackCollection>(key) &&
	(evtStore()->retrieve(trackmap[key],key)).isSuccess()){
    } else {
      trackmap[key] = new TrackCollection;
    }
  }  

   for (const auto & key : m_trackKeys){
    Trk::TrackParticleBaseCollection* tempTpbs{};
    tpbmap[key] = tempTpbs;
    if (evtStore()->contains<Trk::TrackParticleBaseCollection>(key) &&
	(evtStore()->retrieve(tpbmap[key],key)).isSuccess()){
    } else {
      tpbmap[key] = new Trk::TrackParticleBaseCollection;
    }
  }  

  //We need to add an approprate fraction of unfit tracks to the half and full vertex collections
  //lets pull in the full list of tracks  

  if (m_savetpb and tpbTES){
    //we loop over that list
    std::string oeNameString;
    std::stringstream sss;
    oeNameString.reserve(20);
    for (const auto * tpb: *tpbTES){
      //it looks like our track collection is actually sorted by the vertex that they're in
      //which means that just alternating odd vs even is equivalent to splitting the vertex first, then splitting the remining
      //instead, we will just put in rand() call
      m_isOdd = std::rand() % 2;
      oeNameString.clear();
      if (m_isOdd)  oeNameString = "odd";
      if (!m_isOdd) oeNameString = "even";
      sss.str("");
      sss << oeNameString << "_" << m_addToVx << "_Tracks";
      std::string oecontainerName = sss.str();   		  	
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << m_addToVx << "_Tracks";
      std::string allcontainerName = sss.str();  
      Trk::TrackParticleBase *trkCopy1 = new Trk::TrackParticleBase(*tpb);
      Trk::TrackParticleBase *trkCopy2 = new Trk::TrackParticleBase(*tpb);
      ATH_MSG_DEBUG("found a trackparticlebase, with momentum "<<tpb->definingParameters().momentum()<<" giving it the key: "<< oecontainerName);	
      tpbmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found a trackparticlebase, with momentum "<<tpb->definingParameters().momentum()<<" giving it the key: "<< allcontainerName);	
      tpbmap[allcontainerName]->push_back(trkCopy2);	
      
      m_addToVx++;
      if (m_addToVx > m_maxVtx) m_addToVx = 1;
    }
  }
  
  if (!m_savetpb){
    std::cout<<"NotYet Implemented"<<std::endl;
  }

  if (m_savetpb){
        for (const auto & key : m_trackKeys){
      if(evtStore()->record(tpbmap[key],key,false).isFailure() ){
	      ATH_MSG_ERROR("Could not save the "<< key);
      }
    }
  } else {
    for (const auto & key : m_trackKeys){
      if(evtStore()->record(trackmap[key],key,false).isFailure() ){
	      ATH_MSG_ERROR("Could not save the "<< key);
      }
    }
  } 
  ATH_MSG_DEBUG("split_vertices() succeeded");
  m_eventN++;
  return StatusCode::SUCCESS;
}

