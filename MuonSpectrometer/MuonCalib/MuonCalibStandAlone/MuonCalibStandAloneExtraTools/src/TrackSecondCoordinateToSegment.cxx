/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneExtraTools/TrackSecondCoordinateToSegment.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"

//root
#include "TFile.h"
#include "TH1.h"

namespace MuonCalib {

TrackSecondCoordinateToSegment::TrackSecondCoordinateToSegment(const std::string &t, const std::string &n, const IInterface *p): 
  AthAlgTool(t, n, p), m_root_file(NULL), m_delta_x(NULL), m_delta_sx(NULL), m_delta_hit_x(NULL) {
  m_control_histos = true;
  declareProperty("ControlHistograms", m_control_histos);
  declareInterface< CalibSegmentPreparationTool >(this);
}

////////////////
// initialize //
////////////////
StatusCode TrackSecondCoordinateToSegment::initialize(void) {
  if(m_control_histos) {
    m_root_file  = new TFile("TrackSecondCoordinateToSegment.root", "RECREATE");
    m_delta_x    = new TH1F("delta_x", "", 4000, -8000, 8000);
    m_delta_sx   = new TH1F("delta_sx", "", 100, -10, 10);
    m_delta_hit_x= new TH1F("delta_hit_x", "", 4000, -8000, 8000);
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackSecondCoordinateToSegment::finalize(void) {
  if(m_root_file != NULL) {
    m_root_file->Write();
  }
  return StatusCode::SUCCESS;
}

void TrackSecondCoordinateToSegment::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  ATH_MSG_INFO("START TrackSecondCoordinateToSegment::prepareSegments");
  //loop over segments
  for(std::map<NtupleStationId, MuonCalibSegment *>::iterator it=segments.begin(); it!= segments.end(); it++) {
    MuonCalibExtendedSegment *e_segment(dynamic_cast<MuonCalibExtendedSegment *>(it->second));
    if(e_segment==NULL) {
      ATH_MSG_FATAL("This tool only works with MuonCalibExtendedSegment!");
      event = NULL;
      return;
    }
    //select best track
    const MuonCalibExtendedTrack *track(NULL);
    for(std::vector<const MuonCalibExtendedTrack*>::const_iterator t_it=e_segment->associatedTracks().begin(); t_it != e_segment->associatedTracks().end(); t_it++) {
      if(track==NULL) {
	track = *t_it;
	continue;
      }
      if(track->nrHits() < (*t_it)->nrHits()) {
	track = *t_it;
	continue;				
      }
      if(track->nrHits() > (*t_it)->nrHits()) {
	continue;
      }
      double prev_chi2 = track->chi2()/static_cast<double>(track->ndof());
      double cur_chi2 = (*t_it)->chi2()/static_cast<double>((*t_it)->ndof());
      if(cur_chi2 < prev_chi2) {
	track = *t_it;
      }
    }
    if(track==NULL) {
      ATH_MSG_WARNING("No track assigned to segment!");
      continue;
    }
    
    apply_2n_coordinate(track, e_segment);
  }
}  //end TrackSecondCoordinateToSegment::prepareSegments

inline void TrackSecondCoordinateToSegment::apply_2n_coordinate(const MuonCalibExtendedTrack *track, MuonCalibSegment *segment) const {
  //get transformations
  Amg::Transform3D global_to_local((segment->localToGlobal()).inverse());
  Amg::Transform3D local_to_global((segment->localToGlobal()));
  //transform track position and direction to local coordinates	
  const Amg::Vector3D trk_pos(global_to_local *track->position());
  const Amg::Vector3D trk_dir(global_to_local *track->direction());
  //get local track parameters
  const Amg::Vector3D &seg_pos(segment->position());
  const Amg::Vector3D &seg_dir(segment->direction());
  //fit 2nd coordinate in to existing parameters		
  double new_x = trk_pos.x() + ((seg_pos.z() - trk_pos.z())/trk_dir.z()) * trk_dir.x();
  double new_dx = (trk_dir.x()/trk_dir.z()) * seg_dir.z();
  //update track position and direction
  Amg::Vector3D new_pos(new_x, seg_pos.y(), seg_pos.z());
  Amg::Vector3D new_dir(new_dx, seg_dir.y(), seg_dir.z());
  if(m_root_file != NULL) {
    m_delta_x->Fill(new_pos.x() - seg_pos.x());
    m_delta_sx->Fill(new_dir.x()/seg_dir.z() - seg_dir.x()/seg_dir.z());
  }
  segment->SetSegment(new_pos, new_dir);
  //update mdt hit positions
  for(MuonCalibSegment::MdtHitVec::iterator h_it = segment->mdtHOTBegin(); h_it != segment->mdtHOTEnd(); h_it++) {
    MdtCalibHitBase &hit(*(*h_it));
    Amg::Vector3D local_position(hit.localPosition());
    updatePosition(local_position, new_pos, new_dir);
    if(m_delta_hit_x) {
      m_delta_hit_x->Fill(local_position.x() - hit.localPosition().x());
    }
    hit.setLocalPos(local_position);
    hit.setGlobalPos(local_to_global *local_position);
    Amg::Vector3D point_of_closest_ap( hit.localPointOfClosestApproach() );
    updatePosition(point_of_closest_ap, new_pos, new_dir);
    hit.setLocalPointOfClosestApproach(point_of_closest_ap);
    hit.setGlobalPointOfClosestApproach( local_to_global*point_of_closest_ap);
  }
}  //end TrackSecondCoordinateToSegment::apply_2n_coordinate

inline void TrackSecondCoordinateToSegment::updatePosition(Amg::Vector3D &position, const Amg::Vector3D &seg_pos, const Amg::Vector3D &seg_dir) const {
  double new_x = seg_pos.x() + ((position.z() - seg_pos.z())/seg_dir.z()) * seg_dir.x();
  //position.setX(new_x);
  position=Amg::Vector3D(new_x, position.y(), position.z());
}

}  //namespace MuonCalib
