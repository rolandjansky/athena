/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelector.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"

#include "MuonCalibEventBase/MuonCalibEvent.h"

#include "TFile.h"
#include "TH1.h"
#include "TDirectory.h"

namespace MuonCalib {

///////////////////////////
// ExtraRegionSelector //
///////////////////////////
ExtraRegionSelector::ExtraRegionSelector(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p) {
  m_exclusive_segments = false;
  declareProperty("ExclusiveSegments", m_exclusive_segments);
  m_muonboy = false;
  declareProperty("MuonBoyTracks", m_muonboy);
  m_min_hits = 1;
  declareProperty("MinHits", m_min_hits);
  m_max_hits = 0; 
  declareProperty("MaxHits", m_max_hits);
  m_select_seg_author = 4;
  declareProperty("SelectSegmentAuthor", m_select_seg_author);
  m_momentum_cut=-1.5;
  declareProperty("MomentumCut", m_momentum_cut);
  m_do_debug_file=false;
  declareProperty("DoDebugFile", m_do_debug_file);
  m_debug_file=NULL;
  m_all_p=NULL;
  m_selected_p_by_track=NULL;
  m_selected_p_by_segment=NULL;
  declareInterface< CalibSegmentPreparationTool >(this);
}  //end ExtraRegionSelector::ExtraRegionSelector

////////////////
// initialize //
////////////////
StatusCode ExtraRegionSelector::initialize(void) {
  MsgStream log(msgSvc(), name());
  StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
  if(!sc.isSuccess())	{
    log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endmsg;
  }
  if(m_muonboy) {
    m_track_author_id = m_evt_handler.muonboyIndices().track;
  } else {
    m_track_author_id = m_evt_handler.mooreIndices().track;
  }
  if(m_do_debug_file)	{
    TDirectory *prev_dir=gDirectory;
    m_debug_file =  new TFile("ExtraRegionSelector.root", "RECREATE");
    m_all_p = new TH1F("all_p", "" ,20000, -100000, 100000);
    m_selected_p_by_track = new TH1F("selected_p_by_track", "" ,20000, -100000, 100000);
    m_selected_p_by_segment = new TH1F("selected_p_by_segment", "" ,20000, -100000, 100000);
    prev_dir->cd();
  }
  return sc;
}  //end ExtraRegionSelector::initialize

StatusCode ExtraRegionSelector::finalize(void) {
  if(m_do_debug_file) {
    m_debug_file->Write();
    delete m_all_p;
    delete m_selected_p_by_track;
    delete m_selected_p_by_segment;
    delete m_debug_file;
  }
  return StatusCode::SUCCESS;
}

void ExtraRegionSelector::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  MsgStream log(msgSvc(), name());
  if(event==NULL) {
    log << MSG::FATAL << "event=NULL"<<endmsg;
    event=NULL;
    return;
  }
  const MuonCalibEvent *e(event);
  const MuonCalibEvent_E *e_event=dynamic_cast<const MuonCalibEvent_E *>(e);
  if(e_event==NULL) {
    log << MSG::FATAL << "This tool works only with extra events!"<<endmsg;
    event=NULL;
    return;
  }
  segments.clear();
  m_evt_handler.setEvent(*e_event);
  std::vector<unsigned int> calib_region_hit_map;
  const EventHandler::TrackVec& e_trks(m_evt_handler.extendedTracks(m_track_author_id));
  for(EventHandler::TrkCit tit = e_trks.begin(); tit!=e_trks.end(); tit++) {
    const MuonCalibExtendedTrack *trk(*tit);
    const std::vector<const MuonCalibExtendedSegment*> &segs(trk->associatedSegments());
    if(m_all_p) m_all_p->Fill(trk->p());
    if (m_momentum_cut>-1) {
      if(trk->p()>1e8) continue;
      if(m_momentum_cut>=0 && std::fabs(trk->p())<m_momentum_cut) continue;
    }
    if(m_selected_p_by_track) m_selected_p_by_track->Fill(trk->p());
    for(std::vector<const MuonCalibExtendedSegment*>::const_iterator seg_it=segs.begin(); seg_it!=segs.end(); seg_it++)	{
      MuonCalibExtendedSegment *seg = const_cast<MuonCalibExtendedSegment *>(*seg_it);
      //check count hits in the calibration region and remove others
      calib_region_hit_map.clear();
      bool reject(false);
      int count(0);
      for(MuonCalibSegment::MdtHitCit hit_it=seg->mdtHOTBegin(); hit_it!=seg->mdtHOTEnd(); hit_it++){
	unsigned int is_in_region(static_cast<unsigned int>(!p_reg_sel_svc->isInRegion((*hit_it)->identify())));// || m_suppressed_tubes.find((*hit_it)->identify())!=m_suppressed_tubes.end()));
	calib_region_hit_map.push_back(is_in_region);
	if(is_in_region!=0 && m_exclusive_segments){
	  reject=true;
	  break;
	}
	if(is_in_region==0)
	  count++;
      }
      if(reject) continue;
      if(count<m_min_hits || (count>m_max_hits && m_max_hits>0)) continue;
      if(count==0) continue;
      seg->refineMdtSelection(calib_region_hit_map);
      NtupleStationId id(seg->mdtHOT()[0]->identify());
      id.SetMultilayer(0);
      if(m_selected_p_by_segment) m_selected_p_by_segment->Fill(trk->p());
      //store segment if it is the first in the station
      if(segments[id] == NULL) {
	segments[id] = seg;
	continue;
      }
      //overwrite segment if this has more hits than the stored segment
      MuonCalibSegment *stored(segments[id]);
      if(stored->mdtHitsOnTrack() < seg->mdtHitsOnTrack()) {
	segments[id] = seg;
	continue;
      }
      if(stored->mdtHitsOnTrack() > seg->mdtHitsOnTrack()) {
	continue;
      }		
      //overwrite segment if the new one has a better chi^2
      if(stored->chi2() > seg->chi2()){
	segments[id] = seg;
      }
    }
  }
}  //end ExtraRegionSelector::prepareSegments

} // namespace Muoncalib
