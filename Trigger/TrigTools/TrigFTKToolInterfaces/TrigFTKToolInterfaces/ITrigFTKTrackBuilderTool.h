/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __ITRIG_FTK_TRACK_BUILDER_TOOL_H__
#define __ITRIG_FTK_TRACK_BUILDER_TOOL_H__

#include "GaudiKernel/IAlgTool.h"

#include <memory>
#include <unordered_map>
#include <vector>


class FTKTrack;
class FTKTruthTrajectory;

static const InterfaceID IID_ITrigFTKTrackBuilderTool("ITrigFTKTrackBuilderTool",1,0);

/** @class ITrigFTKTrackBuilderTool

    Interface for FTK truth trajectory conversion into an FTK track
*/

class ITrigFTKTrackBuilderTool : virtual public IAlgTool
{
public:
  /** other standard AlgTool methods */

  static const InterfaceID& interfaceID ()   //!< the Tool's interface
  {  return IID_ITrigFTKTrackBuilderTool; }

  // Create the FTK track from the truth trajectory best matching to any
  // of the sectors
  virtual std::unique_ptr<FTKTrack> createFTKTrack(const FTKTruthTrajectory& traj,
    const std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    const std::vector<int>& regions,
    const std::vector<int>& sectors) const = 0;
};

#endif
