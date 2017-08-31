/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetAmbiTrackSelectionTool.cxx, (c) ATLAS Detector software
// Markus Elsing
///////////////////////////////////////////////////////////////////

#include "InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool.h"
#include "InDetIdentifier/SiliconID.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRecToolInterfaces/ITrtDriftCircleCutTool.h"
#include "PixelGeoModel/IBLParameterSvc.h"

#include <ext/functional>
//================ Constructor =================================================

InDet::InDetAmbiTrackSelectionTool::InDetAmbiTrackSelectionTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_assoTool("Trk::PRD_AssociationTool/PRD_AssociationTool"),
  m_selectortool("InDet::InDetTrtDriftCircleCutTool"       ),
  m_IBLParameterSvc("IBLParameterSvc",n),
  // Initialization of atlas id helper 
  m_detID(nullptr)

{
  declareInterface<IAmbiTrackSelectionTool>(this);

  //  template for property decalration
  declareProperty("AssociationTool"      , m_assoTool);
  declareProperty("DriftCircleCutTool"   , m_selectortool);
  declareProperty("minHits"              , m_minHits                 = 5);
  declareProperty("minTRTHits"           , m_minTRT_Hits             = 0);
  declareProperty("maxShared"            , m_maxSharedModules        = 1);
  declareProperty("minScoreShareTracks"  , m_minScoreShareTracks     = 0.0);
  declareProperty("maxTracksPerSharedPRD", m_maxTracksPerPRD         = 2);
  declareProperty("minNotShared"         , m_minNotShared            = 6);
  declareProperty("Cosmics"              , m_cosmics                 = false);
  declareProperty("UseParameterization"  , m_parameterization        = true);
  declareProperty("doPixelSplitting"     , m_doPixelClusterSplitting = false);
  declareProperty("sharedProbCut"        , m_sharedProbCut           = 0.02);
  // A.S.: remove that when solved properly by updating the SplitProb info with isExcluded
  declareProperty("MaximalSplitSize"     , m_maxSplitSize            = 49);
  

  // compute the number of shared hits from the number of max shared modules
  m_maxShared=2*m_maxSharedModules+1;
    
}

//================ Destructor =================================================

InDet::InDetAmbiTrackSelectionTool::~InDetAmbiTrackSelectionTool()
{}


//================ Initialisation =================================================

StatusCode InDet::InDetAmbiTrackSelectionTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;
  if (m_IBLParameterSvc.retrieve().isFailure()) {
       ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  }
  else {
	m_IBLParameterSvc->setBoolParameters(m_doPixelClusterSplitting,"doPixelClusterSplitting");
  }

  sc =  m_assoTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_assoTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else
    msg(MSG::INFO) << "Retrieved tool " << m_assoTool << endmsg;

  // Get segment selector tool
  //
  if(m_parameterization){
    if(m_selectortool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_selectortool <<endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_selectortool << endmsg;
    }
  }

  sc = detStore()->retrieve(m_detID, "SiliconID" );
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Could not get SiliconID "<<endmsg;
      return sc;
    }
  else
    msg(MSG::DEBUG) << "Found SiliconID"<<endmsg;

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetAmbiTrackSelectionTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================
const Trk::Track* InDet::InDetAmbiTrackSelectionTool::getCleanedOutTrack(const Trk::Track* ptrTrack, const Trk::TrackScore score) 
{
  // flag if the track is ok (true) or needs cleaning (false)
  bool TrkCouldBeAccepted        = true;

  // some counters used in the logic
  int  numUnused         = 0;
  int  numTRT_Unused     = 0;
  int  numPseudo         = 0;
  int  numShared         = 0;
  int  numWeightedShared = 0;
  bool thishasblayer     = false;
  bool hassharedblayer   = false;
  bool hassharedpixel    = false;
  bool firstisshared     = true; // logic is that it is set to false if we find a first hit unshared 

  // let's remember the last 2 ROTs on the track
  const Trk::RIO_OnTrack* lastrot       = 0;
  const Trk::RIO_OnTrack* lastbutonerot = 0;  
  int                     lastrotindex  = 0;

  // cut on TRT hits, might use eta dependent cuts here
  int  nCutTRT = m_minTRT_Hits;
  if(m_parameterization) {
    const DataVector<const Trk::TrackParameters>* vpar = ptrTrack->trackParameters();
    const Trk::TrackParameters*                    par = (*vpar)[0];
    // ME bugfix
    int expected = m_selectortool->minNumberDCs(par);
    if (expected > nCutTRT) nCutTRT = expected; 
  }

  // get all TSOS the track
  const DataVector<const Trk::TrackStateOnSurface>* tsos = ptrTrack->trackStateOnSurfaces();
  ATH_MSG_DEBUG ("Study new Track "<< ptrTrack<<"\t , it has "<<tsos->size()<<"\t track states");

  // is this a track from the pattern or a fitted track ?
  bool ispatterntrack = (ptrTrack->info().trackFitter()==Trk::TrackInfo::Unknown);
  if (ispatterntrack) 
    ATH_MSG_DEBUG ("==> this is a pattern track, outliers are good hits (reintegration) !");
  else
    ATH_MSG_DEBUG ("==> this is a refitted track, so we can use the chi2 ! ");

  // some pre-processing of the summary information, if available, needed for special cuts
  int nPixelDeadSensor = -1;
  int nSCTDeadSensor   = -1;
  int npixel    = 0;
  int npixholes = 0;
  int nsctholes = 0;
  const Trk::TrackSummary* trkSummary=ptrTrack->trackSummary();
  if (trkSummary) {
    ATH_MSG_VERBOSE ("---> Found summary information");
    nPixelDeadSensor = trkSummary->get(Trk::numberOfPixelDeadSensors);
    nSCTDeadSensor   = trkSummary->get(Trk::numberOfSCTDeadSensors);
    npixel           = trkSummary->get(Trk::numberOfPixelHits);
    npixholes        = trkSummary->get(Trk::numberOfPixelHoles);
    nsctholes        = trkSummary->get(Trk::numberOfSCTHoles);
  }
  // set nDeadSensors to 0 in case trkSummary wasn't called with HoleSearch
  // (i.e. number of deadSensors not available)
  if (nPixelDeadSensor == -1) nPixelDeadSensor = 0;
  if (nSCTDeadSensor   == -1) nSCTDeadSensor   = 0;
  ATH_MSG_VERBOSE ("---> Number of dead si sensors: " << nPixelDeadSensor + nSCTDeadSensor);

  // possible classification of TSOS
  enum TsosTypes {
    // A measurement not yet used in any other track
    UnusedHit   = 1,
    // A measurement shared with another track
    SharedHit   = 2, 
    // A hit that needs to be removed from the track
    RejectedHit = 3,
    // an outlier, to be copied in case
    Outlier     = 4,
    // other TSOS types to be copied in case
    OtherTsos   = 5
  };
  // create an array of types for each TSOS
  std::vector<int> tsosType;
  tsosType.resize(tsos->size());

  // loop over TSOS
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
  for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos, ++index) {

    // init array
    tsosType[index] = OtherTsos;

    // get measurment from TSOS
    const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();

    // if we do not have a measurement, we should just mark it
    if (!meas) {
      ATH_MSG_VERBOSE ("-> No measurement on TSOS, it is another type, to be copied over");
      tsosType[index] = OtherTsos;
      continue;
    }

    // ok, let try to get the ROT then
    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
    if (!rot) {
      // could be a Pseudo-Measurement ?
      const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast <const Trk::PseudoMeasurementOnTrack*> (meas);
      if (pseudo){
	ATH_MSG_VERBOSE ("-> Copy pseudo measurement");
	numPseudo++; // increase counter
      } else {
	ATH_MSG_WARNING ("-> Measurement is not a pseudo measurment, not yet supported, try to copy !");
      }
      tsosType[index] = OtherTsos;
      continue;
    }

    //
    // we have a TSOS with a measurement, keep analysing it
    //

    // let's get some information about the measurement
    const Identifier& id = rot->identify();
    bool isTRT           = m_detID->is_trt(id);
    bool isPixel         = m_detID->is_pixel(id);
    bool isBlayer        = isPixel ? m_detID->is_blayer(id) : false;
    bool isoutlier       = (*iTsos)->type(Trk::TrackStateOnSurface::Outlier);              

    // do we have some b-layer hit here (sorting of TSOS is inside out)
    if (isBlayer && (!isoutlier || ispatterntrack)) thishasblayer = true;  // we may reintegrate outliers from pattern

    // special cut to remove problematic combinations in the pixels
    if ( isPixel && !thishasblayer && npixholes>0           &&  // a pixel hit, no b-layer, we do have pixel holes
	 ( ( npixel==1 && !isoutlier ) ||                       // one pixel on track, it is not an additional outlier     
	   ( ispatterntrack && npixel==0 && isoutlier) )   ) {  // pattern track, no pixels, but an outlier (we may reintegrate it)
      ATH_MSG_VERBOSE ("-> Special case, problematic single pixel hit on track, reject it !");
      tsosType[index]    = RejectedHit;
      // mark track as bad !
      TrkCouldBeAccepted = false;
      continue;             
    }

    // do we have an outlier (an not a pattern track) ?
    // if ( (isoutlier && !ispatterntrack) || !(m_detID->is_indet(id)) ) {
    if ( (isoutlier && !ispatterntrack) || !(m_detID->is_indet(id)) ) {
      ATH_MSG_VERBOSE ("-> Prd is outlier on a fitter track (or not InDet), copy it over");
      tsosType[index] = Outlier;
      continue;
    }

    // let's check if this is a shared hit (even if it is an outlier on a pattern track) ?
    if (!m_assoTool->isUsed(*(rot->prepRawData()))) {
      if ( !isoutlier )
	ATH_MSG_VERBOSE ("-> Prd is unused, copy it over");
      else
	ATH_MSG_VERBOSE ("-> Prd is outlier on a pattern track and is unused, copy it over");

      tsosType[index] = UnusedHit;
      // increase some counters
      if (isTRT) 
	numTRT_Unused++;
      else
	numUnused++;
      // remember if first hit is shared, we need that later
      if (numShared == 0 ) firstisshared = false;
      // remember the last 2 ROTs
      lastbutonerot = lastrot;
      lastrot       = rot;
      lastrotindex  = index;

      continue;
    }

    //
    // ok, we have a shared hit
    //

    // do we have an outlier and a pattern track, but the hit is shared, so reject it (we reintegrate it otherwise)
    if ( isoutlier && ispatterntrack ) {
      ATH_MSG_VERBOSE ("-> Shared Prd is outlier on a pattern track, we do not want to reintegrate it, so reject it ");
      tsosType[index]    = RejectedHit;
      // mark track as bad !
      TrkCouldBeAccepted = false;
      continue;
    }

    // first check if it is a shared TRT hit, this is not allowed
    if (isTRT) {
      ATH_MSG_VERBOSE ("-> Share TRT hit, drop it !");
      tsosType[index]    = RejectedHit;
      // mark track as bad !
      TrkCouldBeAccepted = false;
      continue;
    }

    /* consider hit to be a shared one, if
       - not too many tracks share this hit already
       - the score of the track is high enough to allow for shared hits
    */		    
    Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = m_assoTool->onTracks(*(rot->prepRawData()));		    
    int                             numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    ATH_MSG_VERBOSE ("---> number of tracks with this share Prd: " << numberOfTracksWithThisPrd << " maxtracks: " << m_maxTracksPerPRD);

    // see if we try keeping it as a shared hit ? 
    if ( numberOfTracksWithThisPrd < m_maxTracksPerPRD  && // we do not allow to share with to many tracks
	 score > m_minScoreShareTracks                  && // score needs to be good
	 (!isPixel || npixholes<=0)                    ) { // we do not share pixels if there are holes in pixels

      // special treatment of share split pixel clusters...
      if (m_doPixelClusterSplitting && isPixel) {
	// get pixel cluster
	const InDet::PixelCluster* clus = dynamic_cast <const InDet::PixelCluster*> (rot->prepRawData());
	
	if ( !clus ) {
	  ATH_MSG_WARNING ("-----> cast to Pixel cluster failed, should not happen !");
	    // mark track as bad !
	    TrkCouldBeAccepted = false; // we have to remove at least one PRD
	    continue; 
	} else {
	  
	  // split clusters are not allowed to be shared at all, unless       
	  if ( clus->isSplit() )  {
	    ATH_MSG_VERBOSE ("-----> Pixel cluster is split, reject shared hit !!!");
	    tsosType[index]    = RejectedHit;
	    // mark track as bad !
	    TrkCouldBeAccepted = false; // we have to remove at least one PRD
	    continue; 
	  }
	  
	  // is cluster compatible with being a shared cluster ?
	  // A.S.: also a hack for the max size: allows large clusters that are exluded from the splitter to be shared
	  //       needs isExcluded() flag in the future
	  if (clus->splitProbability1() < m_sharedProbCut && clus->rdoList().size() <= size_t(m_maxSplitSize) ) {
	    ATH_MSG_VERBOSE ("-----> Pixel cluster is not compatible with being shared (splitProb = " 
			     << clus->splitProbability1() << ") , reject shared hit !!!");
	    tsosType[index]    = RejectedHit;
	    // mark track as bad !
	    TrkCouldBeAccepted = false; // we have to remove at least one PRD
	    continue; 
	  }
	}
      }

      ATH_MSG_VERBOSE ("---> Shared hit, but good track, let's enter hit in the list and try to keep it !");
      tsosType[index]   = SharedHit;
      numShared++;                             // increase counter 
      numWeightedShared += (isPixel ? 2 : 1);  // increase counter
	    
      // some additional bookkeeping, we need this later
      if (isPixel) {
	// ME: bugfix, either shared blayer or shared pixel
	if (isBlayer ) 
	  hassharedblayer = true;
	else
	  hassharedpixel  = true;
      }
      // remember the last 2 ROTs
      lastbutonerot = lastrot;
      lastrot       = rot;
      lastrotindex  = index;
      
      continue;
    }	
    
    // ok, we failed to keep the share hit, let's reject it.
    ATH_MSG_VERBOSE ("---> Share hit and bad track, reject it !");
    tsosType[index]    = RejectedHit;
    // mark track as bad !
    TrkCouldBeAccepted = false; // we have to remove at least one PRD
  }

  // total number of hits with dead modules
  int totalSiHits = numUnused + nPixelDeadSensor + nSCTDeadSensor;

  // special cut, do not allow the last hit to be to far away or after to many holes
  if (ispatterntrack                                                                              && // pattern track and
      totalSiHits > m_minHits                                                                     && // we have enough hits on the track
      (lastrot && lastbutonerot)                                                                  && // has enough ROTs
      (lastrot->globalPosition()-lastbutonerot->globalPosition()).mag()>1000*CLHEP::mm) { // distance cut
    ATH_MSG_DEBUG ("Special cut on distance, reject last hit on track !");
    tsosType[lastrotindex] = RejectedHit;
    numUnused--; // update counter
    // mark track as bad !
    TrkCouldBeAccepted     = false;
  }

  // special cut, do not allow the last hit to be after to many holes
  if (ispatterntrack                                                                              && // pattern track and
      totalSiHits > m_minHits                                                                     && // we have enough hits on the track
      nsctholes>3 ) { // too many holes cut
    ATH_MSG_DEBUG ("Special cut on too many holes, reject last hit on track !");
    tsosType[lastrotindex] = RejectedHit;
    numUnused--; // update counter
    // mark track as bad !
    TrkCouldBeAccepted     = false;
  }

  

  // get chi2/NDF, if track is fitted
  if ( !ispatterntrack ) {
    double trackchi2 = 0;
    if  (ptrTrack->fitQuality() && ptrTrack->fitQuality()->numberDoF()>0 ) 
      trackchi2 = ptrTrack->fitQuality()->chiSquared()/ptrTrack->fitQuality()->numberDoF();  
    
    // if we have share hits and this is a bad track, we reject it 
    if ( numShared > 0 && !ispatterntrack && trackchi2 > 3 ) {
      ATH_MSG_DEBUG ("Shared hits, we have a bad chi2 track, mark it as bad !");
      // mark track as bad !
      TrkCouldBeAccepted = false;
    }
  }


  // numTRT cut, special for back tracking
  if ( numTRT_Unused < nCutTRT) {
    ATH_MSG_DEBUG ("Track fails TRT hit cut, mark it as bad !");
    // mark track as bad !
    TrkCouldBeAccepted = false;
  }

  //
  // now see what to do with the track
  //
  
  // best case, we like this track and it passes this complex if statement
  if ( TrkCouldBeAccepted                                                    && // we did not mark the track as bad before
       ( !hassharedblayer || npixholes<=1 )                                  && // if blayer, at most 1 pixel hole
       !hassharedpixel                                                       && // no shared pixel hits
       ( ( totalSiHits >= m_minHits      && numShared == 0 )                 || // no shared and enough hits OR
 	 ( totalSiHits >= m_minNotShared && numWeightedShared <= m_maxShared && // shared hits and enough unique hits and shared hits with good quality
	   ( totalSiHits + std::min(numShared,m_maxShared) ) >= m_minHits && score > m_minScoreShareTracks ) ) ) {
    ATH_MSG_DEBUG ("=> Suggest to keep track with "<<numShared<<" shared hits !");
    return ptrTrack; // keep track as it is

  // ok, failed that one, can we recover the track ?
  } else if ( numTRT_Unused >= nCutTRT           && // TRT track or no TRT at all (back tracking)
	      ( totalSiHits >= m_minHits         || // we have enough hits OR
		( totalSiHits >= m_minNotShared  && // shared hits and enough unique hits and shared hits with good quality
		  totalSiHits+std::min(numShared,m_maxShared) >= m_minHits && score > m_minScoreShareTracks ) ) ) {
    // catch, if this is cosmics, accept the incoming track
    if (m_cosmics) {
      ATH_MSG_DEBUG ("=> Cosmics, accept input track even with shared hits");
      return ptrTrack;
    }

    // if ( numShared == 0 && TrkCouldBeAccepted ) {
    //	ATH_MSG_DEBUG ("=> Nothing to recover from, drop track !");
    //	return 0;
    // }
    
    // Track is potentially ok, create a stripped down version from the unused hits and the allowed shared hits
    // join shared to unused hits, if requirements are met

    ATH_MSG_VERBOSE ("Trying to recover track, allow for some shared hits is possible.");
      
    // new TSOS vector
    std::vector<const Trk::TrackStateOnSurface*> newTSOS;
      
    // counter for the weighted number of added shared hits 
    int cntIns = 0;
    
    // loop over all TSOS (and types) and copy the good ones over
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
      
    for (int index = 0 ; iTsos != iTsosEnd ; ++iTsos,++index ) {
      
      // remove rejected hits
      if (tsosType[index] == RejectedHit) {
	ATH_MSG_VERBOSE ("-> Dropping rejected hit");
	
      } else if (tsosType[index] != SharedHit ) {
	ATH_MSG_VERBOSE ("-> Copy good TSOS");
	newTSOS.push_back(*iTsos);
	
      } else if (cntIns >= m_maxShared) {
	ATH_MSG_VERBOSE ("-> Too many share hits, dropping outer hit(s)");
	
      } else {
	ATH_MSG_VERBOSE ("-> Try to recover a shared hit");
	
	// get measurment from TSOS
	const Trk::MeasurementBase* meas = (*iTsos)->measurementOnTrack();
	// get ROT from this TSOS
	const Trk::RIO_OnTrack*      rot = dynamic_cast <const Trk::RIO_OnTrack*> (meas);
	
	if (!rot) {
	  ATH_MSG_WARNING ("Cast to RIO_OnTrack failed, should never happen !");
	  continue;
	}

	// is it a pixel cluster ?
	bool isPixel = m_detID->is_pixel(rot->identify());
	
	// find out how many tracks use this hit already
	Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange range = m_assoTool->onTracks(*(rot->prepRawData()));		    
	int                             numberOfTracksWithThisPrd = std::distance(range.first,range.second);
	ATH_MSG_VERBOSE ("---> number of tracks with this shared Prd: " << numberOfTracksWithThisPrd << " maxtracks: " << m_maxTracksPerPRD);
	
	// check if this newly shared hit would exceed the shared hits limit of the already accepted track (**) 
	int  iShared        = 0;
	int  othernpixel    = 0;
	bool otherhasblayer = false;
	if ( numberOfTracksWithThisPrd == 1 ) {
	  std::vector< const Trk::PrepRawData* > prdsToCheck = m_assoTool->getPrdsOnTrack(*(range.first->second));
	  for ( std::vector< const Trk::PrepRawData* >::const_iterator it = prdsToCheck.begin() ; it != prdsToCheck.end(); ++it) {
	    if (m_assoTool->isShared(**it))
	      ++iShared;
	    if (m_detID->is_pixel((**it).identify())) {
	      othernpixel++;
	      if (m_detID->is_blayer((**it).identify()) ) otherhasblayer=true;
	    }
	  }
	}
        
	// now decide what to do, can we keep the shared hit
	if ( numberOfTracksWithThisPrd < m_maxTracksPerPRD            && // number of tracks sharing hit 
	     score > m_minScoreShareTracks                            && // score cut
	     (!isPixel || !hassharedblayer || npixholes <= 0)         && // shared b-layer only if no pixel holes
	     ( iShared < m_maxShared || (isPixel && !firstisshared) ) && // do not allow other accepted track to exceed the shared limit, if first pixel hit is shared
	     (!isPixel || thishasblayer == otherhasblayer )           && // only allow shared pixel if both have blayer or both not
	     (!isPixel || npixel >= othernpixel )                    ) { // only allow shared pixel if new track as at least as many pixel hits  
	  
	  ATH_MSG_VERBOSE ("---> Accepted hit shared with " << numberOfTracksWithThisPrd << " tracks !");
	  newTSOS.push_back(*iTsos);
	  numUnused++; // update counter 
	    
	  // update shared hit counter
	  cntIns += (isPixel ? 2 : 1);

	} else
	  ATH_MSG_VERBOSE ("---> Reject hit shared with " << numberOfTracksWithThisPrd << " tracks !");
      }
    }

    // this still may happen per (**) see above.
    if ( numUnused+nPixelDeadSensor+nSCTDeadSensor < m_minHits || newTSOS.size() <= 3 ) {
      ATH_MSG_DEBUG ("=> Too few hits, reject track with shared hits");
      return 0;
    }

    // check that this is not the input track
    if ( newTSOS.size() == tsos->size() ) {
      ATH_MSG_DEBUG ("=> Recovered input track, accept it !");
      return ptrTrack;
    } else {
      // ok, done, create subtrack
      const Trk::Track* newTrack = createSubTrack(newTSOS,ptrTrack);
      if (!newTrack) {
	ATH_MSG_DEBUG ("=> Failed to create subtrack");
	return 0;
      }
	
      Trk::TrackInfo info;
      info.addPatternRecoAndProperties(ptrTrack->info());
      Trk::TrackInfo newInfo;
      newInfo.setPatternRecognitionInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool);
      info.addPatternReco(newInfo);
      newTrack->info().addPatternReco(ptrTrack->info()); 
      
      ATH_MSG_DEBUG ("=> Successfully created subtrack with shared hits recovered !");
      return newTrack;
    }
  } else 
    ATH_MSG_DEBUG ("=> Track is recommended to be dropped");
  
  return 0;
}

//==========================================================================================

const Trk::Track* InDet::InDetAmbiTrackSelectionTool::createSubTrack( const std::vector<const Trk::TrackStateOnSurface*>& tsos, const Trk::Track* track ) const
{
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsosit=tsos.begin();
  int nmeas=0;
  for (;tsosit!=tsos.end();tsosit++){
    if ((**tsosit).type(Trk::TrackStateOnSurface::Measurement)) nmeas++;
  }
  if (nmeas<3) {
    ATH_MSG_DEBUG ("Less than 3 measurements, reject track !");
    return 0;
  }

  DataVector<const Trk::TrackStateOnSurface>* vecTsos = new DataVector<const Trk::TrackStateOnSurface>();

  // loop over TSOS, copy TSOS and push into vector
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos    = tsos.begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsosEnd = tsos.end(); 
  for ( ; iTsos != iTsosEnd ; ++iTsos) {
    const Trk::TrackStateOnSurface* newTsos = new Trk::TrackStateOnSurface(**iTsos);
    vecTsos->push_back(newTsos);
  }

  Trk::TrackInfo info;
  info.addPatternRecoAndProperties(track->info());
  Trk::TrackInfo newInfo;
  newInfo.setPatternRecognitionInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool);
  info.addPatternReco(newInfo);

  const Trk::Track* newTrack = new Trk::Track(info, vecTsos,0);
  
  return newTrack;

}

