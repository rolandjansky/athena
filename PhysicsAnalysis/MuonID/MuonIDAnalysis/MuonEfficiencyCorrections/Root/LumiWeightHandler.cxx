/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * LumiWeightHandler.cxx
 *
 *  Created on: Jul 10, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/LumiWeightHandler.h"
#include <cstdlib>

namespace CP {

LumiWeightHandler::LumiWeightHandler(){}
LumiWeightHandler::~LumiWeightHandler(){}

bool LumiWeightHandler::LoadLumiFromInput(LumiWeightHandler::lumimap & map, std::string file, ToolHandle<IPileupReweightingTool> &prw_tool ){

    // start by clearing anything in the weight map so far
    // in case we initialize multiple times for some reason...
    map.clear();


    // read what is provided in the file
    TFile* fin = TFile::Open(file.c_str(),"READ");
    if (!fin) {
        Error("MuonEfficiencyScaleFactors","unable to open %s for lumi input",file.c_str());
        return false;
    }
    TTree* intree=0;
    fin->GetObject("LumiData",intree);

    // if no Lumi tree is found, we assume that the SF are not binned in time
    if (!intree){
        map["All"] = 1;
        //            Error("MuonEfficiencyScaleFactors","unable to read lumi data tree from %s",file.c_str());
        fin->Close();
        return true;
    }
    std::string *period = 0;
    unsigned int firstRun = 0;
    unsigned int lastRun = 0;
    intree->SetBranchAddress("Period",&period);
    intree->SetBranchAddress("FirstRun",&firstRun);
    intree->SetBranchAddress("LastRun",&lastRun);

    for (int t = 0; intree->GetEntry(t);t++){
        if (!prw_tool.empty()) map[*period] = prw_tool->GetIntegratedLumi(firstRun,lastRun);
    }

    fin->Close();
    return true;
}


} /* namespace CP */
