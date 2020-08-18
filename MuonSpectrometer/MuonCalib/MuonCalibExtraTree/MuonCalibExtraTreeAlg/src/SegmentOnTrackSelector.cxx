/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeAlg/SegmentOnTrackSelector.h"

#include "TrkTrack/Track.h"
#include "MuonCalibEvent/MuonCalibPatternCollection.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

SegmentOnTrackSelector::SegmentOnTrackSelector(const std::string &type,const std::string &name,const IInterface *parent): 
  AthAlgTool(type,name,parent),
  m_pattern_location(""),
  m_min_hits_on_track(3),
  m_max_hits_not_on_track(1)
{
  declareInterface<ISegmentOnTrackSelector>(this);
  declareProperty("PattternLocation", m_pattern_location);
  declareProperty("MinHitsOnTrack", m_min_hits_on_track);
  declareProperty("MaxHitsNotOnTrack", m_max_hits_not_on_track);
}

StatusCode SegmentOnTrackSelector::initialize() {
  if(m_pattern_location=="") {
    ATH_MSG_FATAL("Cannot run without patterns!");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_edmHelperSvc.retrieve());
  ATH_CHECK(m_idToFixedIdTool.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode SegmentOnTrackSelector::beginEvent() {
  const MuonCalibPatternCollection *patterns;
  if(!evtStore()->retrieve(patterns, m_pattern_location).isSuccess()) {
    ATH_MSG_FATAL("Cannot retrieve patterns from '"<<m_pattern_location<<"'");
    return StatusCode::FAILURE;
  }
  m_segment_hits.clear();
  m_authors.clear();
	
  MuonCalibPatternCollection::const_iterator it=patterns->begin();
  MuonCalibPatternCollection::const_iterator it_end=patterns->end();
  for(;it!=it_end; it++) {
    const MuonCalibPattern *pat(*it);
    for(MuonCalibPattern::MuonSegCit seg_it=pat->muonSegBegin(); seg_it!=pat->muonSegEnd(); seg_it++) {
      storeSegment(**seg_it);
    }
  }
  return StatusCode::SUCCESS;
}  //end SegmentOnTrackSelector::beginEvent

std::vector<unsigned int> SegmentOnTrackSelector::SegmentIndices(const Trk::Track &track, const std::set<int> segment_authors) const {
//get muonfixedids of MDT and CSC hits on the track  
  std::set<MuonFixedId> ids;
  getIdentifiers(track, ids);  

  ATH_MSG_DEBUG("start creating segment vector! segment_authors.size()=" << segment_authors.size() << " and m_authors.size()=" << m_authors.size());
  for(auto a: segment_authors) ATH_MSG_DEBUG("segment_authors()=" << a);
  std::vector<unsigned int> ret;
  for(unsigned int i=0; i<m_authors.size(); i++) {
    ATH_MSG_DEBUG("m_authors[i]=" << m_authors[i]);
    if(segment_authors.find(m_authors[i])==segment_authors.end()) {
      continue;
    }
    ATH_MSG_DEBUG("m_authors[i]=" << m_authors[i] << " found.");
    std::vector<MuonFixedId> result(ids.size());
    std::vector<MuonFixedId>::iterator out_it=std::set_intersection( ids.begin(), ids.end(), m_segment_hits[i].begin(), m_segment_hits[i].end(), result.begin());
    int n_match(out_it - result.begin());
    ATH_MSG_DEBUG("cuts: n_match=" << n_match << "<" << m_min_hits_on_track << "||" << static_cast<int>(m_segment_hits[i].size()) - n_match << "<m_max_hits_not_on_track=" << m_max_hits_not_on_track);
    if(n_match<m_min_hits_on_track || (static_cast<int>(m_segment_hits[i].size()) - n_match) > m_max_hits_not_on_track)
      continue;
    ATH_MSG_DEBUG("get here");
    ret.push_back(i);
  }
  return ret;
}  //end SegmentOnTrackSelector::SegmentIndices

//  Fill set ids with muonfixedid's of MDT and CSC hits on a segment
//  Add segment author to vector m_authors
//  add ids to m_segment_hits
inline void SegmentOnTrackSelector::storeSegment(const MuonCalibSegment &seg) {
  m_authors.push_back(seg.author());
  std::set<MuonFixedId> ids;
  for(MuonCalibSegment::MdtHitCit it=seg.mdtHOTBegin(); it!=seg.mdtHOTEnd(); it++) {
    ids.insert((*it)->identify());
  }
  for(MuonCalibSegment::CscHitCit it=seg.cscHOTBegin(); it!=seg.cscHOTEnd(); it++) {
    ids.insert((*it)->identify());
  }
  m_segment_hits.push_back(ids);
}  //end SegmentOnTrackSelector::storeSegment

//  Fill set ids with muonfixedid's of MDT and CSC hits on a track
inline void SegmentOnTrackSelector::getIdentifiers(const Trk::Track &tk, std::set<MuonFixedId> &ids) const {
  ids.clear();
  const DataVector<const Trk::TrackStateOnSurface>* states = tk.trackStateOnSurfaces();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();	
  for(; tsit!=tsit_end ; ++tsit) {
    if( !(*tsit)->type(Trk::TrackStateOnSurface::Measurement) )
      continue;
    const Trk::MeasurementBase* measurement = (*tsit)->measurementOnTrack();
    Identifier id = m_edmHelperSvc->getIdentifier(*measurement);
    if(!m_idHelperSvc->isMuon(id)) continue;
    if(!m_idHelperSvc->isMdt(id) && !m_idHelperSvc->isCsc(id)) continue;
    ids.insert(m_idToFixedIdTool->idToFixedId(id));
  }
}  //end SegmentOnTrackSelector::getIdentifiers

} //namespace MuonCalib 
