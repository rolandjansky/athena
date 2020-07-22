/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/SystemOfUnits.h"
//
#include "EFHadCalibHypo.h"
//
#include "Particle/TrackParticle.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFHadCalibHypo::EFHadCalibHypo(const std::string& name, 
			       ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_toVertex("Reco::TrackToVertex")
{

  // cuts on quality of high pt tracks (HT)
  declareProperty("CutOnPtHT"             , m_PtMinCut   = 0.0*Gaudi::Units::GeV );
  declareProperty("CutOnEtaHT"            , m_maxEta     = 10.0 ); 
  declareProperty("CutminSCTHitsHT"       , m_minSCTHits = -1 );
  declareProperty("CutminPixHitsHT"       , m_minPixHits = -1   );
  declareProperty("CutmaxD0HT"            , m_maxD0      = 9999.9*Gaudi::Units::mm );
  declareProperty("CutmaxZ0HT"            , m_maxZ0      = 9999.9*Gaudi::Units::mm );

  declareProperty("CutOnIsolationDeltaR"  , m_trackIsoDR         = 0.0 );  
  declareProperty("CutnTracksOnDeltaR"    , m_nMaxTracksOnDeltaR = 99);  
  declareProperty("MaxTrackPtInIso"       , m_maxPtInIso         = 99);  

  declareMonitoredVariable("CutCounter"   , m_cutCounter  );
  declareMonitoredVariable("nTracksInIso" , m_nTracksInIso);
  declareMonitoredVariable("HTpT"         , m_HTpT        );
  declareMonitoredVariable("HTeta"        , m_HTeta       );
  declareMonitoredVariable("HTphi"        , m_HTphi       );
  declareMonitoredVariable("HTd0"         , m_HTd0        );
  declareMonitoredVariable("HTz0"         , m_HTz0        );
  declareMonitoredVariable("HTnPixHit"    , m_HTnPixHit   );
  declareMonitoredVariable("HTnSCTHit"    , m_HTnSCTHit   );

  m_cutCounter2 = 1;
  m_DQtrackPass = true;

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFHadCalibHypo::~EFHadCalibHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode EFHadCalibHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: EFHadCalibHypo will cut on "                   << endmsg;
  msg() << MSG::INFO << " REGTEST: CutForHighPtTrack "   << m_PtMinCut            << endmsg; 
  msg() << MSG::INFO << " REGTEST: CutOnIsolationDeltaR "<< m_trackIsoDR          << endmsg; 
  msg() << MSG::INFO << " REGTEST: CutnTracksOnDeltaR "  << m_nMaxTracksOnDeltaR  << endmsg; 
  msg() << MSG::INFO << " REGTEST: MaxTrackPtInIso "     << m_maxPtInIso          << endmsg; 
  msg() << MSG::INFO << " REGTEST: minPixHits "          << m_minPixHits          << endmsg; 
  msg() << MSG::INFO << " REGTEST: minSCTHits "          << m_minSCTHits          << endmsg; 
  msg() << MSG::INFO << " REGTEST: maxD0 "               << m_maxD0               << endmsg; 
  msg() << MSG::INFO << " REGTEST: maxZ0 "               << m_maxZ0               << endmsg; 
  msg() << MSG::INFO << " REGTEST: maxEta "              << m_maxEta              << endmsg; 
  msg() << MSG::INFO << " REGTEST: ------ "                                       << endmsg;

  //Retrieve the tool for track extrapolate to the calorimeter
  //if (m_toCalo.retrieve().isFailure()) 
  //  {
  //    msg() << MSG::FATAL << "Could not retrieve m_toCalo " << m_toCalo << endmsg;
  //    return HLT::BAD_JOB_SETUP;
  //  }

  msg() << MSG::INFO << "Initialization of EFHadCalibHypo completed successfully" << endmsg;
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode EFHadCalibHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode EFHadCalibHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  
    msg() << MSG::DEBUG << "REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  pass = false;

  m_cutCounter=0;
  m_HTpT=-9999999;
  m_nTracksInIso=-1;
  m_HTeta=-99.;
  m_HTphi=-99.;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(inputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return status;
  }

  if( msgLvl() <= MSG::DEBUG )
    {
      msg() << MSG::DEBUG << "+++++++++++++++++++ mdacunha: starting new event +++++++++++++++++++" << endmsg;
    }

  //////////////////////////////////////////////////////////
  ///////////// Isolation implementation ///////////////////
  //////////////////////////////////////////////////////////

  // Step 1: Retrieve the TrackParticleContainer.
  
  std::vector<const Rec::TrackParticleContainer*> TPContainerVector;
  
  status = getFeatures(inputTE, TPContainerVector);
  if (status != HLT::OK)
    {
      if( msgLvl() <= MSG::INFO )
	msg() << MSG::INFO
	      << " REGTEST: Failed to get TrackParticleContainer's from the trigger element" 
	      << endmsg;
      return HLT::OK;
    } 
  if(TPContainerVector.size() == 0){
    if( msgLvl() <= MSG::INFO )
      msg() << MSG::INFO << " REGTEST: Received 0 TrackParticleContainers  "
            << endmsg;
    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "TrackParticleContainer successfully retrieved" << endmsg;
    msg() << MSG::DEBUG << "TTPContainerVector.size = " << TPContainerVector.size() << endmsg;
  }

  // TrackParticleContainer: Container with all Track Particles inside the ROI (ex. 2x0.3 x 2x0.3)
  const Rec::TrackParticleContainer* TPContainer = TPContainerVector.back();
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "TrackParticleContainer size: " << TPContainer->size() << endmsg;

  if(TPContainer->size() == 0){
    if( msgLvl() <= MSG::INFO )
      msg() << MSG::INFO << " REGTEST: Empty TrackParticleContainer. Returning "
            << endmsg;
    return HLT::OK;
  }

  Trk::Vertex beamposition(Amg::Vector3D(0,0,0));
  //const VxContainer*& pointerToEFPrmVtxCollections = 0;

  std::vector<const VxContainer*> vectorOfEFPrmVtxCollections;
  //HLT::ErrorCode 
  status = getFeatures(inputTE, vectorOfEFPrmVtxCollections, ""); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get VxContainer from the trigger element" << endmsg;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFPrmVtxCollections.size() << " VxContainer" << endmsg;
    
  std::vector<const VxContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const VxContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();
    
  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 

    if ((*pPrmVtxColl)->size() != 0) {
      if ((*pPrmVtxColl)->front()->vertexType() == 1) {
	if (msgLvl() >= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "Selected collection with Primary Vertex label" 
		<< (*pPrmVtxColl)->front()->vertexType() << endmsg;
	break;
      }
    }
  }
    
  if (pPrmVtxColl == lastPrmVtxColl) {
    //pointerToEFPrmVtxCollections = 0;
    return HLT::ERROR;
  } else {
    //pointerToEFPrmVtxCollections = *pPrmVtxColl;
    //return HLT::OK;
  }

  const VxContainer *Vxcontainer = *pPrmVtxColl;
  beamposition=(*(Vxcontainer->begin()))->recVertex();
  msg() << MSG::DEBUG << "PV position " << beamposition.position() << endmsg;

  m_cutCounter++; // all initialized
    
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  for (Rec::TrackParticleContainer::const_iterator trackIter = TPContainer->begin();
       trackIter != TPContainer->end(); ++trackIter)
    {

      m_cutCounter2 = 1;
 
      double TrackPt = fabs( (*trackIter)->pt());

      if(TrackPt < m_PtMinCut)
	{
	  m_cutCounter = ((m_cutCounter2 > m_cutCounter) ? m_cutCounter2:m_cutCounter);
	  continue;
	}

      m_cutCounter2++; // at least one track with pt threshold

      if(m_HTpT < TrackPt)
	m_HTpT = TrackPt;
            
      hltTrackQualityCheck(trackIter,beamposition);
	
      if (!m_DQtrackPass)
	{
	  m_cutCounter = ((m_cutCounter2 > m_cutCounter) ? m_cutCounter2:m_cutCounter);
	  continue;
	}

      // Step 2: Propagate tracks to the Calorimeter.
      //         Count tracks in the isolation region with pT above cut.
  
      //const double GAP_ETA = 1.5; // Gap between EMB and EMEC
  
      double extrapolatedEta = -50.;
      double extrapolatedPhi = -50.;
      int countTracksOnDeltaR =0;

      for (Rec::TrackParticleContainer::const_iterator trackIter2 = TPContainer->begin();
	   trackIter2 != TPContainer->end(); ++trackIter2)
	{
      
	  // Ensure that the high pt track is not compared to itself
	  if (trackIter2 == trackIter) continue;

	  // Count only tracks with a certain pT:
	  if( fabs( (*trackIter2)->pt()) < m_maxPtInIso ) continue;

	  const Trk::TrackParameters * param_at_calo = 0;

	  if( msgLvl() <= MSG::VERBOSE ){ 
	    msg() << MSG::VERBOSE << "Extrapolating track to calo." << endmsg;
	    msg() << MSG::VERBOSE << "Track eta = " << (*trackIter2)->eta() << ", "
		  << "phi = " << (*trackIter2)->phi() << ", "
		  << "pt = "  << (*trackIter2)->pt()
		  << endmsg;
	  }

	  /*
	  if (fabs( (*trackIter2)->eta()) < GAP_ETA)
	    {
	      param_at_calo = m_toCalo->extrapolate(( Trk::TrackParticleBase) *(*trackIter2),
						    CaloCell_ID::EMB2,
						    0.0,
						    Trk::alongMomentum,
						    Trk::undefined);
	    }
	  else
	    {
	      param_at_calo = m_toCalo->extrapolate(( Trk::TrackParticleBase) *(*trackIter2),
						    CaloCell_ID::EME2,
						    0.0,
						    Trk::alongMomentum,
						    Trk::undefined);
	    }
	  */

// commented lines because of coverity issue: param_at_calo is always false, so 'else' dead code (D.Zanzi,4/4/16)	  
//	  if (!param_at_calo) 
//	    {
	      //use non-extrapolated track direction
	      extrapolatedEta = (*trackIter2)->eta();
	      extrapolatedPhi = (*trackIter2)->phi();

	      if( msgLvl() <= MSG::WARNING )
		msg() << MSG::WARNING << "Problem extrapolating track to calo for track with" 
		      << "eta = " << (*trackIter2)->eta() << " and "
		      << "phi = " << (*trackIter2)->phi()
		      << ". Using non-extrapolated track direction! "
		      << endmsg;
//	    }

//	  else 
//	    {
//	      extrapolatedEta = param_at_calo->position().eta();
//	      extrapolatedPhi = param_at_calo->position().phi();
//	    }

	  double deta = fabs(extrapolatedEta - (*trackIter)->eta());
	  double dphi = fabs(extrapolatedPhi - (*trackIter)->phi());
	  double dr = 1000;
	
	  if (dphi > M_PI)	dphi = 2 * M_PI - dphi;
	
	  dr = sqrt(dphi*dphi+deta*deta);
	
	  if (dr < m_trackIsoDR)
	    {
	      countTracksOnDeltaR++;
	    }

	  if( msgLvl() <= MSG::VERBOSE )
	    msg() << MSG::VERBOSE << "Extrapolated "
		  << "eta = " << param_at_calo->position().eta() << " and "
		  << "phi = " << param_at_calo->position().phi() 
		  << ", dR = " << dr << endmsg;

	  delete param_at_calo;
	}

      if( msgLvl() <= MSG::INFO )
	msg() << MSG::INFO << "REGTEST: Number of tracks in iso region = " << countTracksOnDeltaR 
	      << endmsg;

      m_nTracksInIso = countTracksOnDeltaR;

      if(countTracksOnDeltaR > m_nMaxTracksOnDeltaR){
	if( msgLvl() <= MSG::DEBUG )
	  {
	    msg() << MSG::DEBUG << "REGTEST: The selected track is not isolated. skipping" << endmsg;
	    msg() << MSG::DEBUG << "REGTEST: checking next track (if any)" << endmsg;
	  }
	continue;
      }

      pass = true;
      if( msgLvl() <= MSG::INFO )
	msg() << MSG::INFO << " REGTEST: TE accepted !! " << endmsg;

      m_cutCounter++; // at least one track pass isolation criteria
      
      return HLT::OK;
      
    }

  if( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "REGTEST: TE rejected !!"
	  << endmsg;
  
  return HLT::OK;
}

HLT::ErrorCode EFHadCalibHypo::hltTrackQualityCheck(Rec::TrackParticleContainer::const_iterator trackIter,
						    Trk::Vertex beamposition)
{
  m_DQtrackPass = true;

  // extrapolate to PV
  const Trk::Perigee *mp(m_toVertex->perigeeAtVertex(*(*trackIter), beamposition.position()));
  if (!mp) {
    msg() << MSG::WARNING << "Extrapolation @ EF to vertex is failed. Skip this track." << endmsg;
    m_DQtrackPass = false;
    return HLT::OK;
  } 

  m_HTeta = (*trackIter)->eta() ;
  m_HTphi = (*trackIter)->phi() ;

  //const Trk::MeasuredPerigee *mp = (*trackIter) -> measuredPerigee();
  const Trk::TrackSummary *summary = (*trackIter) -> trackSummary();
   
  m_HTnPixHit =  summary->get(Trk::numberOfPixelHits);
  m_HTnSCTHit =  summary->get(Trk::numberOfSCTHits);
  m_HTz0 = mp->parameters() [Trk::z0] * sin(mp->parameters()[Trk::theta]);
  m_HTd0 = mp->parameters() [Trk::d0];

  if( msgLvl() <= MSG::DEBUG )
    {
      msg() << MSG::DEBUG << "++++++++++++ High pT track ++++++++++++" << endmsg;
      msg() << MSG::DEBUG
	    << "eta = " << m_HTeta << ", "
	    << "phi = " << m_HTphi << " and "
	    << "pT  = " << m_HTpT * 1e-3 << "GeV"
	    << endmsg;
      msg() << MSG::DEBUG 
	    << "d0  = " << m_HTd0  << ", "
	    << "z0  = " << m_HTz0  << " and "
	    << "p   = " << (*trackIter)->p  () * 1e-3 << "GeV"
	    << endmsg;
      msg() << MSG::DEBUG << "hits:"
	    << "pix = " << m_HTnPixHit << " "
	    << "sct = " << m_HTnSCTHit << " "
	    << endmsg;
      msg() << MSG::DEBUG << "+++++++++++++++++++++++++++++++++++++++" << endmsg;
      
    }
  if (m_HTnPixHit < m_minPixHits) 
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: This track Pixel Hits = " <<m_HTnPixHit << " "
              << "did not pass the quality threshold: " 
	      << m_minPixHits
	      << endmsg;
      m_DQtrackPass = false;
      return HLT::OK;
    }
  m_cutCounter2++; // at least one track pass the pixel hits threshold

  if (m_HTnSCTHit < m_minSCTHits) 
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: This track SCT Hits = " << m_HTnSCTHit << " "
              << "did not pass the quality threshold: " 
	      << m_minSCTHits
	      << endmsg;
      m_DQtrackPass = false;
      return HLT::OK;
    }
  m_cutCounter2++; // at least one track pass SCT hits threshold
   
  if (fabs(m_HTd0 ) > m_maxD0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: This track D0 = " 
	      << fabs(m_HTd0 ) << " "
              << "did not pass the quality threshold: " 
	      << m_maxD0
	      << endmsg;
      m_DQtrackPass = false;
      return HLT::OK;
    }
  m_cutCounter2++; // at least one track pass d0 threshold

  if (fabs( m_HTz0 ) > m_maxZ0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: This track Z0 * sin(theta) = " 
	      << m_HTz0 << " "
              << "did not pass the quality threshold: " 
	      << m_maxZ0
	      << endmsg;
      m_DQtrackPass = false;
      return HLT::OK;
    }
  m_cutCounter2++; // at least one track pass z0 threshold

  if (fabs(m_HTeta) > m_maxEta)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: This track eta = " 
	      << m_HTeta << " "
              << "did not pass the quality threshold: " 
	      << m_maxEta
	      << endmsg;
      m_DQtrackPass = false;
      return HLT::OK;
    }
  m_cutCounter2++; // at least one track pass eta threshold

  return HLT::OK;

}
