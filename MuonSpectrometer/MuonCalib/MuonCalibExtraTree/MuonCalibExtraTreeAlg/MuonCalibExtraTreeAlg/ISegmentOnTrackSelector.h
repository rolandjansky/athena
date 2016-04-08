/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_ISegmentOnTrackSelector_h
#define MuonCalib_ISegmentOnTrackSelector_h

#include "GaudiKernel/IAlgTool.h"
#include "vector"
#include "set"

namespace Trk {
  class Track;
}

namespace MuonCalib {

static const InterfaceID IID_ISegmentOnTrackSelector("MuonCalib::ISegmentOnTrackSelector", 1, 0);
class ISegmentOnTrackSelector: virtual public IAlgTool {
 public:
  virtual std::vector<unsigned int> SegmentIndices(const Trk::Track &track, const std::set<int> segment_authors) const=0;
  virtual StatusCode beginEvent()=0;
  static const InterfaceID &interfaceID(void) {
    return IID_ISegmentOnTrackSelector;
  };	
};

}  //namespace MuonCalib

#endif
