/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/SegmentRegionSelector.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"

namespace MuonCalib {

///////////////////////////
// SegmentRegionSelector //
///////////////////////////
SegmentRegionSelector::SegmentRegionSelector(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p), 
m_reg_sel_svc("RegionSelectionSvc", n) {
  m_exclusive_segments = false;
  declareProperty("ExclusiveSegments", m_exclusive_segments);
  m_min_hits = 1;
  declareProperty("MinHits", m_min_hits);
  m_max_hits = 0; 
  declareProperty("MaxHits", m_max_hits);
  m_select_seg_author = 4;
  declareProperty("SelectSegmentAuthor", m_select_seg_author);
  m_adc_cut = 0;
  declareProperty("AdcCut", m_adc_cut);
  declareProperty("RegionSelectionSvc", m_reg_sel_svc);
  declareInterface< CalibSegmentPreparationTool >(this);
}

////////////////
// initialize //
////////////////
StatusCode SegmentRegionSelector::initialize(void) {
  ATH_CHECK( m_reg_sel_svc.retrieve() );
  return StatusCode :: SUCCESS;
}

void SegmentRegionSelector::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  segments.clear();
  std::vector<unsigned int> calib_region_hit_map;
//---------------------loop on all patterns-------------------------------------
  MuonCalibEvent::MCPVecCit pat_it = event->patternBegin();
  MuonCalibEvent::MCPVecCit pat_it_end = event->patternEnd();
  for( ;pat_it!=pat_it_end; ++pat_it) {
//--------------------loop on all segemnts--------------------------------------
    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it = (*pat_it)->muonSegBegin();
    MuonCalibPattern::MuonSegmentVec::const_iterator seg_it_end = (*pat_it)->muonSegEnd();
    for ( ; seg_it!=seg_it_end; ++seg_it) {
      MuonCalibSegment *seg = *seg_it;
      if(m_select_seg_author>0 && seg->author()>0)
	if (m_select_seg_author != seg->author())
	  continue;
      //check count hits in the calibration region and remove others
      calib_region_hit_map.clear();
      bool reject(false);
      int count(0);
      for(MuonCalibSegment::MdtHitCit hit_it=seg->mdtHOTBegin(); hit_it!=seg->mdtHOTEnd(); hit_it++) {
	unsigned int is_in_region(static_cast<unsigned int>(!m_reg_sel_svc->isInRegion((*hit_it)->identify())));// || m_suppressed_tubes.find((*hit_it)->identify())!=m_suppressed_tubes.end()));
	if(is_in_region!=0 && m_exclusive_segments) {
	  reject=true;
	  break;
	}
	if((*hit_it)->adcCount()<m_adc_cut) is_in_region=1;
	calib_region_hit_map.push_back(is_in_region);
	if(is_in_region==0) count++;
      }
      if(reject) continue;
      if(count<m_min_hits || (count>m_max_hits && m_max_hits>0)) continue;
      if(count==0) continue;
      seg->refineMdtSelection(calib_region_hit_map);
      NtupleStationId id(seg->mdtHOT()[0]->identify());
      id.SetMultilayer(0);
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
      if(stored->chi2() > seg->chi2()) {
	segments[id] = seg;
      }
    }
  }
}  //end SegmentRegionSelector::prepareSegments

} // namespace Muoncalib
