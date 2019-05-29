/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//abstract interface

#ifndef __ITRIG_FTK_SECTOR_MATCH_TOOL_H__
#define __ITRIG_FTK_SECTOR_MATCH_TOOL_H__

#include "GaudiKernel/IAlgTool.h"

#include <unordered_map>
#include <vector>

class FTKTruthTrajectory;

static const InterfaceID IID_ITrigFTKSectorMatchTool("ITrigFTKSectorMatchTool",1,0);

/** @class ITrigFTKSectorMatchTool

   Interface for the matching of a truth FTK track to an FTK sector
*/

class ITrigFTKSectorMatchTool : virtual public IAlgTool 
{ 
public:
  static const InterfaceID& interfaceID () //!< the Tool's interface
  { return IID_ITrigFTKSectorMatchTool; }  	

  // Match an FTKTruthTrajectory to all possible FTK sectors, and
  // fill the mapping of hit index to plane for each sector, as
  // well as the region and sector IDs for each sector.
  // Return true if at least one successful match could be made.
  virtual bool match(const FTKTruthTrajectory &traj,
    std::vector<std::unordered_map<int, int> >& sectorHitToPlaneMap,
    std::vector<int>& regions,
    std::vector<int>& sectors) const = 0;
};

#endif
