/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NJetBtagSelector.h"
#include "TopConfiguration/TopConfig.h"

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  NJetBtagSelector::NJetBtagSelector(const std::string& params, std::shared_ptr<top::TopConfig> config,
                                     bool doTrackJets) :
    SignValueSelector((doTrackJets ? "TJET_N_BTAG" : "JET_N_BTAG"), params, true, true, ":", "_", "DL1r"),
    m_doTrackJets(doTrackJets) {
    checkMultiplicityIsInteger();

    // check if the provided btag WP is really available - need to replace : with _ to match the naming of variables
    bool bTagWP_exists = false;
    if (m_doTrackJets) bTagWP_exists =
        (std::find(config->bTagWP_trkJet().begin(), config->bTagWP_trkJet().end(), valueStringDelimReplace()) != config->bTagWP_trkJet().end());
    else bTagWP_exists =
        (std::find(config->bTagWP().begin(), config->bTagWP().end(), valueStringDelimReplace()) != config->bTagWP().end());

    if (!bTagWP_exists) {
      ATH_MSG_ERROR("NJetBtagSelector is confused\n"
          << "B-tagging working point " << valueString() << " doesn't seem to be configured.\n"
          << "Did you provide the argument as  bTagAlgorithm:bTagWP ? \n "
          << "Did you specified it in the \"BTaggingCaloWP\" or \"BTaggingTrackJetWP\" field of your cutfile?\n");
      throw std::runtime_error("NJetBtagSelector: Invalid btagging selector WP: " + name());
    }
  }

  bool NJetBtagSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr) {
                  if (!jetPtr->isAvailable<char>("isbtagged_" + valueStringDelimReplace())) {
                    throw std::runtime_error("NJetBtagSelector: Jet doesn't have decoration \"isbtagged_" +
                                             valueStringDelimReplace() + "\"");
                  }
                  return jetPtr->auxdataConst<char>("isbtagged_" + valueStringDelimReplace());
                };
    int count = 0;

    if (m_doTrackJets) {
      count = std::count_if(event.m_trackJets.begin(), event.m_trackJets.end(), func);
    } else {
      count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    }
    return checkInt(count, multiplicity());
  }
}
