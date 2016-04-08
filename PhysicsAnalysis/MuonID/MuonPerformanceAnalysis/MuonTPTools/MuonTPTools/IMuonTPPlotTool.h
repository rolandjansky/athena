/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IMuonTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef IMUONTPPLOTTOOL_H
#define IMUONTPPLOTTOOL_H


#include "AsgTools/IAsgTool.h"
#include "MuonPerformanceHistUtils/Probe.h"
#include "AsgTools/ToolHandleArray.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"

class IMuonTPSelectionTool;
class IMuonTPEfficiencyTool;

class IMuonTPPlotTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPPlots)

public:

	// used to initialize the required histos
	 virtual StatusCode RegisterPlots (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools) = 0;

	 // fill the histos
	 virtual void fill(Probe& probe, ToolHandle <IMuonTPSelectionTool> & tp_tool, ToolHandle <IMuonTPEfficiencyTool> & eff_tool) = 0;

	 // fill the cut flow
	 virtual void fillCutFlow (std::string stage, double weight, ToolHandle <IMuonTPSelectionTool> & tp_tool) = 0;

	  /// retrieve booked histograms
	  virtual std::vector<HistData> retrieveBookedHistograms() = 0;
      virtual std::vector<std::pair <TGraph*,  std::string> > retrieveBookedGraphs() = 0;
	  virtual void CalcEff () = 0;
};


#endif /* IMUONTPPLOTTOOL_H */
