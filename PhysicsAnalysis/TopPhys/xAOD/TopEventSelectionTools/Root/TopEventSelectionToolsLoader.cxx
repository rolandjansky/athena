/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/TopEventSelectionToolsLoader.h"


#include "TopEventSelectionTools/ExamplePlots.h"
#include "TopEventSelectionTools/JetFlavorPlots.h"
#include "TopEventSelectionTools/FakesMMConfigs.h"
#include "TopEventSelectionTools/GRLSelector.h"
#include "TopEventSelectionTools/GoodCaloSelector.h"
#include "TopEventSelectionTools/HTSelector.h"
#include "TopEventSelectionTools/IP3DSV1Selector.h"
#include "TopEventSelectionTools/InitialSelector.h"
#include "TopEventSelectionTools/JetCleaningSelector.h"
#include "TopEventSelectionTools/JetNGhostSelector.h"
#include "TopEventSelectionTools/KLFitterSelector.h"
#include "TopEventSelectionTools/METMWTSelector.h"
#include "TopEventSelectionTools/METSelector.h"
#include "TopEventSelectionTools/MLLSelector.h"
#include "TopEventSelectionTools/MLLWindowSelector.h"
#include "TopEventSelectionTools/MV1Selector.h"
#include "TopEventSelectionTools/MV2c20Selector.h"
#include "TopEventSelectionTools/MV2c10Selector.h"
#include "TopEventSelectionTools/MWTSelector.h"
#include "TopEventSelectionTools/NElectronNMuonSelector.h"
#include "TopEventSelectionTools/NElectronNMuonTightSelector.h"
#include "TopEventSelectionTools/NElectronSelector.h"
#include "TopEventSelectionTools/NElectronTightSelector.h"
#include "TopEventSelectionTools/NJetSelector.h"
#include "TopEventSelectionTools/NJetBtagSelector.h"
#include "TopEventSelectionTools/NMuonSelector.h"
#include "TopEventSelectionTools/NoBadMuonSelector.h"
#include "TopEventSelectionTools/NMuonTightSelector.h"
#include "TopEventSelectionTools/NPhotonSelector.h"
#include "TopEventSelectionTools/NTauSelector.h"
#include "TopEventSelectionTools/NTauTightSelector.h"
#include "TopEventSelectionTools/OSLeptonSelector.h"
#include "TopEventSelectionTools/OSLeptonTightSelector.h"
#include "TopEventSelectionTools/ParticleLevelSelector.h"
#include "TopEventSelectionTools/PrimaryVertexSelector.h"
#include "TopEventSelectionTools/PrintEventSelector.h"
#include "TopEventSelectionTools/RecoLevelSelector.h"
#include "TopEventSelectionTools/SSLeptonSelector.h"
#include "TopEventSelectionTools/SSLeptonTightSelector.h"
#include "TopEventSelectionTools/SaveEventSelector.h"
#include "TopEventSelectionTools/TrigDecisionSelector.h"
#include "TopEventSelectionTools/TrigDecisionLooseSelector.h"
#include "TopEventSelectionTools/TrigDecisionTightSelector.h"
#include "TopEventSelectionTools/TrigMatchSelector.h"
#include "TopEventSelectionTools/RunNumberSelector.h"
#include "TopEventSelectionTools/NLargeJetSelector.h"

#include "TopConfiguration/TopConfig.h"

#include <iostream>

#include "TFile.h"

namespace top {

  top::EventSelectorBase* TopEventSelectionToolsLoader::initTool(const std::string& name, const std::string& line, TFile* outputFile,std::shared_ptr<top::TopConfig> config, EL::Worker* wk) {
    //get the first bit of the string and store it in toolname
    std::istringstream iss(line);
    std::string toolname;
    getline(iss, toolname, ' ');

    //any parameters?
    std::string param;
    if (line.size() > toolname.size())
        param = line.substr(toolname.size() + 1);

    if (toolname == "JET_N")
        return new top::NJetSelector(param);
    else if (toolname == "JET_N_BTAG")
        return new top::NJetBtagSelector(param,config);
    else if (toolname == "LJET_N")
        return new top::NLargeJetSelector(param);
    else if (toolname == "MV1_N")
        return new top::MV1Selector(param);
    else if (toolname == "MV2C20_N")
        return new top::MV2c20Selector(param);
    else if (toolname == "MV2C10_N")
        return new top::MV2c10Selector(param);
    else if (toolname == "IP3DSV1_N")
        return new top::IP3DSV1Selector(param);
    else if (toolname == "EL_N_OR_MU_N")
        return new top::NElectronNMuonSelector(param);
    else if (toolname == "EL_N_OR_MU_N_TIGHT")
        return new top::NElectronNMuonTightSelector(param);
    else if (toolname == "EL_N")
        return new top::NElectronSelector(param);
    else if (toolname == "EL_N_TIGHT")
        return new top::NElectronTightSelector(param);
    else if (toolname == "MU_N")
        return new top::NMuonSelector(param);
    else if (toolname == "MU_N_TIGHT")
        return new top::NMuonTightSelector(param);
    else if (toolname == "PH_N")
      return new top::NPhotonSelector(param);
    else if (toolname == "TAU_N")
        return new top::NTauSelector(param);
    else if (toolname == "TAU_N_TIGHT")
        return new top::NTauTightSelector(param);
    else if (toolname == "MET+MWT")
        return new top::METMWTSelector(param);
    else if (toolname == "MET")
        return new top::METSelector(param);
    else if (toolname == "MWT")
        return new top::MWTSelector(param);
    else if (toolname == "MLLWIN")
        return new top::MLLWindow(param);
    else if (toolname == "MLL")
        return new top::MLLSelector(param);
    else if (toolname == "HT")
        return new top::HTSelector(param);
    else if (toolname == "NOBADMUON")
        return new top::NoBadMuonSelector(config);
    else if (toolname == "OS")
        return new top::OSLeptonSelector();
    else if (toolname == "OS_TIGHT")
        return new top::OSLeptonTightSelector();
    else if (toolname == "SS")
        return new top::SSLeptonSelector();
    else if (toolname == "SS_TIGHT")
        return new top::SSLeptonTightSelector();
    else if (toolname == "INITIAL")
        return new top::InitialSelector;
    else if (toolname == "SAVE")
        return new top::SaveEventSelector;
    else if (toolname == "PRINT")
        return new top::PrintEventSelector;
    else if (toolname == "EXAMPLEPLOTS")
      return new top::ExamplePlots(name, outputFile, wk, config);
    else if (toolname == "JETFLAVORPLOTS")
      return new top::JetFlavorPlots(name, outputFile, param, config, wk);
    else if (toolname == "GRL")
        return new top::GRLSelector();
    else if (toolname == "TRIGDEC")
        return new top::TrigDecisionSelector(name,config);
    else if (toolname == "TRIGDEC_LOOSE")
        return new top::TrigDecisionLooseSelector(name,config);
    else if (toolname == "TRIGDEC_TIGHT")
        return new top::TrigDecisionTightSelector(name,config);
    else if (toolname == "TRIGMATCH")
        return new top::TrigMatchSelector(name,config);
    else if (toolname == "JETCLEAN")
        return new top::JetCleaningSelector(param,config);
    else if (toolname == "KLFITTER")
        return new top::KLFitterSelector(param);
    else if (toolname == "JET_N_GHOST")
        return new top::JetNGhostSelector(param);
    else if (toolname == "GOODCALO")
        return new top::GoodCaloSelector();
    else if (toolname == "PRIVTX")
        return new top::PrimaryVertexSelector();
    else if (toolname == "RECO_LEVEL")
        return new top::RecoLevelSelector();
    else if (toolname == "PARTICLE_LEVEL")
        return new top::ParticleLevelSelector();
    else if (toolname == "FAKESMMCONFIGS")
        return new top::FakesMMConfigs(param);
    else if (toolname == "RUN_NUMBER")
        return new top::RunNumberSelector(param,config);


    return nullptr;
  }

}
