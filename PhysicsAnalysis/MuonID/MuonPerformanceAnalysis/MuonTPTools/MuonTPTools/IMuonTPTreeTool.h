/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IMuonTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef IMUONTPTreeTool_H
#define IMUONTPTreeTool_H


#include "AsgTools/IAsgTool.h"
#include "MuonPerformanceHistUtils/Probe.h"
#include "AsgTools/ToolHandleArray.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"
#include "TTree.h"

class IMuonTPSelectionTool;
class IMuonTPEfficiencyTool;

class IMuonTPTreeTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPTreeTool)

    public:

  // used to initialize the required histos
  virtual StatusCode RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools) = 0;

  // fill the histos
  virtual void updateMatch(Probe& probe,ToolHandle <IMuonTPEfficiencyTool> & eff_tool) = 0;
  virtual void ForgetKnownProbes() = 0;
  virtual void fill(Probe& probe,ToolHandle <IMuonTPSelectionTool> & sel_tool) = 0;
  //      virtual void 

  /// retrieve booked histograms
  virtual std::vector< std::pair<TTree*, std::string> > retrieveBookedTrees() = 0;
};


#endif /* IMUONTPTreeTool_H */
