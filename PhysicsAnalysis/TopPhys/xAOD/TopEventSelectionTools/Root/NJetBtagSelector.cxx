/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NJetBtagSelector.h"
#include "TopConfiguration/TopConfig.h"

namespace top {

  NJetBtagSelector::NJetBtagSelector(const std::string& params,std::shared_ptr<top::TopConfig> config) :
    SignValueSelector("JET_N_BTAG", params, true, true, ":","_","MV2c10") {
    checkMultiplicityIsInteger();
    // check if the provided btag WP is really available - need to replace : with _ to match the naming of variables
    if (std::find(config->bTagWP_available().begin(), config->bTagWP_available().end(), valueStringDelimReplace()) == config->bTagWP_available().end()) {
      std::cout << "NJetBtagSelector is confused\n";
      std::cout << "B-tagging working point " << valueString() << " doesn't seem to be supported.\n";
      std::cout << "Please note that you should provide the argument as ==> bTagAlgorithm:bTagWP now. \n ";
      std::cout << "Please provide a real one! Did you specified it in the \"BTaggingWP\" field of your cutfile?\n";
      exit(1);
    }

  }
  
  bool NJetBtagSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr){
      if(!jetPtr->isAvailable<char>("isbtagged_"+valueStringDelimReplace())) {
        std::cout << "NJetBtagSelector is confused\n";
        std::cout << "A jet doesn't have the necessady decoration called \"isbtagged_"+valueStringDelimReplace()+"\".\n";
        std::cout << "Exiting...\n";
        exit(1);
      }
      return jetPtr->auxdataConst<char>("isbtagged_"+valueStringDelimReplace());
    };
    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
  }
  
}
