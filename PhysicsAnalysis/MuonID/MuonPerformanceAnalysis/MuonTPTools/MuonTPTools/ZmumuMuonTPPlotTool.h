/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef ZMUMUMUONTPPLOTTOOL_H_
#define ZMUMUMUONTPPLOTTOOL_H_
#include "MuonTPTools/IMuonTPPlotTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class ZmumuMuonTPPlotTool:
		public MuonTPPlotTool {
		ASG_TOOL_CLASS(ZmumuMuonTPPlotTool, IMuonTPPlotTool)

public:
		ZmumuMuonTPPlotTool(std::string name);

		virtual std::vector<MuonTPEfficiencyPlotBase*> AddPlots(std::string sDir, bool isMatched);
        virtual std::vector<MuonTPCutFlowBase*> AddCutFlowPlots(std::string sDir);

protected:
		bool m_do_TPBasic;
		bool m_do_TPFineEtaPhi;
        bool m_do_DetRegions;
		bool m_do_Valid;
		bool m_do_DiLepton;
        bool m_apply_SF;

};

#endif /* ZMUMUMUONTPPLOTTOOL_H_ */
