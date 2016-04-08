/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_TrackTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRTTRACKTIMINGTOOL_H
#define ITRTTRACKTIMINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "TrkToolInterfaces/ITrackTimingTool.h"

namespace InDet
{
  static const InterfaceID IID_ITRT_TrackTimingTool("ITRT_TrackTimingTool", 1, 0);

  /** @class ITRT_TrackTimingTool
    @brief Interface for tools that calculate the track time - only a wrapper such that it can be accessed in the same way as calo and Muon timing information for muons
    @brief wraps the tool from InDetCosmicsEventPhase that actually calculates the time

    @author Sasa Fratina
  */

  class ITRT_TrackTimingTool : virtual public IAlgTool,
                               virtual public Trk::ITrackTimingTool {
  public:
    static const InterfaceID& interfaceID( );

    // all relevant methods are already defined in ITrackTimingTool

  };

  inline const InterfaceID& ITRT_TrackTimingTool::interfaceID(){
    return IID_ITRT_TrackTimingTool;
  }
}

#endif

