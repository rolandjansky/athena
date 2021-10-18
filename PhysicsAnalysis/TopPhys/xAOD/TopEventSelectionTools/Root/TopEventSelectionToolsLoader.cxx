/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "AsgMessaging/MessageCheck.h"

#include "TopEventSelectionTools/TopEventSelectionToolsLoader.h"


#include "TopEventSelectionTools/ExamplePlots.h"
#include "TopEventSelectionTools/JetFlavorPlots.h"
#include "TopEventSelectionTools/JetResponsePlots.h"
#include "TopEventSelectionTools/JetFtagEffPlots.h"
#include "TopEventSelectionTools/FakesMMConfigs.h"
#include "TopEventSelectionTools/GlobalTrigDecisionSelector.h"
#include "TopEventSelectionTools/GlobalTrigMatchSelector.h"
#include "TopEventSelectionTools/GRLSelector.h"
#include "TopEventSelectionTools/GoodCaloSelector.h"
#include "TopEventSelectionTools/HTSelector.h"
#include "TopEventSelectionTools/InitialSelector.h"
#include "TopEventSelectionTools/JetCleaningSelector.h"
#include "TopEventSelectionTools/TrackJetCleaningSelector.h"
#include "TopEventSelectionTools/JetNGhostSelector.h"
#include "TopEventSelectionTools/KLFitterSelector.h"
#include "TopEventSelectionTools/METMWTSelector.h"
#include "TopEventSelectionTools/METSelector.h"
#include "TopEventSelectionTools/MLLSelector.h"
#include "TopEventSelectionTools/MLLWindowSelector.h"
#include "TopEventSelectionTools/MV2c10Selector.h"
#include "TopEventSelectionTools/MWTSelector.h"
#include "TopEventSelectionTools/NElectronNMuonSelector.h"
#include "TopEventSelectionTools/NElectronNMuonTightSelector.h"
#include "TopEventSelectionTools/NElectronSelector.h"
#include "TopEventSelectionTools/NElectronTightSelector.h"
#include "TopEventSelectionTools/NFwdElectronSelector.h"
#include "TopEventSelectionTools/NFwdElectronTightSelector.h"
#include "TopEventSelectionTools/NJetSelector.h"
#include "TopEventSelectionTools/NJetBtagSelector.h"
#include "TopEventSelectionTools/NJetElectronSelector.h"
#include "TopEventSelectionTools/NTruthTopWSelector.h"
#include "TopEventSelectionTools/NMuonSelector.h"
#include "TopEventSelectionTools/NSoftMuonSelector.h"
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
#include "TopEventSelectionTools/NRCJetSelector.h"
#include "TopEventSelectionTools/NVarRCJetSelector.h"

#include "TopConfiguration/TopConfig.h"

#include <iostream>

#include "TFile.h"

namespace top {
  top::EventSelectorBase* TopEventSelectionToolsLoader::initTool(const std::string& name, const std::string& line,
                                                                 TFile* outputFile,
                                                                 std::shared_ptr<top::TopConfig> config,
                                                                 EL::Worker* wk) {
    //get the first bit of the string and store it in toolname
    std::istringstream iss(line);
    std::string toolname;
    getline(iss, toolname, ' ');

    //any parameters?
    std::string param;
    if (line.size() > toolname.size()) param = line.substr(toolname.size() + 1);

    if (toolname == "JET_N") return new top::NJetSelector(param);
    else if (toolname == "JET_N_BTAG") return new top::NJetBtagSelector(param, config, false);
    else if (toolname == "TJET_N_BTAG") return new top::NJetBtagSelector(param, config, true);
    else if (toolname == "LJET_N") return new top::NLargeJetSelector(param);
    else if (toolname == "JET_EL_N") return new top::NJetElectronSelector(param, config);
    else if (toolname == "N_Top_W") return new top::NTruthTopWSelector(param);
    else if (toolname == "RCJET_N") return new top::NRCJetSelector(param);
    else if (toolname == "VRCJET_N") {
      std::istringstream is(param);
      std::string name;
      getline(is, name, ' ');
      return new top::NVarRCJetSelector(name, param.substr(name.size() + 1));
    } else if (toolname == "MV2C10_N") return new top::MV2c10Selector(param);
    else if (toolname == "EL_N_OR_MU_N") return new top::NElectronNMuonSelector(param);
    else if (toolname == "EL_N_OR_MU_N_TIGHT") return new top::NElectronNMuonTightSelector(param);
    else if (toolname == "EL_N") return new top::NElectronSelector(param);
    else if (toolname == "EL_N_TIGHT") return new top::NElectronTightSelector(param);
    else if (toolname == "FWDEL_N") return new top::NFwdElectronSelector(param);
    else if (toolname == "FWDEL_N_TIGHT") return new top::NFwdElectronTightSelector(param);
    else if (toolname == "MU_N") return new top::NMuonSelector(param);
    else if (toolname == "SOFTMU_N") return new top::NSoftMuonSelector(param);
    else if (toolname == "MU_N_TIGHT") return new top::NMuonTightSelector(param);
    else if (toolname == "PH_N") return new top::NPhotonSelector(param);
    else if (toolname == "TAU_N") return new top::NTauSelector(param);
    else if (toolname == "TAU_N_TIGHT") return new top::NTauTightSelector(param);
    else if (toolname == "MET+MWT") return new top::METMWTSelector(param);
    else if (toolname == "MET") return new top::METSelector(param);
    else if (toolname == "MWT") return new top::MWTSelector(param);
    else if (toolname == "MLLWIN") return new top::MLLWindow(param);
    else if (toolname == "MLL") return new top::MLLSelector(param);
    else if (toolname == "HT") return new top::HTSelector(param);
    else if (toolname == "NOBADMUON")
    {
       if(!config->useMuons()) 
       {
         asg::msgUserCode::ATH_MSG_ERROR("NOBADMUON selection cannot be used if the muon collection is not defined");
         return 0;
       }
       return new top::NoBadMuonSelector(config);
    }
    else if (toolname == "OS") return new top::OSLeptonSelector();
    else if (toolname == "OS_TIGHT") return new top::OSLeptonTightSelector();
    else if (toolname == "SS") return new top::SSLeptonSelector();
    else if (toolname == "SS_TIGHT") return new top::SSLeptonTightSelector();
    else if (toolname == "INITIAL") return new top::InitialSelector;
    else if (toolname == "SAVE") return new top::SaveEventSelector;
    else if (toolname == "PRINT") return new top::PrintEventSelector;
    else if (toolname == "EXAMPLEPLOTS") return new top::ExamplePlots(name, outputFile, wk, config);
    else if (toolname == "JETFLAVORPLOTS") return new top::JetFlavorPlots(name, outputFile, param, config, wk);
    else if (toolname == "JETRESPONSEPLOTS") return new top::JetResponsePlots(name, outputFile, param, config, wk);
    else if (toolname == "JETFTAGEFFPLOTS") return new top::JetFtagEffPlots(name, outputFile, param, config, wk);
    else if (toolname == "GRL") return new top::GRLSelector();
    else if (toolname == "GTRIGDEC") return new top::GlobalTrigDecisionSelector();
    else if (toolname == "GTRIGMATCH") return new top::GlobalTrigMatchSelector();
    else if (toolname == "TRIGDEC") return new top::TrigDecisionSelector(name, config);
    else if (toolname == "TRIGDEC_LOOSE") return new top::TrigDecisionLooseSelector(name, config);
    else if (toolname == "TRIGDEC_TIGHT") return new top::TrigDecisionTightSelector(name, config);
    else if (toolname == "TRIGMATCH") return new top::TrigMatchSelector(name, config);
    else if (toolname == "JETCLEAN") return new top::JetCleaningSelector(param, config);
    else if (toolname == "TRACKJETCLEAN") return new top::TrackJetCleaningSelector(config);
    else if (toolname == "KLFITTER") return new top::KLFitterSelector(param);
    else if (toolname == "JET_N_GHOST") return new top::JetNGhostSelector(param);
    else if (toolname == "GOODCALO") return new top::GoodCaloSelector();
    else if (toolname == "PRIVTX") return new top::PrimaryVertexSelector();
    else if (toolname == "RECO_LEVEL") return new top::RecoLevelSelector();
    else if (toolname == "PARTICLE_LEVEL") return new top::ParticleLevelSelector();
    else if (toolname == "FAKESMMCONFIGS") return new top::FakesMMConfigs(param);
    else if (toolname == "RUN_NUMBER") return new top::RunNumberSelector(param, config);


    return nullptr;
  }
}
