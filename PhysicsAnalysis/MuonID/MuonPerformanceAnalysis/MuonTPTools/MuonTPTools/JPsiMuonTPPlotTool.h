/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPPlots.h
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#ifndef JPSIMUONTPPLOTTOOL_H_
#define JPSIMUONTPPLOTTOOL_H_
#include "MuonTPTools/IMuonTPPlotTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class JPsiMuonTPPlotTool:
		public MuonTPPlotTool {
		ASG_TOOL_CLASS(JPsiMuonTPPlotTool, IMuonTPPlotTool)

public:
		JPsiMuonTPPlotTool(std::string name);

		virtual std::vector<MuonTPEfficiencyPlotBase*> AddPlots(std::string sDir, bool isMatched);

protected:

};

#endif /* JPSIMUONTPPLOTTOOL_H_ */
