/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_TimingTool.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

//Constructor

Muon::RPC_TimingTool::RPC_TimingTool(const std::string& t, 
				     const std::string& n, 
				     const IInterface* p):
  AthAlgTool(t,n,p),
  m_idHelper(0)
{
  
  declareInterface<Trk::ITrackTimingTool>(this);

}

//Destructor

Muon::RPC_TimingTool::~RPC_TimingTool() {}

//Initialization

StatusCode Muon::RPC_TimingTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  if(sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_idHelper, "RPCIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_FATAL ("Could not get Rpc Id Helper" );
    return StatusCode::FAILURE;
  }

  /*  sc = m_helperTool.retrieve();
  if(sc.isFailure()){
    ATH_MSG_FATAL("Unable to retrieve MuonSpectro EDM helper tool "<<m_helperTool);
    return sc;
  } else ATH_MSG_DEBUG("Successfully retrieved "<<m_helperTool);
*/
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;

}

//Finalization

StatusCode Muon::RPC_TimingTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

std::vector<Trk::SpaceTimePoint*> Muon::RPC_TimingTool::timeMeasurements
  (const Trk::Track& track,
   const Trk::SegmentCollection* /* muted */) const
{

  std::vector<Trk::SpaceTimePoint*> spaceTimePoints;
  ATH_MSG_DEBUG ("entering timeMeasurements(track) ...");
    
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = 
    track.trackStateOnSurfaces()->begin();
  for(; tsit!=track.trackStateOnSurfaces()->end() ; ++tsit) {
    const Trk::MeasurementBase* measurement = (*tsit)->measurementOnTrack();
    if( (measurement == NULL) || 
	(*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;

    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
    if( !rot ) {
      const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(measurement);
      if( crot && !crot->containedROTs().empty() ) {
	rot = crot->containedROTs().front();
      }
    }
    if (!rot) continue;
    const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(rot);
    //const RpcPrepData* rpcPRD = rpc ? rpc->prepRawData() : 0;
    //if (rpcPRD==NULL) continue; //  or: if(m_idHelper->is_rpc(rot->identify())) continue;
    if (rpc == NULL ) continue;
    
    //double t0_offset  = rpcPRD->time();
    double t0_offset  = rpc->time();
    double t0_error   = getError(rpc);
    ATH_MSG_DEBUG ("Track has RPC with time = "<<t0_offset<<", error = "<<t0_error);
    spaceTimePoints.push_back(new Trk::SpaceTimePoint(rpc->globalPosition(), t0_offset, t0_error, 1.0/(t0_error*t0_error), Trk::TrackState::RPC));
  }
  return spaceTimePoints;
}

std::vector<Trk::SpaceTimePoint*> Muon::RPC_TimingTool::timeMeasurements(const Trk::TrackParticleBase& tpl) const {
  std::vector<Trk::SpaceTimePoint*> s;
  return tpl.originalTrack()                  ?
    timeMeasurements(* (tpl.originalTrack())) :
    s;
}


//NOTE: Should only be passed a vector of segments that can be cast to MuonSegment*
std::vector<Trk::SpaceTimePoint*> Muon::RPC_TimingTool::timeMeasurements(std::vector<const Trk::Segment*> segs) const 
{

  std::vector<Trk::SpaceTimePoint*> spaceTimePoints;

  std::vector<const Trk::Segment*>::const_iterator seg_iter = segs.begin();
  std::vector<const Trk::Segment*>::const_iterator seg_end = segs.end();
  
  //Loop over the segments
  for(; seg_iter != segs.end(); ++seg_iter) {

    //Cast to a muon segment
    const Muon::MuonSegment* mu_seg = dynamic_cast<const Muon::MuonSegment*>(*seg_iter);
    if(!mu_seg) continue;

    std::vector<const Trk::MeasurementBase*>::const_iterator itm = 
      mu_seg->containedMeasurements().begin();
    for( ;itm!=mu_seg->containedMeasurements().end();++itm ){

      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*itm);
      if( !rot ) {
	const CompetingMuonClustersOnTrack* crot = dynamic_cast<const CompetingMuonClustersOnTrack*>(*itm);
	if( crot && !crot->containedROTs().empty() ) rot = crot->containedROTs().front();
      }
      
      if (!rot) continue;

      const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(rot);
      //const RpcPrepData* rpcPRD = rpc ? rpc->prepRawData() : 0;
      //if (rpcPRD==NULL) continue;
      if (rpc==NULL) continue;
      

      //double t0_offset  = rpcPRD->time();
      double t0_offset  = rpc->time();
      double t0_error   = getError(rpc);
      ATH_MSG_DEBUG ("Segment has RPC with time = "<<t0_offset<<", error = "<<t0_error);
      spaceTimePoints.push_back(new Trk::SpaceTimePoint(rpc->globalPosition(), t0_offset, t0_error, 1.0/(t0_error*t0_error), Trk::TrackState::RPC));
    }

  }

  return spaceTimePoints;
}

double Muon::RPC_TimingTool::getError(const Muon::RpcClusterOnTrack* rpcRIO) const 
{ 

  // S. Spagnolo: Aug. 5th 2011 - implementation for rel 17 reprocessing (based on slide 8 and 9 of 
  // https://indico.cern.ch/getFile.py/access?contribId=6&resId=0&materialId=slides&confId=147791
  // ... no calibration offline yet, no correction for signal propagation time)
  Identifier id = rpcRIO->identify();
  int measPhi = m_idHelper->measuresPhi(id);
  std::string stName = m_idHelper->stationNameString(m_idHelper->stationName(id));

  if (stName.substr(1,1)=="O")
    {
      // outer layer station => high pt 
      if (measPhi==1) return 5.10;
      else return 4.84;     
    }
  else 
    {
      // pivot plane of Middle station dbR=2 or low pT confirm dbR=1 but names like "BMS", "BML", etc
      if (measPhi==1) return 5.04;
      else return 4.18;      
    }
}
// double Muon::RPC_TimingTool::getError(const Muon::RpcPrepData& /*RpcPrd*/) const{
//   //TO BE UPDATED FURTHER:.

//   return 3.125; // one time bin resolution?
// }

