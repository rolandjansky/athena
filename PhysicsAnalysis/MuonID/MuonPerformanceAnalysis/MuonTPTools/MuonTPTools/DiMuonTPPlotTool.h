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

class DiMuonTPPlotTool:
		public MuonTPPlotTool {
		ASG_TOOL_CLASS(DiMuonTPPlotTool, IMuonTPPlotTool)

public:
		DiMuonTPPlotTool(std::string name);

		virtual std::vector<MuonTPEfficiencyPlotBase*> AddPlots(std::string sDir, bool isMatched);
        virtual std::vector<MuonTPCutFlowBase*> AddCutFlowPlots(std::string sDir);

protected:
		bool m_do_TPBasic_JPsi;
        bool m_do_TPBasic_Zmm;
        bool m_do_DiLepton_Zmm;
        bool m_do_DiLepton_JPsi;
		bool m_do_TPFineEtaPhi;
        bool m_do_TriggerPlots;
        bool m_do_DetRegions;
		bool m_do_Valid;
        bool m_apply_SF;

};

#endif /* ZMUMUMUONTPPLOTTOOL_H_ */
