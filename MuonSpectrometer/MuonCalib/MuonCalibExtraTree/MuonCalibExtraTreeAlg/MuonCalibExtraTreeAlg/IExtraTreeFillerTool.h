/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_IExtraTreeFillerTool_h
#define MuonCalib_IExtraTreeFillerTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


static const InterfaceID IID_MuonCalibExtraTreeAlg_IExtraTreeFillerTool("IExtraTreeFillerTool",1,0);

namespace MuonCalib {

class MuonCalibTrack_EBranch;
class MuonCalibHit_EBranch;
class MuonCalibTrackSegmentBranch;
class ISegmentOnTrackSelector;

class IExtraTreeFillerTool: virtual public IAlgTool {
 public:	
  inline IExtraTreeFillerTool(): m_trackBranch(NULL), m_hitBranch(NULL),  m_trackSegmentBranch(NULL), m_segmentOnTrackSelector(NULL) {
  }
		
  virtual ~IExtraTreeFillerTool() {};
		
  inline static const InterfaceID& interfaceID() {
    return IID_MuonCalibExtraTreeAlg_IExtraTreeFillerTool;
  }
		
  virtual StatusCode writeTracks(unsigned int &index)=0;
		
  inline void SetBranches(MuonCalibTrack_EBranch *trackBranch, MuonCalibHit_EBranch *hitBranch, MuonCalibTrackSegmentBranch *trackSegmentBranch, ToolHandle<ISegmentOnTrackSelector> &segment_on_track_selector) {
    m_trackBranch = trackBranch;
    m_hitBranch = hitBranch;
    m_trackSegmentBranch = trackSegmentBranch;
    m_segmentOnTrackSelector = &segment_on_track_selector;
  }
	
 protected:
  MuonCalibTrack_EBranch      *m_trackBranch;
  MuonCalibHit_EBranch        *m_hitBranch;
  MuonCalibTrackSegmentBranch *m_trackSegmentBranch;
  ToolHandle<ISegmentOnTrackSelector> *m_segmentOnTrackSelector;
};

} //namespace MuonCalib
#endif
