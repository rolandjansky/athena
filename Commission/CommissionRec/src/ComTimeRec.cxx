/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CommissionRec/ComTimeRec.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "Gaudi/Property.h"

#include "CommissionEvent/ComTime.h"

// #include "TrackRecord/TimedTrackRecordCollection.h"
// #include "TrackRecord/TimedTrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrackRecord/TrackRecord.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>

//----------------------------------------------------------------

ComTimeRec::ComTimeRec(const std::string& name,
                       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),m_triggerTimeTool(0)
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

  ATH_MSG_INFO( "In initialize()" );

  // This level of obfuscation is necessary because ITriggerTime, the interface class for
  //  the timing tool, does not provide the method that is needed here (which is bad design).
  // The right thing to do is to fix the interface, at which point this is a simple retrieve.
  // Until then, we are stuck with the old way.
  IToolSvc *toolSvc(0);
  CHECK( service( "ToolSvc",toolSvc ) );

  IAlgTool *trigTool(0);
  CHECK( toolSvc->retrieveTool("CosmicTriggerTimeTool",trigTool) );

  m_triggerTimeTool = dynamic_cast<CosmicTriggerTimeTool*>(trigTool);
  if(0==m_triggerTimeTool){
    ATH_MSG_ERROR( " Failed to dynamic cast CosmicTriggerTimeTool " );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------

StatusCode ComTimeRec::execute()
{

  ATH_MSG_DEBUG( "In execute()" );

  // Pick method of calculating trigger point.
  if(m_mode == "TTRCavern" ) {

    // Use the Timed Track Record of the muon entering the cavern.

    if(!evtStore()->contains<TrackRecordCollection>(m_TTRKey) ){
      ATH_MSG_ERROR( " can not retrieve TrackRecordCollection with key " << m_TTRKey );
      // Put default ComTime into SG for.
      ComTime *theComTime = new ComTime();
      CHECK( evtStore()->record(theComTime, m_comTimeKey) );
      return StatusCode::SUCCESS;
    }
    // const TimedTrackRecordCollection* coll;
    const TrackRecordCollection *coll(NULL);
    CHECK( evtStore()->retrieve(coll, m_TTRKey) );
    int    nMuons           = 0;
    double earliestMuonTime = -1.;
    // TimedTrackRecord* earliestMuon=NULL;
    TrackRecord *earliestMuon=nullptr;

    for(auto it : *coll) {

      if(abs(it.GetPDGCode()) == 13 ) {
        nMuons++;
        if( (nMuons==1) or (it.GetTime() < earliestMuonTime) ) {
          // earliestMuon = const_cast<TimedTrackRecord*>(*it);
          earliestMuon = &it;
          earliestMuonTime = it.GetTime();
        }
      }
    }

    if( nMuons>=1 ) { // use the earliest muon if more than 1
      CLHEP::Hep3Vector pos = earliestMuon->GetPosition();
      CLHEP::Hep3Vector mom = earliestMuon->GetMomentum();
      double energy  = sqrt(mom.x()*mom.x() + mom.y()*mom.y() + mom.z()*mom.z());

      CLHEP::Hep3Vector cosThetaDir = CLHEP::Hep3Vector(mom.x()/energy, mom.y()/energy, mom.z()/energy);

      double time    = earliestMuon->GetTime();

      ATH_MSG_DEBUG( " Earliest Muon xyz position " << pos.x()<<" "
                     << pos.y() << " " << pos.z() );
      ATH_MSG_DEBUG( "                cosThetaDir " << cosThetaDir.x() << " "
                     << cosThetaDir.y() << " " << cosThetaDir.z() );
      ATH_MSG_DEBUG( "                   time     " << time );

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
          ATH_MSG_WARNING( "Backwards extrapolation??  pos.y(),cosThetaDir.y() = "
                           << pos.y()<<"," << cosThetaDir.y() );
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
          ATH_MSG_WARNING( "Backwards extrapolation??  pos.z(),cosThetaDir.z() = "
                           << pos.z()<<"," << cosThetaDir.z() );
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
        ATH_MSG_FATAL( "Invalid method = " << m_method );
        return StatusCode::FAILURE;
      }

      double TimeCritical = time + PathCritical/m_velocity;
      double TTCTime      = m_samplingPeriod * floor(TimeCritical/m_samplingPeriod);

      ATH_MSG_DEBUG( " Extrapolated muon xyz position " << PointCritical.x()<<" "
                     << PointCritical.y()<<" " << PointCritical.z());
      ATH_MSG_DEBUG( "                           path " << PathCritical );
      ATH_MSG_DEBUG( "                           time " << TimeCritical );
      ATH_MSG_DEBUG( "                        TTCTime " << TTCTime      );

      ComTime* theComTime = new ComTime(TTCTime,TimeCritical,PointCritical,cosThetaDir);
      CHECK( evtStore()->record(theComTime, m_comTimeKey) );

      m_triggerTimeTool->setComTime(theComTime);

    } else {  // no muons entering cavern found.
      ATH_MSG_DEBUG( " No muons entered cavern." );
      ComTime *theComTime = new ComTime();
      CHECK( evtStore()->record(theComTime, m_comTimeKey) );
    }
  } else if(m_mode == "CollisionMode" ) {
    // Using cosmic reco during collisions. Fire on BCIDs.
    // Just provide a default ComTime object.
    ComTime *theComTime = new ComTime();
    CHECK( evtStore()->record(theComTime, m_comTimeKey) );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_FATAL( "Invalid mode = " << m_mode );
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode ComTimeRec::finalize() {

  ATH_MSG_INFO( "In finalize()" );

  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------
