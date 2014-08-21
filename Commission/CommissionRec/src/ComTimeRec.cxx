/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "CommissionRec/ComTimeRec.h"
#include "CommissionEvent/ComTime.h"
#include "CommissionUtils/CosmicTriggerTimeTool.h"

// #include "TrackRecord/TimedTrackRecordCollection.h"
// #include "TrackRecord/TimedTrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrackRecord/TrackRecord.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>

//----------------------------------------------------------------

ComTimeRec::ComTimeRec(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),m_triggerTimeTool(0)
{
  declareProperty("ComTimeKey",m_comTimeKey="ComTime");
  declareProperty("Mode",m_mode="TTRCavern");
  declareProperty("Method",m_method="YZero"); // or "ZZero" or "Origin"
  declareProperty("TTRKey",m_TTRKey="CosmicRecord");
  declareProperty("SamplingPeriod",m_samplingPeriod=1./(40.08*CLHEP::megahertz));
  declareProperty("Velocity", m_velocity=299.792458*CLHEP::millimeter/CLHEP::nanosecond);
}

//----------------------------------------------------------------

ComTimeRec::~ComTimeRec()
{ 
}

//----------------------------------------------------------------

StatusCode ComTimeRec::initialize()
{

  MsgStream logStr(messageService(),name());

  logStr << MSG::INFO << "In initialize()" << endreq;

  if( service( "StoreGateSvc", m_eventStore).isFailure() ) {
    logStr << MSG::FATAL << "Unable to locate the eventStore" << endreq;
    return StatusCode::FAILURE;
  } 


  IToolSvc *toolSvc(0);
  if (service( "ToolSvc",toolSvc  ).isFailure()) {
      logStr << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
      return StatusCode::FAILURE;
  }


  IAlgTool *trigTool(0); 
  if (toolSvc->retrieveTool("CosmicTriggerTimeTool",trigTool).isFailure()) {
      logStr << MSG::ERROR << "Unable to retrieve CosmicTriggerTimeTool" <<endreq;
      return StatusCode::FAILURE; 
  }

  m_triggerTimeTool = dynamic_cast<CosmicTriggerTimeTool*>(trigTool); 
  if(0==m_triggerTimeTool){
      logStr << MSG::ERROR << " Failed to dynamic cast CosmicTriggerTimeTool " <<endreq;
      return StatusCode::FAILURE; 
  } 

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------

StatusCode ComTimeRec::execute()
{

  MsgStream logStr(messageService(),name());

  logStr << MSG::DEBUG << "In execute()" << endreq;

  // Pick method of calculating trigger point.
  if(m_mode == "TTRCavern" ) {

    // Use the Timed Track Record of the muon entering the cavern.
    
    // const TimedTrackRecordCollection* coll;
    const TrackRecordCollection *coll(NULL); 
    if(m_eventStore->retrieve(coll, m_TTRKey).isFailure()) {
      logStr << MSG::ERROR << " can not retrieve TTR with key " << m_TTRKey << endreq; 
      // Put default ComTime into SG for.
      ComTime *theComTime = new ComTime();
      if (m_eventStore->record(theComTime, m_comTimeKey).isFailure( )) {
	logStr << MSG::FATAL << "Cannot record ComTime" << endreq;
	return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }    
    
    int    nMuons           = 0;
    double earliestMuonTime = -1.;
    // TimedTrackRecord* earliestMuon=NULL;
    TrackRecord *earliestMuon=NULL;
    
    // TimedTrackRecordCollection::const_iterator it = coll->begin(); 
    // TimedTrackRecordCollection::const_iterator it_e = coll->end(); 
    TrackRecordCollection::const_iterator it = coll->begin(); 
    TrackRecordCollection::const_iterator it_e = coll->end(); 
    for(; it!=it_e;++it) { 

      // TimedTrackRecord * rec = const_cast<TimedTrackRecord*>( *it ); 
      TrackRecord *rec = const_cast<TrackRecord*>( *it ); 

      if(abs(rec->GetPDGCode()) == 13 ) {
	nMuons++;
	if( (nMuons==1) or (rec->GetTime() < earliestMuonTime) ) {
          // earliestMuon = const_cast<TimedTrackRecord*>(*it);
          earliestMuon = const_cast<TrackRecord*>(*it);
          earliestMuonTime = earliestMuon->GetTime();
	}
      }
    }

    if( nMuons>=1 ) { // use the earliest muon if more than 1
      CLHEP::Hep3Vector pos = earliestMuon->GetPosition(); 
      CLHEP::Hep3Vector mom = earliestMuon->GetMomentum();
      double energy  = sqrt(mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z());

      CLHEP::Hep3Vector cosThetaDir = CLHEP::Hep3Vector(mom.x()/energy, mom.y()/energy, mom.z()/energy);
      
      double time    = earliestMuon->GetTime();
      
      logStr <<MSG::DEBUG << " Earliest Muon xyz position " << pos.x()<<" "
	     << pos.y() << " " << pos.z() << endreq ; 
      logStr <<MSG::DEBUG << "                cosThetaDir " << cosThetaDir.x() << " "
	     << cosThetaDir.y() << " " << cosThetaDir.z() << endreq ; 
      logStr <<MSG::DEBUG << "                   time     " << time    << endreq ; 
      
      // Three different methods of definining the virtrual trigger point
      //   1) m_method = YZero  : extrapolate to Y=0 plane (default)
      //   2) m_method = ZZero  : extrapolate to Z=0 plane
      //   3) m_method = Origin : extrapolate to closest approach to (0,0,0)

      
      double PathCritical;
      CLHEP::Hep3Vector PointCritical;

      if( m_method == "YZero" ) { // Extrapolate to Y=0 plane.  Good for cosmics.
	double dXdY      = cosThetaDir.x()/cosThetaDir.y();
	double dZdY      = cosThetaDir.z()/cosThetaDir.y();

	double XCritical = pos.x()-dXdY*pos.y();
	double YCritical = 0.0;
	double ZCritical = pos.z()-dZdY*pos.y();

        PointCritical = CLHEP::Hep3Vector(XCritical,YCritical,ZCritical);

	PathCritical = sqrt(pow(XCritical-pos.x(),2) + pow(YCritical-pos.y(),2) + 
                            pow(ZCritical-pos.z(),2));
        if( (YCritical-pos.y())*cosThetaDir.y() < 0 ) { // backwards extrapolation?? 
	  logStr << MSG::WARNING << "Backwards extrapolation??  pos.y(),cosThetaDir.y() = "
                 << pos.y()<<"," << cosThetaDir.y() <<endreq;
	  PathCritical = -1.0 * PathCritical;
	}
      }
      else if (m_method == "ZZero") { // Extrapolate to Z=0 plane.  Good for halo.
	double dXdZ      = cosThetaDir.x()/cosThetaDir.z();
	double dYdZ      = cosThetaDir.y()/cosThetaDir.z();

	double XCritical = pos.x()-dXdZ*pos.z();
	double YCritical = pos.y()-dYdZ*pos.z();
	double ZCritical = 0.0;

        PointCritical = CLHEP::Hep3Vector(XCritical,YCritical,ZCritical);

	PathCritical = sqrt(pow(XCritical-pos.x(),2) + pow(YCritical-pos.y(),2) + 
                            pow(ZCritical-pos.z(),2));
        if( (ZCritical-pos.z())*cosThetaDir.z() < 0 ) { // backwards extrapolation?? 
	  logStr << MSG::WARNING << "Backwards extrapolation??  pos.z(),cosThetaDir.z() = "
                 << pos.z()<<"," << cosThetaDir.z() <<endreq;
	  PathCritical = -1.0 * PathCritical;
	}
      }
      else if (m_method == "Origin") { // Extrapolate to closest approach to (0,0,0)
	// See arithmetic in RMcP logbook 28 Feb 2006
	PathCritical = -1.*(pos.x()*cosThetaDir.x() + pos.y()*cosThetaDir.y() +
			    pos.z()*cosThetaDir.z());
	PointCritical = CLHEP::Hep3Vector( pos.x() + PathCritical*cosThetaDir.x(),
				    pos.y() + PathCritical*cosThetaDir.y(),
				    pos.z() + PathCritical*cosThetaDir.z());
      }
      else { // This is bad ... invalid method requested
        logStr << MSG::FATAL  << "Invalid method = " << m_method << endreq;
        return StatusCode::SUCCESS;
      }
      
      double TimeCritical = time + PathCritical/m_velocity;
      double TTCTime      = m_samplingPeriod * floor(TimeCritical/m_samplingPeriod);
      
      logStr <<MSG::DEBUG << " Extrapolated muon xyz position " << PointCritical.x()<<" "
	     << PointCritical.y()<<" " << PointCritical.z()<< endreq;
      logStr <<MSG::DEBUG << "                           path " << PathCritical << endreq;
      logStr <<MSG::DEBUG << "                           time " << TimeCritical << endreq;
      logStr <<MSG::DEBUG << "                        TTCTime " << TTCTime      << endreq;

      ComTime* theComTime = new ComTime(TTCTime,TimeCritical,PointCritical,cosThetaDir);
      if( m_eventStore->record(theComTime, m_comTimeKey).isFailure() ) {
	logStr << MSG::FATAL << "Cannot record ComTime" << endreq;
	return StatusCode::FAILURE;
      }	

      m_triggerTimeTool->setComTime(theComTime); 

    } else {  // no muons entering cavern found.
      logStr <<MSG::DEBUG << " No muons entered cavern." << endreq;
      ComTime *theComTime = new ComTime();
      if( m_eventStore->record(theComTime, m_comTimeKey).isFailure() ) {
	logStr << MSG::FATAL << "Cannot record ComTime" << endreq;
	return StatusCode::FAILURE;
      }
    }
  } else {
    logStr << MSG::FATAL  << "Invalid mode = " << m_mode << endreq;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode ComTimeRec::finalize() {

  MsgStream logStr(messageService(),name());

  logStr << MSG::INFO << "In finalize()" << endreq;

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------
