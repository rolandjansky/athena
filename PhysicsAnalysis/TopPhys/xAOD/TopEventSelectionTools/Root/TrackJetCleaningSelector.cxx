/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TrackJetCleaningSelector.h"
#include "TopEvent/EventTools.h"
#include "xAODJet/JetContainer.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  TrackJetCleaningSelector::TrackJetCleaningSelector(std::shared_ptr<top::TopConfig> config) :
    m_config(config) {
  }

  bool TrackJetCleaningSelector::apply(const top::Event& event) const {
    // Selection is applied only if Variable-R track jets are used
    if (m_config->sgKeyTrackJets() == "AntiKtVR30Rmax4Rmin02PV0TrackJets") {
      static SG::AuxElement::Accessor<char> acc_passDRcut("passDRcut");
      for (const xAOD::Jet* jet : event.m_trackJets) {
        top::check(acc_passDRcut.isAvailable(
                     *jet), "passDRcut info not found in variable-R track jet. Needed for cleaning algorithm.");
        if (!acc_passDRcut(*jet)) return false;
      }
    }
    return true;
  }

  std::string TrackJetCleaningSelector::name() const {
    return "TRACKJETCLEAN";
  }
}
