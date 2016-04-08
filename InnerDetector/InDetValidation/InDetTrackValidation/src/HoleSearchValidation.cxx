/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HoleSearchValidation.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "InDetTrackValidation/HoleSearchValidation.h"
// Gaudi 
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"

#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

#include "TrkMeasurementBase/MeasurementBase.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SiliconID.h"

#include "CLHEP/Random/RandFlat.h"

using namespace InDet;

HoleSearchValidation::HoleSearchValidation(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name, pSvcLocator),
	m_idHelper(0),
	m_pixelID(0),
	m_sctID(0),
	m_trtID(0),
	m_siliconID(0),
    m_holeSearchTool("InDet::InDetTrackHoleSearchTool"),
    m_trackCollectionName("Tracks"),
	m_saveNewTracksInSG(false),
	m_removeSctSide0(true),
	m_removeSctSide1(true),
	m_removePix0(false),
	m_removePix1(false),
	m_removePix2(false),
	m_removeSct0(true),
	m_removeSct1(true),
	m_removeSct2(false),
	m_removeSct3(false),
	m_removeSct4(false),
	m_removeSct5(false),
	m_removeSct6(false),
	m_removeSct7(false),
	m_removeSct8(false),
	m_removeOverlapHitsOnly(false),
    m_ignoreTrackEnds(true),
	m_randomRemovalMode(false),
    m_maxNumberOfHoles(3),
  	m_rndmGenSvc("AtRndmGenSvc", name),
  	m_randomEngine(0),
  	m_randomEngineName("HoleSearchRnd")

{
  declareProperty("HoleSearchTool"          , m_holeSearchTool);
  declareProperty("TrackCollection"         , m_trackCollectionName, "Name of the reference track collection");

  declareProperty("SaveNewTracksInSG"       , m_saveNewTracksInSG  , "Boolean: save tracks in SG");
  declareProperty("RemoveSCTSide0"          , m_removeSctSide0     , "Remove SCT hits on side 0");
  declareProperty("RemoveSCTSide1"          , m_removeSctSide1     , "Remove SCT hits on side 1");

  declareProperty("RemovePIX0"              , m_removePix0         , "Remove hit(s) in PIXEL layer 0");
  declareProperty("RemovePIX1"              , m_removePix1         , "Remove hit(s) in PIXEL layer 1");
  declareProperty("RemovePIX2"              , m_removePix2         , "Remove hit(s) in PIXEL layer 2");

  declareProperty("RemoveSCT0"              , m_removeSct0         , "Remove hit(s) in SCT layer 0");
  declareProperty("RemoveSCT1"              , m_removeSct1         , "Remove hit(s) in SCT layer 1");
  declareProperty("RemoveSCT2"              , m_removeSct2         , "Remove hit(s) in SCT layer 2");
  declareProperty("RemoveSCT3"              , m_removeSct3         , "Remove hit(s) in SCT layer 3");

  declareProperty("RemoveSCT4"              , m_removeSct4         , "Remove hit(s) in SCT endcap layer 4");
  declareProperty("RemoveSCT5"              , m_removeSct5         , "Remove hit(s) in SCT endcap layer 5");
  declareProperty("RemoveSCT6"              , m_removeSct6         , "Remove hit(s) in SCT endcap layer 6");
  declareProperty("RemoveSCT7"              , m_removeSct7         , "Remove hit(s) in SCT endcap layer 7");
  declareProperty("RemoveSCT8"              , m_removeSct8         , "Remove hit(s) in SCT endcap layer 8");

  declareProperty("RemoveOverlapHitsOnly"  , m_removeOverlapHitsOnly, "Only remove overlap track hits");
  declareProperty("IgnoreTrackEnds"        , m_ignoreTrackEnds     , "Ignore hits at the end of the track");
  
  declareProperty("RandomRemovalMode"       , m_randomRemovalMode  , "Randomly remove track hits (overwrites the other flags!)");
  declareProperty("MaximalHoles"            , m_maxNumberOfHoles   , "Number of maximal holes to be created");
  

  // start with 10x10
  m_trackStats = std::vector< std::vector< unsigned int> >(0, std::vector< unsigned int>(0));
}

HoleSearchValidation::~HoleSearchValidation() {}

StatusCode HoleSearchValidation::initialize() {
  ATH_MSG_INFO( "HoleSearchValidation initialize()" ) ;

  //ID Helper
  StatusCode sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
     ATH_MSG_WARNING(  "Could not get AtlasDetectorID !" ) ;
     return StatusCode::SUCCESS;
  }else{
     ATH_MSG_DEBUG( "Found AtlasDetectorID" ) ;
  }
   
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "Could not get Pixel ID helper !" ) ;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "Initialized PixelIDHelper" ) ;
  
  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "Could not get SCT ID helper !" ) ;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "Initialized SCTIDHelper" ) ;
 
  sc = detStore()->retrieve(m_trtID, "TRT_ID");
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "Could not get TRT ID helper !" ) ;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "Initialized TRTIDHelper" ) ;

  sc = detStore()->retrieve(m_siliconID, "SiliconID");
  if (sc.isFailure()) {
    ATH_MSG_WARNING(  "Could not get Silicon ID helper !" ) ;
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG(  "Initialized SiliconIDHelper" ) ;


  // Get HoleSearchTool
  sc = m_holeSearchTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL(  "Could not retrieve "<< m_holeSearchTool ) ;
    return sc;
  }  
  
  if ( m_rndmGenSvc.retrieve().isFailure() ){
    ATH_MSG_FATAL( "Could not retrieve " << m_rndmGenSvc );
    return StatusCode::FAILURE;
  }
  //Get own engine with own seeds:
  m_randomEngine = m_rndmGenSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
      ATH_MSG_FATAL( "Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode HoleSearchValidation::execute() {

  ATH_MSG_VERBOSE( "HoleSearchValidation execute() start" ) ;

  StatusCode sc = StatusCode::SUCCESS;
  
  const TrackCollection* tracks = 0;
  TrackCollection* newTrackCollection = 0;
  if (m_saveNewTracksInSG) {
    newTrackCollection = new TrackCollection();
  }
  

  // get track collection
  if (m_trackCollectionName != "") {
    sc = evtStore()->retrieve(tracks, m_trackCollectionName);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "tracks not found:  " << m_trackCollectionName ) ;
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE( "tracks found: " << m_trackCollectionName
						  << " with size = " << tracks->size() ) ;
    }
  }else{
    ATH_MSG_ERROR( "No track collection name given (empty string)!" );
    return StatusCode::FAILURE;
  }


  // loop over tracks
  TrackCollection::const_iterator trackIterator = tracks->begin();
  for ( ; trackIterator < tracks->end(); ++trackIterator) {
    if (!((*trackIterator))) {
      ATH_MSG_WARNING( "TrackCollection " << m_trackCollectionName << "contains empty entries" ) ;
      continue;
    }

    // get all TSOS
    const Trk::Track& track = *(*trackIterator);
    const DataVector<const Trk::TrackStateOnSurface>* tsos = track.trackStateOnSurfaces();
    ATH_MSG_DEBUG(  "Perform hole search on unmodified track (" << *trackIterator << ")" 
		   << " which contains " << tsos->size() <<" track states" ) ;
    // perform hole search
    unsigned int oldHoles = doHoleSearch( *trackIterator );



    // remve some hits from track
    //msg(MSG::VERBOSE)<<"Copying all but "<<m_numberHitsToRemove <<" into new track"<<endreq;    
    DataVector<const Trk::TrackStateOnSurface>* vecTsos 
      = new DataVector<const Trk::TrackStateOnSurface>();
    
    // loop over TSOS, copy TSOS and push into vector
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end();

    unsigned int nRemoved = 0;
    unsigned int counter  = 0;

    std::vector< unsigned int > pixelHitsPerLayer = std::vector< unsigned int>(3);
    std::vector< unsigned int > sctHitsPerLayer = std::vector< unsigned int>(9);

    // in case of removing the end hits we need to parse the track first once,
    //   variables will be call pl (for preloop)
    int maxPixelLayerBarrel = -1;
    int maxPixelLayerEndcap = -1;
    int maxSctLayerBarrel   = -1;
    int maxSctLayerEndcap   = -1;

    if (m_ignoreTrackEnds || m_randomRemovalMode){
        ATH_MSG_VERBOSE(  "Parsing track first to find end layers and maximal numbers" ) ;
    	for ( ; iTsos != iTsosEnd ; ++iTsos) {
			// 
            Identifier plSurfaceID;
			const Trk::MeasurementBase* plMesb = (*iTsos)->measurementOnTrack();
	        if (plMesb &&  plMesb->associatedSurface().associatedDetectorElement()){
            	plSurfaceID = plMesb->associatedSurface().associatedDetectorElement()->identify(); 
                // to find out whether it is barrel / endcap
  				// check pixel / sct 
                if ( m_idHelper->is_pixel( plSurfaceID ) ) {
	    			int plLayer = abs(m_pixelID->layer_disk( plSurfaceID ));
                    bool isBarrel = m_pixelID->is_barrel( plSurfaceID );
                    // set the maximal pixel layer: barrel / ec
					if ( isBarrel )
						maxPixelLayerBarrel = plLayer > maxPixelLayerBarrel ? plLayer : maxPixelLayerBarrel;
                    else 
						maxPixelLayerEndcap = plLayer > maxPixelLayerEndcap ? plLayer : maxPixelLayerEndcap;
                } else if (m_idHelper->is_sct( plSurfaceID ) ) {
				    int plLayer = abs(m_sctID->layer_disk(  plSurfaceID )); 
                    bool isBarrel = m_sctID->is_barrel( plSurfaceID );
                    // set the maximal pixel layer: barrel / ec
					if ( isBarrel )
						maxSctLayerBarrel = plLayer > maxSctLayerBarrel ? plLayer : maxSctLayerBarrel;
                    else 
						maxSctLayerEndcap = plLayer > maxSctLayerEndcap ? plLayer : maxSctLayerEndcap;
               }
            }
        }
        // sct overrules pixels
        maxPixelLayerBarrel = maxSctLayerBarrel > 0 ? -1 : maxPixelLayerBarrel;
        maxPixelLayerEndcap = maxSctLayerEndcap > 0 ? -1 : maxPixelLayerEndcap;
      // reset to start the main loop correctly
      iTsos    = tsos->begin();
    }

    
    for ( ; iTsos != iTsosEnd ; ++iTsos) {
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)) {
	counter++;
	
	Identifier surfaceID;
	const Trk::MeasurementBase* mesb = (*iTsos)->measurementOnTrack();

    // also reset the identifiers
    unsigned int randomHoles = 0;
    if (m_randomRemovalMode){
       // ---------------------------------------------------------------------------------------
       randomHoles = (unsigned int)(m_maxNumberOfHoles*CLHEP::RandFlat::shoot(m_randomEngine));
       ATH_MSG_VERBOSE("Random mode chosen: will create " << randomHoles << " holes on the track.");

       // reset the all the triggers (they will be set randomly)
	   m_removeSctSide0 = false;
	   m_removeSctSide1 = false;
	   m_removePix0 = false;
	   m_removePix1 = false;
	   m_removePix2 = false;
	   m_removeSct0 = false;
	   m_removeSct1 = false;
	   m_removeSct2 = false;
	   m_removeSct3 = false;
	   m_removeSct4 = false;
	   m_removeSct5 = false;
	   m_removeSct6 = false;
	   m_removeSct7 = false;
	   m_removeSct8 = false;

       // max int pixel  
       unsigned int maxPixel = maxPixelLayerBarrel > maxPixelLayerEndcap 
                    ? maxPixelLayerBarrel : maxPixelLayerEndcap;
       // max int sct
       unsigned int maxSct = maxSctLayerBarrel > maxSctLayerEndcap 
                    ? maxSctLayerBarrel : maxSctLayerEndcap;
       // -------------------------------------------------------------------
       int maxHit 		  = maxPixel + maxSct;
       int holesTriggered = 0;
       maxHit -= m_ignoreTrackEnds ? 1 : 0;
       // make the switch
       for (unsigned int ihole = 0; ihole < randomHoles && holesTriggered < int(randomHoles); ++ihole){
            // throw the dices
			unsigned int holeId = (unsigned int)(maxHit*CLHEP::RandFlat::shoot(m_randomEngine));
            ATH_MSG_VERBOSE( "Random mode : layer identifier " << holeId << " chosen." );
           {
			 // now switch between --------
             switch (holeId) {
				case 0 : { m_removePix0 = true; ++holesTriggered; }; break;
				case 1 : { m_removePix1 = true; ++holesTriggered; }; break;
				case 2 : { m_removePix2 = true; ++holesTriggered; }; break;
				case 3 : { m_removeSct0 = true; ++holesTriggered; }; break;
				case 4 : { m_removeSct1 = true; ++holesTriggered; }; break;
				case 5 : { m_removeSct2 = true; ++holesTriggered; }; break;
				case 6 : { m_removeSct3 = true; ++holesTriggered; }; break;
				case 7 : { m_removeSct4 = true; ++holesTriggered; }; break;
				case 8 : { m_removeSct5 = true; ++holesTriggered; }; break;
				case 9 : { m_removeSct6 = true; ++holesTriggered; }; break;
				case 10 : { m_removeSct7 = true; ++holesTriggered; }; break;
				case 11 : { m_removeSct8 = true; ++holesTriggered; }; break;
                default : break;
            }
            // make the side decision on the side
	   		if (holeId > 2) {
                double sideDecision = CLHEP::RandFlat::shoot(m_randomEngine);
				if ( sideDecision < 1./3. )
					m_removeSctSide0 = true;
				else if ( sideDecision < 2./3. )
					m_removeSctSide1 = true;
				else {
					m_removeSctSide0 = true;
					m_removeSctSide1 = true;
                    ++holesTriggered;
                }
            }
         }
	   }
    }

	// hits, outliers
	if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement() != NULL) {
	  surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify(); 
      // the pixel case	  
	  if ( m_idHelper->is_pixel( surfaceID ) ) {
	    int layer = abs(m_pixelID->layer_disk( surfaceID ));
        // check barrel / ec
        bool isBarrel = m_pixelID->is_barrel( surfaceID );
        // indicate how many pixel hits are per layer
	    pixelHitsPerLayer[layer]++;
        ATH_MSG_VERBOSE(  "Pixel hits on layer " << layer << " : " << pixelHitsPerLayer[layer] ) ;
        // check for last layer
        bool isLastLayer = (isBarrel && layer == maxPixelLayerBarrel) || (!isBarrel && layer == maxPixelLayerEndcap);
		// only in the ignore track case
        if (m_ignoreTrackEnds && isLastLayer){
        	ATH_MSG_VERBOSE(  "This pixel hit is not removed, it is at the track end." ) ;
        } else if ( !m_removeOverlapHitsOnly || pixelHitsPerLayer[layer] > 1) {
	      if ( !m_ignoreTrackEnds && layer == 0 && m_removePix0 ) {
		ATH_MSG_DEBUG(  "Removing PIXEL layer 0 hit" ) ;
		printInfoTSoS( *iTsos );
		nRemoved++;
		continue;
	      } else if( layer == 1 && m_removePix1 ) {
		ATH_MSG_DEBUG(  "Removing PIXEL layer 1 hit" ) ;
		printInfoTSoS( *iTsos );
		nRemoved++;
		continue;
	      } else if( layer == 2 && m_removePix2 ) {
		ATH_MSG_DEBUG(  "Removing PIXEL layer 2 hit" ) ;
		printInfoTSoS( *iTsos );
		nRemoved++;
		continue;
	      } 
	    }
	  } else if ( m_idHelper->is_sct( surfaceID ) ) {
        int layer = abs(m_sctID->layer_disk( surfaceID ));
        // check barrel / ec
        bool isBarrel = m_sctID->is_barrel( surfaceID );
        // counter for number of layers
        sctHitsPerLayer[layer]++;	    
        ATH_MSG_VERBOSE(  "SCT hits on layer " << layer << " : " << sctHitsPerLayer[layer] ) ;
        // steer the side to be removed
	    int side  = m_sctID->side( surfaceID );
	    bool canRemoveSide = (side == 0) ? m_removeSctSide0 : m_removeSctSide1;
        // check for last layer
        bool isLastLayer = (isBarrel && layer == maxSctLayerBarrel) || (!isBarrel && layer == maxSctLayerEndcap);
		// only in the ignore track case
        if (m_ignoreTrackEnds && isLastLayer){
        	ATH_MSG_VERBOSE(  "This SCT hit is not removed, it is at the track end." ) ;
        } else if ( layer == 0 && m_removeSct0 && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT layer 0 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 1 && m_removeSct1  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT layer 1 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 2 && m_removeSct2  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT layer 2 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 3 && m_removeSct3  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT layer 3 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 4 && m_removeSct4  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT endcap layer 4 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 5 && m_removeSct5  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT endcap layer 5 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 6 && m_removeSct6  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT endcap layer 6 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 7 && m_removeSct7  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT endcap layer 7 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } else if ( layer == 8 && m_removeSct8  && canRemoveSide ) {
	      ATH_MSG_DEBUG(  "Removing SCT endcap layer 8 hit" ) ;
	      printInfoTSoS( *iTsos );
	      nRemoved++;
	      continue;
	    } 

	  } // end SCT hit
	 } // end have identifier
    } // end TSoS is of type measurement

     const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(**iTsos);
     vecTsos->push_back(newTsos);
    } // end loop over all TSoS
    
    ATH_MSG_DEBUG(  "Removed total of " << nRemoved << " TSoS on track." ) ;

    Trk::Track* newTrack = new Trk::Track(track.info(), vecTsos, 0 );
    msg(MSG::VERBOSE)<<"New Track ("<< newTrack <<") has "<< vecTsos->size() <<"\t track states"<<endreq;
    ATH_MSG_VERBOSE(  "Perform hole search on new track:" ) ;
    // perform hole search
    unsigned int newHoles = doHoleSearch( newTrack );
    unsigned int foundHoles = abs(newHoles - oldHoles);
    
    if ( foundHoles == nRemoved ) 
      ATH_MSG_DEBUG(  "== OK      : "<< nRemoved <<" generated holes out of which all were found" ) ;
    else
      ATH_MSG_DEBUG(  "== PROBLEM : "<< nRemoved <<" generated holes out of which "<< newHoles - oldHoles << " were found" ) ;

    // fill track statistics
    // first, check size of vectors
     while (m_trackStats.size() < nRemoved+1) {
       m_trackStats.push_back( std::vector< unsigned int >(0) );
     }
     while (m_trackStats[nRemoved].size() < foundHoles+1) {
       m_trackStats[nRemoved].push_back( 0 );
     }
     ATH_MSG_DEBUG(  "m_trackStats.size()= " << m_trackStats.size() );
//     // increase counter
     m_trackStats[nRemoved][foundHoles]++;


    // posibly save new track into container and SG
    if (m_saveNewTracksInSG) {
      newTrackCollection->push_back( newTrack );
    } else {
      delete newTrack; // TSOS are owned by track (so no need to delete them manually)
    }

  } // end loop over all input tracks
  ATH_MSG_DEBUG(  "Finished looping over all input tracks" ) ;

  if (m_saveNewTracksInSG) {
    // save new tracks to SG
    ATH_MSG_DEBUG(  "Saving new TrackCollection to SG" ) ;
    evtStore()->record( newTrackCollection, m_trackCollectionName+"_removedHits");
  }

  return sc;
}

StatusCode HoleSearchValidation::finalize() {
  ATH_MSG_INFO( "HoleSearchValidation finalize()" ) ;
  
  ATH_MSG_INFO(  "Printing statistics for hole search validation:" ) ;
  ATH_MSG_INFO(  " # removed hits | # tracks / found holes | ... " ) ;
  for ( unsigned int nRemoved = 0; nRemoved < m_trackStats.size(); ++nRemoved) {
    unsigned int nTracks = 0;
    for ( unsigned int holes = 0; holes < m_trackStats[nRemoved].size(); ++holes) nTracks +=  m_trackStats[nRemoved][holes];
    if ( nTracks == 0) continue;
    ATH_MSG_INFO(  "Removed " << nRemoved << " hits from track -- found: " ) ;
     for ( unsigned int holes = 0; holes < m_trackStats[nRemoved].size(); ++holes) {
       ATH_MSG_INFO(  "    -- " << 100.0 * m_trackStats[nRemoved][holes]/float(nTracks) 
		      << "% of tracks with " << holes << " holes (" <<  m_trackStats[nRemoved][holes] << " / " <<  float(nTracks) << ")") ;
     }
   }

  return StatusCode::SUCCESS;
}

unsigned int HoleSearchValidation::doHoleSearch( const Trk::Track* track)
{
  ATH_MSG_VERBOSE(  "start hole search for track ( " << track << ")" ) ;
  unsigned int nHoles(0);

  const DataVector<const Trk::TrackStateOnSurface>* holesOnTrack 
    = m_holeSearchTool->getHolesOnTrack(*track, track->info().particleHypothesis());
  // loop over holes
  if (!holesOnTrack) {
    ATH_MSG_WARNING(  "HoleSearchTool returned NULL !" ) ;
    return nHoles;
  } else {
    nHoles = holesOnTrack->size();
    ATH_MSG_DEBUG(  "found " << nHoles << " holes on track." ) ;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=holesOnTrack->begin();
	 it!=holesOnTrack->end();  ++it) {
      if (!(*it)) {
	ATH_MSG_WARNING(  "TrackStateOnSurface from hole search tool == Null" ) ;
	continue;
      }
      ATH_MSG_DEBUG(  "Found hole:" ) ;
      printInfoTSoS( *it );
    } // end loop on holes
  }
  delete holesOnTrack;
  return nHoles;
}


void HoleSearchValidation::printInfoTSoS( const Trk::TrackStateOnSurface* tsos)
{

  ATH_MSG_VERBOSE(  "Position : " << tsos->trackParameters()->position()
		    << " with R = " << sqrt(pow(tsos->trackParameters()->position().x(),2)
					    +pow(tsos->trackParameters()->position().y(),2)) );
  Identifier surfaceID;
  const Trk::MeasurementBase* mesb = tsos->measurementOnTrack();
  // hits, outliers
  if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement() != NULL) {
    surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify(); 
    if ( m_siliconID->is_barrel( surfaceID ) ) {
      ATH_MSG_VERBOSE(  " -- Barrel:");
    } else {
      ATH_MSG_VERBOSE(  " -- Endcap:");
    }
    if ( m_idHelper->is_sct( surfaceID ) ) {
      ATH_MSG_VERBOSE(  "    -> SCT layer = " << m_sctID->layer_disk( surfaceID )
			<< " side = " << m_sctID->side( surfaceID ) );
      //ATH_MSG_VERBOSE(  m_sctID->show_to_string(surfaceID) ) ;
      
    }
    if ( m_idHelper->is_pixel( surfaceID ) ) {
      ATH_MSG_VERBOSE(  "    -> PIXEL layer = " << m_pixelID->layer_disk( surfaceID ) );
      //ATH_MSG_VERBOSE(  m_pixelID->show_to_string(surfaceID) ) ;
    }
  }

}
