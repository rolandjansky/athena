/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////////
/// Author: Peter V. Loscutoff (ploscutoff@gmail.com
/// February 2009
/////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/DataHandle.h"

/// head file
#include "InDetVertexSplitter.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

#include <algorithm>
#include <math.h>
#include <map>
#include <sstream>

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

InDet::InDetVertexSplitter::InDetVertexSplitter(const std::string& name,
  ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  isMatchedOdd(false),
  isUnmatchOdd(false),
  addToVxMatched(0),
  addToVxUnmatch(0),
  eventN(0){

  /// switches to control the analysis through job options
  declareProperty("TPBContainerName", m_tpbContainerName = "TrackParticleCandidate");
  declareProperty("TrackContainerName", m_trackContainerName = "Tracks");
  declareProperty("VertexContainerName", m_vertexContainerName = "VxPrimaryCandidate");
  declareProperty("MaxVertexNumber", m_maxVtx = 1); //this should not be changed until a more robust handling of multi vertices is implemented
  declareProperty("PrimaryOnly",m_priOnly = true);  //this should not be changed presently
  declareProperty("UseTrackParticleBase",m_savetpb = true); //is this needed?
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

InDet::InDetVertexSplitter::~InDetVertexSplitter() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate

StatusCode InDet::InDetVertexSplitter::initialize() {

  isMatchedOdd = false;
  isUnmatchOdd = false;
  addToVxMatched = 1;
  addToVxUnmatch = 1;
  eventN = 0;

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

  ATH_MSG_INFO ("Initializing InDetVertexSplitter");
  
  return StatusCode::SUCCESS;
}    

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode InDet::InDetVertexSplitter::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  
  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode InDet::InDetVertexSplitter::execute() {

  ATH_MSG_DEBUG("in execute()");

  StatusCode sc = StatusCode::SUCCESS;

  sc = split_vertices();
  if (sc.isFailure()) {
     ATH_MSG_ERROR("InDetVertexSplitter Failed");
     return sc;
  }

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////

StatusCode InDet::InDetVertexSplitter::split_vertices() {

  ATH_MSG_DEBUG("in split_vertices()");

  StatusCode sc = StatusCode::SUCCESS;

  const VxContainer* vtxTES=0;
  sc=evtStore()->retrieve( vtxTES, m_vertexContainerName);
  if( sc.isFailure()  ||  !vtxTES ) {
    ATH_MSG_WARNING("No VxContainer container found in TDS tried " << m_vertexContainerName);
    return StatusCode::SUCCESS;
  }  
 
  ATH_MSG_DEBUG("Vertex successfully retrieved");

  const Rec::TrackParticleContainer* tpbTES=0;
  const TrackCollection* trkTES=0;

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

  std::vector<std::string>::const_iterator key_iter = m_trackKeys.begin();
  const std::vector<std::string>::const_iterator keyEnd(m_trackKeys.end());
  for (; key_iter != keyEnd; ++key_iter){
    const std::string & thisKey=*key_iter;
    TrackCollection* tempTracks = 0;
    trackmap[thisKey] = tempTracks;
    if (evtStore()->contains<TrackCollection>(thisKey) && (evtStore()->retrieve(trackmap[thisKey],thisKey)).isSuccess()){
      //nop
    } else {
      trackmap[thisKey] = new TrackCollection;
    }
  }  

  for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end(); ++key_iter){
    Trk::TrackParticleBaseCollection* tempTpbs = 0;
    tpbmap[(*key_iter)] = tempTpbs;
    if (evtStore()->contains<Trk::TrackParticleBaseCollection>((*key_iter)) &&
  (evtStore()->retrieve(tpbmap[(*key_iter)],(*key_iter))).isSuccess()){
    } else {
      tpbmap[(*key_iter)] = new Trk::TrackParticleBaseCollection;
    }
  }  

  if (m_savetpb){
    Rec::TrackParticleContainer::const_iterator tpbItr = tpbTES->begin();
    Rec::TrackParticleContainer::const_iterator tpbItrE = tpbTES->end();
    //we loop over that list
    for (; tpbItr != tpbItrE; tpbItr++){
      const Trk::TrackParameters* trkPerigee = &((*tpbItr)->definingParameters());
      bool trackmatched = false;
      //we compare it to the tracks already associated with vertices
      
      VxContainer::const_iterator vtxItr  = vtxTES->begin();
      VxContainer::const_iterator vtxItrE = vtxTES->end();  
      
      ATH_MSG_DEBUG("Found "<<vtxTES->size()<<" vertices");  
      
      int i_vtx = 0;
      //  We're relying here on the implicit sorting of vertices by sqrt(N_tracks)*Sum Pt_track^2
      //  This should pick out the most interesting N vertices
      //  Hopefully we have only 1 primary vertex, but if there is > 1 we can grab all of those too
      for (; vtxItr != vtxItrE; vtxItr++){
  if ( (!m_priOnly || (*vtxItr)->vertexType() == 1) && (i_vtx < m_maxVtx) ){
    i_vtx++;
    std::vector<Trk::VxTrackAtVertex*> vertexTracks = (*(*vtxItr)->vxTrackAtVertex());
    ATH_MSG_DEBUG("parent vertex has "<<vertexTracks.size()<<" tracks, at position: "<<(*vtxItr)->recVertex().position().x());
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tavI = vertexTracks.begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tavIe= vertexTracks.end();
    for (; tavI != tavIe; tavI++){
      const Trk::TrackParameters* vxTrkPerigee = (*tavI)->initialPerigee();
      if (trkPerigee == vxTrkPerigee) {trackmatched = true;}
    }
    Trk::TrackParticleBase *trkCopy1 = new Trk::TrackParticleBase((*(*tpbItr)));
    Trk::TrackParticleBase *trkCopy2 = new Trk::TrackParticleBase((*(*tpbItr)));
    if (trackmatched == false){
      std::string oeNameString;
      if (isUnmatchOdd)  oeNameString = "odd";
      if (!isUnmatchOdd) oeNameString = "even";
      std::stringstream sss;
      sss << oeNameString << "_" << addToVxUnmatch << "_Tracks";
      std::string oecontainerName = sss.str();          
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << addToVxUnmatch << "_Tracks";
      std::string allcontainerName = sss.str();  
      ATH_MSG_DEBUG("found an unmatched trackparticlebase, giving it the key: "<< oecontainerName); 
      tpbmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found an unmatched trackparticlebase, giving it the key: "<< allcontainerName);  
      tpbmap[allcontainerName]->push_back(trkCopy2);  
      isUnmatchOdd = !isUnmatchOdd;
      addToVxUnmatch++;
      if (addToVxUnmatch > m_maxVtx) addToVxUnmatch = 1;
    }
    if (trackmatched == true){
      std::string oeNameString;
      if (isMatchedOdd)  oeNameString = "odd";
      if (!isMatchedOdd) oeNameString = "even";
      std::stringstream sss;
      sss << oeNameString << "_" << addToVxMatched << "_Tracks";
      std::string oecontainerName = sss.str();          
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << addToVxMatched << "_Tracks";
      std::string allcontainerName = sss.str();  
      ATH_MSG_DEBUG("found an matched trackparticlebase, giving it the key: "<< oecontainerName); 
      tpbmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found an matched trackparticlebase, giving it the key: "<< allcontainerName);  
      tpbmap[allcontainerName]->push_back(trkCopy2);  
      isMatchedOdd = !isMatchedOdd;
      addToVxMatched++;
      if (addToVxMatched > m_maxVtx) addToVxMatched = 1;
    }
  }
      }
    }
  }
  if (!m_savetpb){
    TrackCollection::const_iterator trkItr = trkTES->begin();
    TrackCollection::const_iterator trkItrE = trkTES->end();
    //we loop over that list
    for (; trkItr != trkItrE; trkItr++){
      const Trk::Perigee* trkPerigee = (*trkItr)->perigeeParameters();
      bool trackmatched = false;
      //we compare it to the tracks already associated with vertices
      
      VxContainer::const_iterator vtxItr  = vtxTES->begin();
      VxContainer::const_iterator vtxItrE = vtxTES->end();  
      
      ATH_MSG_DEBUG("Found "<<vtxTES->size()<<" vertices");  
      
      int i_vtx = 0;
      //  We're relying here on the implicit sorting of vertices by sqrt(N_tracks)*Sum Pt_track^2
      //  This should pick out the most interesting N vertices
      //  Hopefully we have only 1 primary vertex, but if there is > 1 we can grab all of those too
      for (; vtxItr != vtxItrE; vtxItr++){
  if ( (!m_priOnly || (*vtxItr)->vertexType() == 1) && (i_vtx < m_maxVtx) ){
    i_vtx++;
    std::vector<Trk::VxTrackAtVertex*> vertexTracks = (*(*vtxItr)->vxTrackAtVertex());
    ATH_MSG_DEBUG("parent vertex has "<<vertexTracks.size()<<" tracks, at position: "<<(*vtxItr)->recVertex().position().x());
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tavI = vertexTracks.begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator tavIe= vertexTracks.end();
    for (; tavI != tavIe; tavI++){
      const Trk::TrackParameters* vxTrkPerigee = (*tavI)->initialPerigee();
      if (trkPerigee == vxTrkPerigee) {trackmatched = true;}
    }
    Trk::Track *trkCopy1 = new Trk::Track((*(*trkItr)));
    Trk::Track *trkCopy2 = new Trk::Track((*(*trkItr)));
    if (trackmatched == false){
      std::string oeNameString;
      if (isUnmatchOdd)  oeNameString = "odd";
      if (!isUnmatchOdd) oeNameString = "even";
      std::stringstream sss;
      sss << oeNameString << "_" << addToVxUnmatch << "_Tracks";
      std::string oecontainerName = sss.str();          
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << addToVxUnmatch << "_Tracks";
      std::string allcontainerName = sss.str();  
      ATH_MSG_DEBUG("found an unmatched track, giving it the key: "<< oecontainerName); 
      trackmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found an unmatched track, giving it the key: "<< allcontainerName);  
      trackmap[allcontainerName]->push_back(trkCopy2);  
      isUnmatchOdd = !isUnmatchOdd;
      addToVxUnmatch++;
      if (addToVxUnmatch > m_maxVtx) addToVxUnmatch = 1;
    }
    if (trackmatched == true){
      std::string oeNameString;
      if (isMatchedOdd)  oeNameString = "odd";
      if (!isMatchedOdd) oeNameString = "even";
      std::stringstream sss;
      sss << oeNameString << "_" << addToVxMatched << "_Tracks";
      std::string oecontainerName = sss.str();          
      std::string allNameString = "all";
      sss.str("");
      sss << allNameString << "_" << addToVxMatched << "_Tracks";
      std::string allcontainerName = sss.str();  
      ATH_MSG_DEBUG("found an matched track, giving it the key: "<< oecontainerName); 
      trackmap[oecontainerName]->push_back(trkCopy1);
      ATH_MSG_DEBUG("found an matched track, giving it the key: "<< allcontainerName);  
      trackmap[allcontainerName]->push_back(trkCopy2);  
      isMatchedOdd = !isMatchedOdd;
      addToVxMatched++;
      if (addToVxMatched > m_maxVtx) addToVxMatched = 1;
    }
  }
      }
    }
  }

  //Start Track Info Dump
  //std::cout<<"Sending "<<tpbmap["odd_1_Tracks"]->size()<<" Tracks"<<std::endl;
  //Trk::TrackParticleBaseCollection::const_iterator ot1i = tpbmap["odd_1_Tracks"]->begin();
  //Trk::TrackParticleBaseCollection::const_iterator ot1e = tpbmap["odd_1_Tracks"]->end();
  //std::cout<<"Dumping Odd Tracks for Event"<<std::endl;
  //for (; ot1i != ot1e; ot1i++){
  //  std::cout<<"Next Track"<<std::endl;
  //  std::cout<<eventN<<": "<<(*ot1i)->definingParameters().momentum().eta()<<" , "<<(*ot1i)->definingParameters().momentum().phi()<<std::endl;
  //  std::cout<<std::endl;
  //}
  //End Track Info Dump

  if (m_savetpb){
        for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end() ; key_iter++){
      if(evtStore()->record(tpbmap[(*key_iter)],(*key_iter),false).isFailure() ){
  ATH_MSG_ERROR("Could not save the "<< (*key_iter));
      }}}
  else{
    for (key_iter = m_trackKeys.begin(); key_iter != m_trackKeys.end() ; key_iter++){
      if(evtStore()->record(trackmap[(*key_iter)],(*key_iter),false).isFailure() ){
  ATH_MSG_ERROR("Could not save the "<< (*key_iter));
      }}}

    
  ATH_MSG_DEBUG("split_vertices() succeeded");
  
  eventN++;
  return StatusCode::SUCCESS;
}

