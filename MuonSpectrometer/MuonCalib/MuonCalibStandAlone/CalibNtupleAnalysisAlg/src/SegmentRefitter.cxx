/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/SegmentRefitter.h"

//MdtCalibFitters
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"
#include "MdtCalibFitters/StraightPatRec.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/T0Refinement.h"

//MdtCalibData
#include "MdtCalibData/IRtResolution.h"

//MdtCalibIOSvc
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//c - c++
#include "set"
#include "iostream"

namespace MuonCalib {

SegmentRefitter::SegmentRefitter(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p), 
m_calib_input_svc("MdtCalibInputSvc", n) {
  m_curved = false;
  declareProperty("Curved", m_curved);
  m_t0_refine = false;
  declareProperty("RefineT0", m_t0_refine);
  m_refine_hit_selection = true;
  declareProperty("RefineHitSelection", m_refine_hit_selection);
  m_road_width = 2.0;
  declareProperty("RoadWidth", m_road_width);
  m_time_out = 2.0;
  declareProperty("TimeOut", m_time_out);
  m_min_hits=0;
  declareProperty("MinSegmentHits", m_min_hits);
  m_max_hits=-1;
  declareProperty("MaxSegmentHits", m_max_hits);
  declareProperty("MdtCalibInputSvc", m_calib_input_svc);
  declareInterface< CalibSegmentPreparationTool >(this);
}

StatusCode SegmentRefitter::initialize(void) {
  if(m_t0_refine) {
    p_t0_refinement = new T0Refinement();
    p_t0_refinement->setTimeOut(m_time_out);
    p_t0_refinement->setRoadWidth(m_road_width);
  } else {
    if(m_curved) {
      CurvedPatRec *cfitter=new CurvedPatRec();
      cfitter->setTimeOut(m_time_out);
      p_qfitter = cfitter;
    } else {
      p_qfitter = new StraightPatRec();
    }
    p_qfitter->setRoadWidth(m_road_width);
    p_qfitter->SetRefineSegmentFlag(m_refine_hit_selection);
  }
  ATH_CHECK( m_calib_input_svc.retrieve() );
  return StatusCode::SUCCESS;
}  //end SegmentRefitter::initialize

void SegmentRefitter::prepareSegments(const MuonCalibEvent *& /*event*/, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  std::set<NtupleStationId> dropped_segments;
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it=segments.begin(); it!= segments.end(); it++) {
    //		std::cout<<it->first.regionId()<<std::endl;
    if(m_t0_refine) {
      if(!refine_t0(it->second)) {
	dropped_segments.insert(it->first);
      }
    } else {
      if(!p_qfitter->fit(*it->second)) {
	dropped_segments.insert(it->first);
      }
    }
    int n_hits(static_cast<int>(it->second->hitsOnTrack()));
    if((m_max_hits>0 && n_hits>m_max_hits) || n_hits<m_min_hits) {
      dropped_segments.insert(it->first);
    }
  }
  for(std::set<NtupleStationId>::iterator it=dropped_segments.begin(); it!=dropped_segments.end(); it++) {
    std::map<NtupleStationId, MuonCalibSegment *>::iterator m_it(segments.find(*it));
    if(m_it!=segments.end()) {
      segments.erase(m_it);
    }
  }
}  //end SegmentRefitter::prepareSegments

inline bool SegmentRefitter::refine_t0(MuonCalibSegment *&segment) {
  double err;
  bool failed;
  p_t0_refinement->setRoadWidth(1.25*m_road_width);
  p_t0_refinement->getDeltaT0(segment, m_calib_input_svc->GetRtRelation(segment->mdtHOT()[0]->identify()), true, err, failed, m_curved);
  if (failed) return false;
  p_t0_refinement->setRoadWidth(m_road_width);
  p_t0_refinement->getDeltaT0(segment, m_calib_input_svc->GetRtRelation(segment->mdtHOT()[0]->identify()), true, err, failed, m_curved);
  if (failed) return false;
  for (unsigned int h=0; h<segment->mdtHitsOnTrack(); h++) {
    const IRtResolution *resol( m_calib_input_svc->GetResolution( segment->mdtHOT()[h]->identify()));
    double radius(segment->mdtHOT()[h]->driftRadius());
    segment->mdtHOT()[h]->setDriftRadius(radius,			resol->resolution(segment->mdtHOT()[h]->driftTime()));
  }
  return true;
}  //end SegmentRefitter::refine_t0

} //namespace MuonCalib
