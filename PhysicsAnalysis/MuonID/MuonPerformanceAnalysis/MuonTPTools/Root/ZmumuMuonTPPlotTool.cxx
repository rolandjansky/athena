/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/ZmumuMuonTPPlotTool.h"
#include "MuonPerformanceHistUtils/ZmumuBasicTPEfficiencyPlots.h"
#include "MuonPerformanceHistUtils/ZmumuTagProbeDileptonPlots.h"
#include "MuonPerformanceHistUtils/ZmumuTPEventCutFlowPlots.h"

ZmumuMuonTPPlotTool::ZmumuMuonTPPlotTool(std::string name)
	  :  AsgTool(name),MuonTPPlotTool(name){
	declareProperty("doTrkValidPlots",m_do_Valid=false);
	declareProperty("doBasicKinematicPlots",m_do_TPBasic=true);
    declareProperty("doDileptonPlots",m_do_DiLepton=true);
}

std::vector<MuonTPEfficiencyPlotBase*> ZmumuMuonTPPlotTool::AddPlots(std::string sDir, bool isMatched){

	std::vector<MuonTPEfficiencyPlotBase*> out (0);
	if (m_do_Valid) out.push_back( new MuonTPEfficiencyPlots(0, sDir, isMatched));
	if (m_do_TPBasic) out.push_back( new ZmumuBasicTPEfficiencyPlots(0, sDir, isMatched));
    if (m_do_DiLepton) out.push_back( new ZmumuTagProbeDileptonPlots(0, sDir, isMatched));
    // closure plots
    if (m_do_TPBasic) out.push_back( new ZmumuBasicTPEfficiencyPlots(0, sDir, isMatched,true));
    if (m_do_DiLepton) out.push_back( new ZmumuTagProbeDileptonPlots(0, sDir, isMatched,true));
	return out;

}

std::vector<MuonTPCutFlowBase*> ZmumuMuonTPPlotTool::AddCutFlowPlots(std::string sDir){

    std::vector<MuonTPCutFlowBase*> out (0);
    out.push_back (new ZmumuTPEventCutFlowPlots(0,sDir));
    return out;

}
