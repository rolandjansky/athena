/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_TRACK_FITTING_TOOL_H__
#define __ITRIG_L2_TRACK_FITTING_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace Trk {
    class TrkBaseNode;
    class TrkTrackState;
}

static const InterfaceID IID_ITrigL2TrackFittingTool("ITrigL2TrackFittingTool",1,0);


  /** @class ITrigL2TrackFittingTool

      provides the abstract interface for track fitting tools used by TrigInDetTrackFitter

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigL2TrackFittingTool : virtual public IAlgTool { 

  public:
    /** other standard AlgTool methods */

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigL2TrackFittingTool; }  	

    virtual Trk::TrkTrackState* fit(Trk::TrkTrackState*, std::vector<Trk::TrkBaseNode*>&, bool) = 0;
  };

#endif
