/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPPlotTool.cxx
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#include "MuonTPTools/JPsiMuonTPPlotTool.h"

JPsiMuonTPPlotTool::JPsiMuonTPPlotTool(std::string name)
	  :  AsgTool(name),MuonTPPlotTool(name){
}

std::vector<MuonTPEfficiencyPlotBase*> JPsiMuonTPPlotTool::AddPlots(std::string sDir, bool isMatched){

	std::vector<MuonTPEfficiencyPlotBase*> out (0);
	out.push_back( new MuonTPEfficiencyPlots(0, sDir, isMatched));
	return out;

}
