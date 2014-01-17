/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DetectorPerformance/TRT_DetectorTiming.h"


#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/DataHandle.h"

#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
// #include "TrkTrackSummaryTool/ITrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"

#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "CommissionEvent/ComTime.h"

#include "TRT_DetectorPerformance/TRT_StrawMap.h"

#include "TTree.h"



/////////////////////////////////////////////////////////////////////////////

InDet::TRT_DetectorTiming::TRT_DetectorTiming(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_trtCondDbTool("TRT_CalDbSvc", name),
  m_trtStrawNeighbourTool("TRT_StrawNeighbourSvc", name),
  m_trtStrawMap("InDet::TRT_StrawMap"),
  m_histSvc("THistSvc", name)
{
  declareProperty("histogramService", m_histSvc);

  m_ntuple = 0;
}


bool MySortCriteria(InDet::TRT_DetectorTiming::hitParams* hp1, InDet::TRT_DetectorTiming::hitParams* hp2) {

  return (hp1->y > hp2->y);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet::TRT_DetectorTiming::initialize(){

  if (msgLvl(MSG::INFO)) msg()<< "initialize()" << endreq;

  m_numTrtOnlyTracks = 0;
  m_numTrtSctTracks = 0;
  
  //////////////////////////////////////////////
  // Create TTree and register it to THistSvc
  //////////////////////////////////////////////

  if ( m_histSvc.retrieve().isFailure() ){
    msg(MSG::FATAL) << "histogram service not found" << endreq;
    return StatusCode::FAILURE;
  } else 
    if (msgLvl(MSG::INFO)) msg() << "retrieved histogramService" << endreq;

  m_ntuple = new TTree("ntuple" , "ntuple");
  
  std::string fullTreeName_ntuple =  "/AANT/ntuple" ;
  if((m_histSvc->regTree(fullTreeName_ntuple, m_ntuple)).isFailure())
    {
      msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeName_ntuple << endreq;
      return StatusCode::FAILURE;
    }

  //////////////////////////////////////////////////////
  /// Book variables to be added to the output ntuple
  //////////////////////////////////////////////////////
  m_ntuple->Branch("corLE"           , &m_correctedLeadingEdge, "corLE/D");  
  m_ntuple->Branch("EP"              , &m_timeCor             , "EP/D"); 
  m_ntuple->Branch("TOT"             , &m_timeOverThreshold   , "TOT/D"); 
  m_ntuple->Branch("TEbin"           , &m_trailingEdgeBin     , "TEbin/I"); 
  m_ntuple->Branch("LEbin"           , &m_leadingEdgeBin      , "LEbin/I"); 
  m_ntuple->Branch("wrongSide"       , &m_wrongSide           , "wrongSide/I");  // only valid for barrel tracks
  m_ntuple->Branch("board"           , &m_board		      , "board/I"); 
  m_ntuple->Branch("locR"            , &m_locR 		      , "locR/D"); 
  m_ntuple->Branch("timeCorChip"     , &m_timeCorChip         , "timeCorChip/D"); // currently not implemented -- always 0
  m_ntuple->Branch("chip"	     , &m_chip                , "chip/I"); 
  m_ntuple->Branch("side"	     , &m_side                , "side/I"); 
  m_ntuple->Branch("t0"              , &m_t0                  , "t0/D"); 
  m_ntuple->Branch("phi"             , &m_phi                 , "phi/I"); 
  m_ntuple->Branch("straw"           , &m_straw               , "straw/I"); 
  m_ntuple->Branch("strawLayer"      , &m_strawLayer          , "strawLayer/I"); 
  m_ntuple->Branch("layer"           , &m_layer               , "layer/I"); 
  m_ntuple->Branch("nBarHits"        , &m_nTrtBarHits         , "nBarHits/I"); 
  m_ntuple->Branch("nEcHits"         , &m_nTrtEcHits          , "nEcHits/I"); 
  m_ntuple->Branch("nTRThits_barrelA", &m_nTrtHitsBarrelA     , "nTRThits_barrelA/I"); 
  m_ntuple->Branch("nTRThits_barrelC", &m_nTrtHitsBarrelC     , "nTRThits_barrelC/I"); 
  m_ntuple->Branch("nTRThits_ecA"    , &m_nTrtHitsEcA 	      , "nTRThits_ecA/I"); 
  m_ntuple->Branch("nTRThits_ecC"    , &m_nTrtHitsEcC 	      , "nTRThits_ecC/I"); 
  m_ntuple->Branch("nSCThits"        , &m_nSctHits    	      , "nSCThits/I"); 
  m_ntuple->Branch("nPixHits"        , &m_nPixHits    	      , "nPixHits/I"); 
  m_ntuple->Branch("theta"           , &m_theta       	      , "theta/D"); 
  m_ntuple->Branch("eta"             , &m_eta         	      , "eta/D"); 
  m_ntuple->Branch("chi2ndf"         , &m_chi2Ndf     	      , "chi2ndf/D"); 
  m_ntuple->Branch("phi0"            , &m_phi0   	      , "phi0/D"); 
  m_ntuple->Branch("z0"	       	     , &m_z0     	      , "z0/D"); 
  m_ntuple->Branch("d0"	       	     , &m_d0 	              , "d0/D"); 
  m_ntuple->Branch("tubeHit"         , &m_tubeHit	      , "tubeHit/I"); 
  m_ntuple->Branch("locRpat"         , &m_locRPat	      , "locRpat/D"); 

  if (msgLvl(MSG::INFO)) msg()<< "end initialize()" << endreq;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode InDet::TRT_DetectorTiming::execute() {

  if (msgLvl(MSG::DEBUG)) msg() << "execute()" << endreq;

  // TRT helper
  const TRT_ID * m_trtHelper;
  if (detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  const DataVector<Trk::Track> *trkCollection = 0;
  StatusCode sc = evtStore()->retrieve( trkCollection, "Tracks" );
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not find Tracks Collection" << endreq;
    return sc;  
  }

  if ( trkCollection == 0 ) {
    msg(MSG::ERROR) << "trkCollection == 0 " << endreq;
    assert( trkCollection );
    return StatusCode::FAILURE;
  }

  // get the TRT_Phase -- also called the Event Phase
  ComTime* theComTime;
  sc = evtStore()->retrieve(theComTime, "TRT_Phase");
  if(sc.isFailure())
    {
      msg(MSG::ERROR) << "ComTime object not found with name TRT_Phase !!!" << endreq;
    }
  
  if(StatusCode::SUCCESS!=m_trtCondDbTool.retrieve() ) {
    msg(MSG::ERROR) <<"Could not get TRTCalDbTool !"<<endreq;
    return StatusCode::FAILURE;
  } 

  sc = m_trtStrawNeighbourTool.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not find TRTStrawNeighbourTool " << endreq;
    return sc;
  }


  // get the event phase - one phase for the entire event
  double timeCor=0.;
  if(theComTime)
    {
      timeCor = theComTime->getTime();
    }

  // watch out for sneaky infinity trying to set us up the bomb!
  if (timeCor < -100 || timeCor > 100) {
    timeCor = 0.;
  }

  const float tubeHitErr = 1.33333;
  
  
  // open a file to print
  static int firstCall = 0;
  FILE *myFile = fopen("myTRToutput.dat", (firstCall)?"a":"w"); // overwrite the old file only at the first call
  firstCall++; 

  int nTracks = trkCollection->size();
  int countTracks = 0;
  if (msgLvl(MSG::VERBOSE)) msg() << "starting DataVector<Trk::Track> loop over nTracks = " << nTracks << endreq;
  // loop over all tracks 
  for ( DataVector<Trk::Track>::const_iterator trackIt = trkCollection->begin(); trackIt != trkCollection->end(); trackIt++ ) {

    assert( countTracks < nTracks ); countTracks++; 
    
    // want to record all these parameters to look for poor tracks
    int   nTrtBarHits = 0;
    int   nTrtEcHits = 0;
    int   nTrtHitsBarrelA = 0;
    int   nTrtHitsBarrelC = 0;
    int   nTrtHitsEcA = 0;
    int   nTrtHitsEcC = 0;
    int   nSctHits = 0;
    int   nPixHits = 0;
    float theta = 0;
    float eta = 0;
    float chi2Ndf = 0;
    float phi0 = 0;
    float d0 = 0;
    float z0 = 0;
    
    std::vector<InDet::TRT_DetectorTiming::hitParams*> hitParamsVec;

    // use perigee parameters to get theta/eta info
    theta = (**trackIt).perigeeParameters()->parameters()[Trk::theta];
    phi0 = (**trackIt).perigeeParameters()->parameters()[Trk::phi0];
    d0 = (**trackIt).perigeeParameters()->parameters()[Trk::d0];
    z0 = (**trackIt).perigeeParameters()->parameters()[Trk::z0];
    eta = (**trackIt).perigeeParameters()->eta();
    
    // use fitQuality to find chi2/ndf for track
    float chi2 = (**trackIt).fitQuality()->chiSquared();
    float ndf = (**trackIt).fitQuality()->numberDoF();
    chi2Ndf = chi2/ndf;

        
    if (msgLvl(MSG::VERBOSE)) msg() << "start DataVector<const Trk::TrackStateOnSurface>* trackStates " << endreq;
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = (**trackIt).trackStateOnSurfaces();   
    if ( trackStates == 0 ) {
      if (msgLvl(MSG::WARNING)) msg() << "Trk::TrackStateOnSurface empty" << endreq;
      continue;
    } 

    if (msgLvl(MSG::VERBOSE)) msg() << "start DataVector<const Trk::TrackStateOnSurface>* trackStatesIt " << trackStates->size() << endreq;
    unsigned int countTrackStates(0), countTrackSatesMeasurements(0); 

    
    // loop over all "trackStates", in principle one state is associated to each measurement + one more trackState for track parameters at the Interaction Point
    for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); trackStatesIt != trackStates->end(); trackStatesIt++ ) {
      
      if (msgLvl(MSG::VERBOSE)) msg() << "countTrackStates " << countTrackStates << endreq;
      assert( countTrackStates < trackStates->size() ); countTrackStates++; 
      assert( (*trackStatesIt) != 0 );
      if ( *trackStatesIt == 0 ) { if (msgLvl(MSG::WARNING)) msg() << "*trackStatesIt == 0" << endreq; }

      //CLHEP::Hep3Vector pos(0., 0., 0.);
      //CLHEP::Hep3Vector mom(0., 0., 0.);
      Amg::Vector3D pos(0., 0., 0.);
      Amg::Vector3D mom(0., 0., 0.);

      const Trk::TrackParameters* trackParam = (*trackStatesIt)->trackParameters();
      if ( trackParam != 0 ) {
	  
        pos = trackParam->position();
        mom = trackParam->momentum();
		
      } else { 
	  
	if (msgLvl(MSG::WARNING)) msg() << "(*trackStatesIt)->trackParameters() == NULL" << endreq; 
      }



      if ( (*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement) ) {
      
        // try to dynamic cast as SCT space points etc to count the number of silicon hits
        if (dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())!=0) {
          nSctHits++;
          continue;
        }
                
        if (dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())!=0) {
          nPixHits++;
          continue;
        }
        
        if (msgLvl(MSG::VERBOSE)) msg() << "drift Circle " << endreq;
        const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );
        if ( driftCircleOnTrack == 0 ) { if (msgLvl(MSG::WARNING)) msg() << "driftCircleOnTrack == 0" << endreq; continue; }
	//HepGeom::Point3D<double> hit = driftCircleOnTrack->globalPosition();
	const Amg::Vector3D hit = driftCircleOnTrack->globalPosition();
        if (msgLvl(MSG::VERBOSE)) msg() << "driftCircleOnTrack global position " << hit << endreq;
	
        const InDet::TRT_DriftCircle *driftCircle = dynamic_cast<const InDet::TRT_DriftCircle*> (driftCircleOnTrack->prepRawData());
        if ( driftCircle == 0 ) { if (msgLvl(MSG::WARNING)) msg() << "driftCircle == 0" << endreq; continue; }
        if (msgLvl(MSG::VERBOSE)) msg() << "track measurement trail. edge " << driftCircle->trailingEdge() << " " 
	       << ((int) driftCircle->driftTimeValid()) 
	       << " Base valid " << ( (driftCircleOnTrack->detectorElement() == 0) ? 0 : 1 ) << endreq;  
	    
	    
	// this flag checks to see if the t0-corrected drift time is in the range -10ns < t < 75ns (I think...) 
        //  !!! this causes seg faults on specific events !!!   
	if (!driftCircle->driftTimeValid()) continue;
        
        Identifier elID = driftCircle->identify();
	m_trtStrawMap->setNewIdentifier(elID);

        int side       	  = m_trtHelper->barrel_ec(elID);  // -1 for side C, 1 for side A
        int layer      	  = m_trtHelper->layer_or_wheel(elID);
        int phi        	  = m_trtHelper->phi_module(elID);
        int strawLayer 	  = m_trtHelper->straw_layer(elID);
	int myStrawNumber = m_trtStrawMap->straw();
        int chip; m_trtStrawNeighbourTool->getChip(elID, chip);
	int myBoard       = m_trtStrawMap->board();

	
	if(abs(side) == 1)    // barrel
	  {
	    // set chip number to correct scale 
	    chip-=1; 
	    if(layer == 1)      chip+=21;
	    else if(layer == 2) chip+=54;
	  } 
	else // endcap
	  {
	    // set all chips on a scale from 0-11
	    if(chip > 11) chip-=20;
	    chip+=(104+12*myBoard);
	  }
      
	if (myStrawNumber < 0 || myStrawNumber>=5482) { if (msgLvl(MSG::WARNING)) msg() << MSG::WARNING << "invalid straw number !!! " << myStrawNumber << endreq; continue; }
      
	// use the identifier to look up the t0
	double t0 = m_trtCondDbTool->getT0(elID, TRTCond::ExpandedIdentifier::STRAW);     
      
        double locR(999.);
        //const Trk::MeasuredAtaStraightLine *trackMeasuredAtLine = dynamic_cast<const Trk::MeasuredAtaStraightLine*>( trackParam );
        //if ( trackMeasuredAtLine ) locR = trackMeasuredAtLine->parameters()[Trk::locR];   // point of track's closest approach, I believe
	locR 	= trackParam->parameters()[Trk::locR]; 


        
        // determine if we just have a tube hit:
        //   these have locR == 0
        //   error on locR == 4/sqrt(12)
        const Trk::MeasurementBase* mesb = (*trackStatesIt)->measurementOnTrack();
        if (mesb == 0) {
          if (msgLvl(MSG::WARNING)) msg() << "measurementBase == 0, and we should have had it!!!" << endreq;
          continue;
        }
        float locRPat = mesb->localParameters()[Trk::locR];
        //float errLocR = mesb->localErrorMatrix().covValue(Trk::locR);
        float errLocR = mesb->localCovariance()(Trk::locX,Trk::locX) ;
        int tubeHit = 0;
        if ((fabs(locRPat - 0.) < 0.001) && (fabs(errLocR - tubeHitErr) < 0.01)) {
          tubeHit = 1;
        }


        if (abs(side) == 1) {
          nTrtBarHits++;
        } else if (abs(side) == 2) {
          nTrtEcHits++;
        }
	if(side == 1) ++nTrtHitsBarrelA;
	else if(side == -1) ++nTrtHitsBarrelC;
	else if(side == 2) ++nTrtHitsEcA;
	else if(side == -2) ++nTrtHitsEcC;

        InDet::TRT_DetectorTiming::hitParams* hps = new InDet::TRT_DetectorTiming::hitParams;

        // fill hitParams object with values
	hps->leadingEdgeBin = driftCircle->driftTimeBin();
	hps->trailingEdgeBin = 	driftCircle->trailingEdge();
	hps->leadingEdge = driftCircle->rawDriftTime();
	hps->timeOverThreshold = driftCircle->timeOverThreshold();
	hps->side = side;
	hps->wrongSide = 0;
        hps->correctedLeadingEdge = driftCircle->rawDriftTime() - timeCor;
        hps->x = driftCircleOnTrack->globalPosition().x();
        hps->y = driftCircleOnTrack->globalPosition().y();
	hps->board = myBoard;
	hps->locR = locR;
	hps->localPos = driftCircle->localPosition()[Trk::driftRadius];
        hps->timeCorChip = 0.; // currently not implemented
        hps->chip = chip;
        hps->t0 = t0;
	hps->phi = phi;
        hps->straw = myStrawNumber;
        hps->strawLayer = strawLayer;
        hps->layer = layer;
        hps->tubeHit = tubeHit;
        hps->locRPat = locRPat;
	
	// add hit params object to vector
	hitParamsVec.push_back(hps);

        countTrackSatesMeasurements++;

      } // end if "state==measurement"

    } // end loop over "trackStates"

    // sort by y to order the hits
    std::sort(hitParamsVec.begin(), hitParamsVec.end(), MySortCriteria);
    std::vector<InDet::TRT_DetectorTiming::hitParams*>::iterator hitParamsIt;
    
    // now flag hits that have flipped in z
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  this is currently only valid for the barrel !
    //   needs to be fixed to use this in the endcap !
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    // no need to check for vectors smaller than 2 entries
    //  more importantly, we get seg faults if we go in to this loop!!
    if (hitParamsVec.size() > 2) {
      int last = 0;
      int check = 0;
      int next = 0;
      for(hitParamsIt = hitParamsVec.begin(); hitParamsIt != hitParamsVec.end(); hitParamsIt++) {
        // special cases for first, last hit
        if (hitParamsIt == hitParamsVec.begin()) {
          last = (*(hitParamsIt+2))->side;
        }
        if (hitParamsIt == (hitParamsVec.end()-1)) {
          next = (*(hitParamsIt-1))->side;
        } else {
          next = (*(hitParamsIt+1))->side;
        }
        
        check = (*(hitParamsIt))->side;
      
        // if the hits before and after this one are on the same side
        //  but different from the current, flag this one
        if ((last == next) && (check != next)) {
          (*(hitParamsIt))->wrongSide = 1;
        }
      
        last = (*(hitParamsIt))->side;       
      }
    } // check on empty vector
    
    
    // loop back through hits vector and print output to a file
    for(hitParamsIt = hitParamsVec.begin(); hitParamsIt != hitParamsVec.end(); hitParamsIt++) {
      
      m_correctedLeadingEdge = (*hitParamsIt)->correctedLeadingEdge; 
      m_timeCor              = timeCor;
      m_timeOverThreshold    = (*hitParamsIt)->timeOverThreshold;
      m_trailingEdgeBin      = (*hitParamsIt)->trailingEdgeBin;
      m_leadingEdgeBin       = (*hitParamsIt)->leadingEdgeBin;
      m_wrongSide	     = (*hitParamsIt)->wrongSide;
      m_board		     = (*hitParamsIt)->board;
      m_locR 		     = (*hitParamsIt)->locR;
      m_timeCorChip	     = (*hitParamsIt)->timeCorChip;
      m_chip		     = (*hitParamsIt)->chip;
      m_side 		     = (*hitParamsIt)->side;
      m_t0		     = (*hitParamsIt)->t0;
      m_phi		     = (*hitParamsIt)->phi;
      m_straw		     = (*hitParamsIt)->straw;
      m_strawLayer	     = (*hitParamsIt)->strawLayer;
      m_layer		     = (*hitParamsIt)->layer;
      m_nTrtBarHits	     = nTrtBarHits;
      m_nTrtEcHits	     = nTrtEcHits;
      m_nTrtHitsBarrelA      = nTrtHitsBarrelA;
      m_nTrtHitsBarrelC      = nTrtHitsBarrelC;
      m_nTrtHitsEcA	     = nTrtHitsEcA;
      m_nTrtHitsEcC	     = nTrtHitsEcC;
      m_nSctHits 	     = nSctHits;
      m_nPixHits 	     = nPixHits;
      m_theta		     = theta;
      m_eta		     = eta;
      m_chi2Ndf	             = chi2Ndf;
      m_phi0		     = phi0;
      m_z0		     = z0;
      m_d0		     = d0;
      m_tubeHit	             = (*hitParamsIt)->tubeHit;
      m_locRPat              = (*hitParamsIt)->locRPat;

      m_ntuple->Fill();
    }
    
    for (unsigned int i=0; i<hitParamsVec.size(); i++) { delete hitParamsVec[i]; hitParamsVec[i] = 0; } // need to first delete the members that were created with new!!    
    hitParamsVec.clear();
    
    if (nSctHits == 0) m_numTrtOnlyTracks++;
    else m_numTrtSctTracks++;
    
  } // end loop over tracks

  fclose(myFile);

  if (msgLvl(MSG::DEBUG)) msg() << "end execute()" << endreq;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode InDet::TRT_DetectorTiming::finalize() {

  if (msgLvl(MSG::INFO)) msg() << "finalize()" << endreq;
  if (msgLvl(MSG::INFO)) msg() << "saw " << m_numTrtOnlyTracks << " TRT only tracks and " 
                               << m_numTrtSctTracks << " combined tracks with the SCT" << endreq;
  return StatusCode::SUCCESS;
}

