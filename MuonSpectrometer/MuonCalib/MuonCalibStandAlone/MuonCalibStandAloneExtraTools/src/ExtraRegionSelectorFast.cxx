/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelectorFast.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibExtraTreeEvent
#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibTrack_E.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include <algorithm>

namespace MuonCalib {

inline void fill_histogram(TH1F *&hist, const Double_t &val) {
  if (hist) hist->Fill(val);
}

inline bool ExtraRegionSelectorFast::isOnTrack(const MuonCalibSegment &seg, const std::list<std::set<MuonFixedId> > &track_hits) {
  int n_max_match(0);
  int max_missing(seg.mdtHitsOnTrack() - m_max_hits_not_on_track);
  int n_min_hits_on_track(max_missing<m_min_hits_on_track ? m_min_hits_on_track : max_missing);
  for(std::list<std::set<MuonFixedId> >::const_iterator it=track_hits.begin(); it!=track_hits.end(); it++) {
    int n_match(0);
    for(MuonCalibSegment::MdtHitCit hit_it=seg.mdtHOTBegin(); hit_it!=seg.mdtHOTEnd(); hit_it++) {
      if(it->find((*hit_it)->identify()) != it->end()) {
	n_match++;
      }
      if((!m_n_match_vs_missing) && n_match>=n_min_hits_on_track) return true;
    }
    if(n_max_match<n_match) {
      n_max_match=n_match;
    }
  }
  if(m_n_match_vs_missing) {
    m_n_match_vs_missing->Fill(seg.mdtHitsOnTrack() -  n_max_match, n_max_match);
    if(n_max_match>=n_min_hits_on_track) return true;
  }
  return false;
}  //end ExtraRegionSelectorFast::isOnTrack

///////////////////////////
// ExtraRegionSelectorFast //
///////////////////////////
ExtraRegionSelectorFast::ExtraRegionSelectorFast(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p) {
  m_exclusive_segments = false;
  declareProperty("ExclusiveSegments", m_exclusive_segments);
  declareProperty("TrackAutors", m_track_author_vec);
  m_min_hits = 1;
  declareProperty("MinHits", m_min_hits);
  m_max_hits = 0; 
  declareProperty("MaxHits", m_max_hits);
  m_select_seg_author = 4;
  declareProperty("SelectSegmentAuthor", m_select_seg_author);
  m_momentum_cut=-1.5;
  declareProperty("MomentumCut", m_momentum_cut);
  m_min_hits_on_track=3;
  declareProperty("MinHitsOnTrack", m_min_hits_on_track);
  m_max_hits_not_on_track=1;
  declareProperty("MaxHitsNotOnTrack", m_max_hits_not_on_track);
  m_do_debug_file=false;
  declareProperty("DoDebugFile", m_do_debug_file);
  m_debug_file=NULL;
  m_all_p=NULL;
  m_selected_p_by_track=NULL;
  m_selected_p_by_segment=NULL;
  m_n_match_vs_missing=NULL;
  declareInterface< CalibSegmentPreparationTool >(this);
}  //end ExtraRegionSelectorFast::ExtraRegionSelectorFast

////////////////
// initialize //
////////////////
StatusCode ExtraRegionSelectorFast::initialize(void) {
  MsgStream log(msgSvc(), name());
  StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
  if(!sc.isSuccess()) {
    log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endmsg;
  }
  if(m_do_debug_file) {
    TDirectory *prev_dir=gDirectory;
    m_debug_file =  new TFile("ExtraRegionSelectorFast.root", "RECREATE");
    m_all_p = new TH1F("all_p", "" ,20000, -100000, 100000);
    m_selected_p_by_track = new TH1F("selected_p_by_track", "" ,20000, -100000, 100000);
    m_selected_p_by_segment = new TH1F("selected_p_by_segment", "" ,20000, -100000, 100000);
    m_n_match_vs_missing = new TH2F("n_match", "", 30, -0.5, 29.5, 30, -0.5, 29.5);
    prev_dir->cd();
  }
  m_track_author.clear();
  m_track_author.insert(m_track_author_vec.begin(), m_track_author_vec.end());
  return sc;
}  //end ExtraRegionSelectorFast::initialize

StatusCode ExtraRegionSelectorFast::finalize(void) {
  if(m_do_debug_file) {
    m_debug_file->Write();
    delete m_all_p;
    delete m_selected_p_by_track;
    delete m_selected_p_by_segment;
    delete m_debug_file;
  }
  return StatusCode::SUCCESS;
}

void ExtraRegionSelectorFast::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
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
  std::list<std::set<MuonFixedId> > track_hits;
  for(MuonCalibEvent_E::TrackVec::const_iterator tit=e_event->beginTrack(); tit!=e_event->endTrack(); tit++){
    const MuonCalibTrack_E *trk(*tit);
    if (m_track_author.size()>0 && m_track_author.find(trk->author())==m_track_author.end())
      continue;
    fill_histogram(m_all_p, 1.0/trk->qOverP());
    if (m_momentum_cut>-1) {
      if(trk->qOverP()==0 || trk->p()>1e8) continue;
      if(m_momentum_cut>=0 && std::fabs(trk->p())<m_momentum_cut) continue;
    }
    fill_histogram(m_selected_p_by_segment, 1.0/trk->qOverP());
    std::set<MuonFixedId> selected_hits;
    bool has_hits_in_region(false);
    for(MuonCalibTrack_E::HitVector::const_iterator hitit=trk->beginHit(); hitit!=trk->endHit(); hitit++) {
      const MuonCalibHit_E *hit(*hitit);
      const MuonFixedId id(hit->identify());
      if(!id.is_mdt()) continue;
      if(!has_hits_in_region && p_reg_sel_svc->isInRegion(id)){
	has_hits_in_region=true;
      }
      selected_hits.insert(id);
    }
    if(selected_hits.size()>0 && has_hits_in_region)
      track_hits.push_back(selected_hits);	
  }
  if(track_hits.size()==0)
    return;
  std::vector<unsigned int> calib_region_hit_map;
//---------------------loop on all patterns-------------------------------------
  MuonCalibEvent::MCPVecCit pat_it = event->patternBegin();
  MuonCalibEvent::MCPVecCit pat_it_end = event->patternEnd();
  for( ;pat_it!=pat_it_end; ++pat_it) {
    //--------------------loop on all segments--------------------------------------
    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it = (*pat_it)->muonSegBegin();
    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it_end = (*pat_it)->muonSegEnd();
    for ( ; seg_it!=seg_it_end; ++seg_it) {
      MuonCalibSegment *seg = *seg_it;
      if (m_select_seg_author && m_select_seg_author!=seg->author())
	continue;
      if(!isOnTrack(*seg, track_hits))
	continue;
      //check count hits in the calibration region and remove others
      calib_region_hit_map.clear();
      bool reject(false);
      int count(0);
      for(MuonCalibSegment::MdtHitCit hit_it=seg->mdtHOTBegin(); hit_it!=seg->mdtHOTEnd(); hit_it++){
	unsigned int is_in_region(static_cast<unsigned int>(!p_reg_sel_svc->isInRegion((*hit_it)->identify())));// || m_suppressed_tubes.find((*hit_it)->identify())!=m_suppressed_tubes.end()));
	calib_region_hit_map.push_back(is_in_region);
	if(is_in_region!=0 && m_exclusive_segments) {
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
      id.SetAuthor(seg->author());
      //store segment if it is the first in the station
      if(segments[id] == NULL){
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
}  //end ExtraRegionSelectorFast::prepareSegments

} // namespace Muoncalib
