/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetCosmicsFilters/TrackFilter.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"


//================ Constructor =================================================

InDet::TrackFilter::TrackFilter(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
  m_inputTrackCollectionNames(),
  m_evcount(0),
  m_accept(0)
{
  //  template for property decalration
  declareProperty("MinTRTHits", m_minTRTHits=1);
  declareProperty("MinSCTHits", m_minSCTHits=1);
  declareProperty("MinPixelHits", m_minPixelHits=-1000);
  declareProperty("MinIDHits", m_minIDHits=1);
  declareProperty("MinSCTonlyHits", m_minSCTonlyHits=6);
  declareProperty("MinTRTonlyHits", m_minTRTonlyHits=20);
  declareProperty("TrackCollections",m_inputTrackCollectionNames);  
}

//================ Destructor =================================================

InDet::TrackFilter::~TrackFilter()
{}


//================ Initialisation =================================================

StatusCode InDet::TrackFilter::initialize()
{
  // Code entered here will be executed once at program start.
  
 
  msg(MSG::INFO) << name() << " initialize()" << endmsg;

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if(m_inputTrackCollectionNames.size()==0){
    msg(MSG::ERROR) << "Configuration problem: TrackFilter configured with empty list of TrackCollections. Please fix your jobOptions!"<<endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TrackFilter::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  msg(MSG::INFO) << "finalize() successful in " << name() << endmsg;
  msg(MSG::INFO) << "Events / accepted  = "<<m_evcount <<" / "<<m_accept<<endmsg;

  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TrackFilter::execute()
{

  bool passed=false;

  m_evcount++;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "execute() called in " << name() << endmsg;


  std::vector<std::string>::const_iterator it=m_inputTrackCollectionNames.begin();
  std::vector<std::string>::const_iterator itE=m_inputTrackCollectionNames.end();

  for(;it!=itE && !passed;it++){

  // input TrackSegment Collection
    const TrackCollection *tracks   = 0;
    
    StatusCode sc = evtStore()->retrieve(tracks, *it);
    if(sc.isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " could not open Track collection : " << *it << endmsg;
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << " will skip this collection!!"<<endmsg;
    }else{

      //loop over trackcollection and try to find a "good" track
      TrackCollection::const_iterator itrk = tracks->begin();
      TrackCollection::const_iterator itrkEnd = tracks->end();
      
      for(; itrk != itrkEnd && !passed; ++itrk) {
        const Trk::Track *track=*itrk;
        
        int nscthits=0;
        int ntrthits=0;
	int npixhits=0;

        for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
          
          const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
          
          if (hitOnTrack != 0) {
            const Identifier& surfaceID = hitOnTrack->identify();
            
            if(m_idHelper->is_sct(surfaceID)){
              nscthits++;
            }else if(m_idHelper->is_trt(surfaceID)){
              ntrthits++;
            }else if(m_idHelper->is_pixel(surfaceID)){
	      npixhits++;
	    }
          }
        }
	int sumIDhits=nscthits+ntrthits+npixhits;
        if((ntrthits>=m_minTRTHits && nscthits >= m_minSCTHits && npixhits >= m_minPixelHits && sumIDhits >= m_minIDHits) ||
           (nscthits >= m_minSCTonlyHits) || (ntrthits>=m_minTRTonlyHits))
          passed=true;
      }
    }
  }


  if(passed){
    m_accept++;
  }



  setFilterPassed( passed );
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Set filter passed to " << filterPassed() << endmsg;

  return StatusCode::SUCCESS;
}

//============================================================================================
