/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTrigTPPlotTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc / a.m.
 */

#include "MuonTPTools/MuonTrigTPPlotTool.h"
//#include "MuonPerformanceHistUtils/ZmumuBasicTPEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/MuonTrigTPEfficiencyPlots.h"
//#include "MuonPerformanceHistUtils/ZmumuTagProbeDileptonPlots.h"
#include "MuonPerformanceHistUtils/ZmumuTPEventCutFlowPlots.h"

MuonTrigTPPlotTool::MuonTrigTPPlotTool(std::string name)
	  :  AsgTool(name),MuonTPPlotTool(name)
{
    declareProperty("doTrkValidPlots",       m_do_Valid   = false);
    declareProperty("doBasicKinematicPlots", m_do_TPBasic = true);
    declareProperty("ApplyScaleFactors",     m_apply_SF   = false);
}



//--------------------------------------------------------------------------------
std::vector<MuonTPEfficiencyPlotBase*> MuonTrigTPPlotTool::AddPlots(std::string sDir, bool isMatched)
{
    std::vector<MuonTPEfficiencyPlotBase*> out (0);
    if (m_do_Valid)    out.push_back( new MuonTPEfficiencyPlots(    0, sDir, isMatched));
    if (m_do_TPBasic)  out.push_back( new MuonTrigTPEfficiencyPlots(0, sDir, isMatched,m_apply_SF));
    return out;
}



//--------------------------------------------------------------------------------
std::vector<MuonTPCutFlowBase*> MuonTrigTPPlotTool::AddCutFlowPlots(std::string sDir)
{
    std::vector<MuonTPCutFlowBase*> out (0);
    out.push_back (new ZmumuTPEventCutFlowPlots(0,sDir));
    return out;
}
