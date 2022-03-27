/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetCosmicsFilters/TrackFilter.h"

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
  m_accept(0),
  m_idHelper{}
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
  
 
  ATH_MSG_DEBUG( " initialize()");

  ATH_CHECK (detStore()->retrieve(m_idHelper, "AtlasID"));
  
  if(m_inputTrackCollectionNames.empty()){
    ATH_MSG_ERROR( "Configuration problem: TrackFilter configured with empty list of TrackCollections. Please fix your jobOptions!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "initialize() successful");
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TrackFilter::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  ATH_MSG_DEBUG( "finalize() successful ");
  ATH_MSG_INFO( "Events / accepted  = "<<m_evcount <<" / "<<m_accept);

  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TrackFilter::execute()
{

  bool passed=false;

  m_evcount++;

  ATH_MSG_DEBUG( "execute() called" );


  std::vector<std::string>::const_iterator it=m_inputTrackCollectionNames.begin();
  std::vector<std::string>::const_iterator itE=m_inputTrackCollectionNames.end();

  for(;it!=itE && !passed;++it){

  // input TrackSegment Collection
    const TrackCollection *tracks   = nullptr;
    
    StatusCode sc = evtStore()->retrieve(tracks, *it);
    if(sc.isFailure()){
      ATH_MSG_WARNING(" could not open Track collection : " << *it );
      ATH_MSG_WARNING( " will skip this collection!!");
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
          
          if (hitOnTrack != nullptr) {
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
  ATH_MSG_DEBUG( "Set filter passed to " << filterPassed() );

  return StatusCode::SUCCESS;
}

//============================================================================================
