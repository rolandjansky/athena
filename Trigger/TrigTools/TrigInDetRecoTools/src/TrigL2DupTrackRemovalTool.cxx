/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <iostream>

// general Atlas classes
//#include "GaudiKernel/PropertyMgr.h"
//#include "GaudiKernel/MsgStream.h"

//#include "AthenaKernel/Timeout.h"

/*#include "EventInfo/EventInfo.h"
  #include "EventInfo/EventID.h"
  #include "eformat/DetectorMask.h"

  #include "TrigTimeAlgs/TrigTimerSvc.h"
  #include "TrigInDetEvent/TrigSiSpacePoint.h"
  #include "TrigInDetEvent/TrigSiSpacePointCollection.h"
  #include "TrigInDetEvent/TrigVertex.h"
  #include "TrigInDetEvent/TrigVertexCollection.h"
  #include "TrigInDetEvent/TrigInDetTrack.h"
  #include "TrigInDetEvent/TrigInDetTrackFitPar.h"
  #include "TrigSiSpacePointTool/ISpacePointProvider.h"

  #include "TrigInDetToolInterfaces/ITrigZFinder.h"
  #include "TrigInDetToolInterfaces/ITrigHitFilter.h"
  #include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
  #include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
  #include "TrigInDetToolInterfaces/TrigL2HitResidual.h"
  #include "TrigInDetToolInterfaces/ITrigL2ResidualCalculator.h"

  #include "TrkTrack/TrackCollection.h"
  #include "TrkTrack/Track.h"

  #include<sstream>*/

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetRecoTools/ITrigL2DupTrackRemovalTool.h"
#include "TrigInDetRecoTools/TrigL2DupTrackRemovalTool.h"

//using namespace std;


TrigL2DupTrackRemovalTool::TrigL2DupTrackRemovalTool(const std::string& t, const std::string& n, const IInterface*  p ): 
  AthAlgTool(t,n,p)
{
  declareInterface< ITrigL2DupTrackRemovalTool >( this ); 
  
  declareProperty( "mode",  m_mode = 2 );
}
    

TrigL2DupTrackRemovalTool::~TrigL2DupTrackRemovalTool() {

}


StatusCode TrigL2DupTrackRemovalTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  athenaLog << MSG::INFO << "TrigL2DupTrackRemovalTool initialized "<< endmsg;
  return sc;
}


StatusCode TrigL2DupTrackRemovalTool::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 

  //athenaLog << MSG::INFO << "TrigL2DupTrackRemovalTool finalized "<< endmsg;
  return sc;
}


void TrigL2DupTrackRemovalTool::removeDupTracks(TrigInDetTrackCollection* trackColl,std::vector<unsigned int> placeHolder)
{
  TrigInDetTrack * deleteMe;
  if (m_mode==1) {
    ///////////////////////////////////////////
    /////// DupTrackRemovalTool mode 1 ////////
    // Deleting duplicate tracks based on DR //
    ///////////////////////////////////////////
    TrigInDetTrackCollection::iterator track = trackColl->begin();
    TrigInDetTrackCollection::iterator lastTrack = trackColl->end();
    int trackNum2(0);
    double R(0.);
    TrigInDetTrackCollection::iterator track2 = trackColl->begin();
    for(int trackNum=0; track !=lastTrack; track++, trackNum++){
      for(trackNum2=trackNum, track2 = track; track2 !=lastTrack; trackNum2++, track2++){
	if(track != track2){
	  R = sqrt( pow((*track)->param()->phi0()-(*track2)->param()->phi0(),2) +
		    pow((*track)->param()->eta() -(*track2)->param()->eta(),2) );
	  if(R < 0.01){
	    //delete closeby tracks and keep the one with more spacepoints or a lower chi2
	    //should I be using overall number of spacepoints?
	    if((*track)->NPixelSpacePoints() > (*track2)->NPixelSpacePoints()){
	      trackColl->swapElement(trackNum2, 0, deleteMe);
	      trackColl->erase(track2);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	    }else if((*track2)->NPixelSpacePoints() > (*track)->NPixelSpacePoints()){
	      trackColl->swapElement(trackNum, 0, deleteMe);
	      trackColl->erase(track);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	      track--;
	      trackNum -=1;
	      break;
	    }else if((*track)->NSCT_SpacePoints() > (*track2)->NSCT_SpacePoints()){
	      trackColl->swapElement(trackNum2, 0, deleteMe);
	      trackColl->erase(track2);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	    }else if((*track2)->NSCT_SpacePoints() > (*track)->NSCT_SpacePoints()){
	      trackColl->swapElement(trackNum, 0, deleteMe);
	      trackColl->erase(track);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	      track--;
	      trackNum -=1;
	      break;
	    }else if((*track)->chi2() < (*track2)->chi2()){
	      trackColl->swapElement(trackNum2, 0, deleteMe);
	      trackColl->erase(track2);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	    }else{
	      trackColl->swapElement(trackNum, 0, deleteMe);
	      trackColl->erase(track);
	      delete deleteMe;
	      deleteMe = NULL;
	      lastTrack = trackColl->end();
	      track--;
	      trackNum -=1;
	      break;
	    }
	    track2--;
	    trackNum2 -=1;
	  }
	}
      }
    }
  }

  if (m_mode==2) {
    ///////////////////////////////////////////////////////////////////////////
    /////////////////////// DupTrackRemovalTool mode 2 ////////////////////////
    // Deleting duplicate tracks from multiple z vertices based on shared SP //
    ///////////////////////////////////////////////////////////////////////////
    unsigned int nZvertices = placeHolder.size();
    if (nZvertices==1) return;
    unsigned int trackNum2(0), numPointsShared(0);
    std::vector<const TrigSiSpacePoint*>* trackPoints1;
    std::vector<const TrigSiSpacePoint*>* trackPoints2;
    for(int trackNum1=0; trackNum1 < int (placeHolder[nZvertices - 1]); trackNum1++){
      //only loop through tracks from other vertices 
      for(unsigned int i=0;i<placeHolder.size();i++){
	if(trackNum1 < int (placeHolder[i])){
	  trackNum2 = placeHolder[i];
	  break;
	} 
      }
      trackPoints1 = trackColl->at(trackNum1)->siSpacePoints();
      for(; trackNum2 < trackColl->size(); trackNum2++){
	trackPoints2 = trackColl->at(trackNum2)->siSpacePoints();
	numPointsShared = 0;
	//check if the two tracks share any spacpoints
	for(unsigned int k=0;k<trackPoints1->size(); k++){
	  for(unsigned int j=0;j < trackPoints2->size(); j++){
	    if(trackPoints1->at(k) == trackPoints2->at(j)){
	      numPointsShared +=1; 
	    }
	  }
	  if(numPointsShared > 2) break;
	}
	if(numPointsShared > 2){
	  //delete one of the duplicate tracks, keeping the one with more spacepoints or a lower chi2
	  if(trackColl->at(trackNum1)->NPixelSpacePoints() > trackColl->at(trackNum2)->NPixelSpacePoints()){
	    trackColl->swapElement(trackNum2, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum2);
	    delete deleteMe;
	    deleteMe = NULL;
	  }else if(trackColl->at(trackNum2)->NPixelSpacePoints() > trackColl->at(trackNum1)->NPixelSpacePoints()){
	    trackColl->swapElement(trackNum1, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum1);
	    delete deleteMe;
	    deleteMe = NULL;
	    for(unsigned int l = 0;l<placeHolder.size();l++){
	      if(trackNum1 < int (placeHolder[l])) placeHolder[l] -= 1;
	    }
	    trackNum1 -=1;
	    break;
	  }else if(trackColl->at(trackNum1)->NSCT_SpacePoints() > trackColl->at(trackNum2)->NSCT_SpacePoints()){
	    trackColl->swapElement(trackNum2, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum2);
	    delete deleteMe;
	    deleteMe = NULL;
	  }else if(trackColl->at(trackNum2)->NSCT_SpacePoints() > trackColl->at(trackNum1)->NSCT_SpacePoints()){
	    trackColl->swapElement(trackNum1, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum1);
	    delete deleteMe;
	    deleteMe = NULL;
	    for(unsigned int l = 0;l<placeHolder.size();l++){
	      if(trackNum1 < int (placeHolder[l])) placeHolder[l] -= 1;
	    }
	    trackNum1 -=1;
	    break;
	  }else if(trackColl->at(trackNum1)->chi2() < trackColl->at(trackNum2)->chi2()){
	    trackColl->swapElement(trackNum2, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum2);
	    delete deleteMe;
	    deleteMe = NULL;
	  }else{
	    trackColl->swapElement(trackNum1, 0, deleteMe);
	    trackColl->erase(trackColl->begin() + trackNum1);
	    delete deleteMe;
	    deleteMe = NULL;
	    for(unsigned int l = 0;l<placeHolder.size();l++){
	      if(trackNum1 < int (placeHolder[l])) placeHolder[l] -= 1;
	    }
	    trackNum1 -=1;
	    break;
	  }
	  for(unsigned int l = 0;l<placeHolder.size();l++){
	    if(trackNum2 < placeHolder[l]) placeHolder[l] -= 1;
	  }
	  trackNum2 -=1;
	}
      }
    }
  }
      
}
