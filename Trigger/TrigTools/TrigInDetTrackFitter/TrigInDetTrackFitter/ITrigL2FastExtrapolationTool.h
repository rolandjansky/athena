/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_FAST_EXTRAPOLATION_TOOL_H__
#define __ITRIG_L2_FAST_EXTRAPOLATION_TOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace Trk {
    class TrkPlanarSurface;
    class TrkTrackState;
}

static const InterfaceID IID_ITrigL2FastExtrapolationTool("ITrigL2FastExtrapolationTool",1,0);


  /** @class ITrigL2FastExtrapolationTool

      provides the abstract interface for a track extrapolation tool used by TrigInDetTrackFitter

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigL2FastExtrapolationTool : virtual public IAlgTool {

  public:
    /** other standard AlgTool methods */

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigL2FastExtrapolationTool; }  	
    virtual Trk::TrkTrackState* extrapolate(Trk::TrkTrackState*,
					    Trk::TrkPlanarSurface*,
					    Trk::TrkPlanarSurface*, bool) = 0;
  };

#endif
