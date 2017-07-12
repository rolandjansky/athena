/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/TopEventReconstructionToolsLoader.h"
#include "TopEventReconstructionTools/Sonnenschein.h"
#include "TopEventReconstructionTools/KLFitterRun.h"
#include "TopEventReconstructionTools/PseudoTopRecoRun.h"
#include "TopConfiguration/TopConfig.h"

#include "TopAnalysis/Tools.h" //for the loadLibraries function

namespace top {

top::EventSelectorBase* TopEventReconstructionToolsLoader::initTool(const std::string& /*name*/, const std::string& line, TFile* /*outputFile*/,std::shared_ptr<top::TopConfig> config, EL::Worker* /*wk*/) {
    //get the first bit of the string and store it in toolname
    std::istringstream iss(line);
    std::string toolname;
    getline(iss, toolname, ' ');

    //any parameters?
    std::string param;
    if (line.size() > toolname.size())
        param = line.substr(toolname.size() + 1);

    if (toolname == "RECO::SONNENSCHEIN")
        return new top::Sonnenschein();

    else if (toolname == "RECO::KLFITTERRUN") {
      loadLibraries("libKLFitter");
      return new top::KLFitterRun(param,config);
    }
    
    else if (toolname == "RECO::PSEUDOTOP"){
      return new top::PseudoTopRecoRun(param, config);
    }


    return nullptr;
}

}
