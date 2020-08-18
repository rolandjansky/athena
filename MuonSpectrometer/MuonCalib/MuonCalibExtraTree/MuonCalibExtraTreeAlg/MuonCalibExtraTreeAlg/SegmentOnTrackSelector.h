/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentOnTrackSelector_h
#define MuonCalib_SegmentOnTrackSelector_h

#include "MuonCalibExtraTreeAlg/ISegmentOnTrackSelector.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include <string>
#include <set>
#include <map>
#include <vector>

namespace Trk {
  class Track;
}

namespace MuonCalib {

class MuonFixedId;
class MuonCalibSegment;

class SegmentOnTrackSelector: public AthAlgTool, virtual public ISegmentOnTrackSelector {
 public:
  
  SegmentOnTrackSelector(const std::string &type,const std::string &name,const IInterface* parent);	
  //tool interface functions
  StatusCode initialize();
		
  //call once at begining of event	
  StatusCode beginEvent();
		
  //get list of segment indices on track
  std::vector<unsigned int> SegmentIndices(const Trk::Track &track, const std::set<int> segment_authors) const;
		
 private:
  std::string m_pattern_location;
  int m_min_hits_on_track;
  int m_max_hits_not_on_track;
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };
  ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool{this,"IdToFixedIdTool","MuonCalib::IdToFixedIdTool"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
		
  //internal data structures
  std::vector<std::set<MuonFixedId> > m_segment_hits;
  std::vector<int> m_authors;                          //list of all authors in all segments
  inline void storeSegment(const MuonCalibSegment &seg);
  inline void getIdentifiers(const Trk::Track &tk, std::set<MuonFixedId> &ids) const;
};

}//namespace MuonCalib 

#endif 
