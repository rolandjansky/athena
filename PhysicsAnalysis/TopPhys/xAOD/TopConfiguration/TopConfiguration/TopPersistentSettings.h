/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPCONFIGURATION_TOPPERSISTENTSETTINGS_H
#define ANALYSISTOP_TOPCONFIGURATION_TOPPERSISTENTSETTINGS_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief TopPersistentSettings
 *   We presist this information into the mini-xAOD
 *   It allows us to reconstruct the state of the top-xaod code
 *
 * $Revision: 809568 $
 * $Date: 2017-08-18 14:09:22 +0100 (Fri, 18 Aug 2017) $
 *
 *
 **/

#include "xAODCore/CLASS_DEF.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>

namespace top {
  class TopPersistentSettings {
  public:
    TopPersistentSettings();
    virtual ~TopPersistentSettings() {}

    bool m_isMC;
    bool m_isAFII;
    bool m_applyElectronInJetSubtraction;
    bool m_doOverlapRemovalOnLooseLeptonDef;
    bool m_doKLFitter;
    bool m_doPseudoTop;

    std::string m_sgKeyTopSystematicEvents;
    std::string m_sgKeyTopSystematicEventsLoose;

    std::string m_sgKeyEventInfo;
    std::string m_sgKeyPrimaryVertices;
    std::string m_sgKeyPhotons;
    std::string m_sgKeyElectrons;
    std::string m_sgKeyFwdElectrons;
    std::string m_sgKeyMuons;
    std::string m_sgKeySoftMuons;
    std::string m_sgKeyTaus;
    std::string m_sgKeyJets;
    std::string m_sgKeyLargeRJets;
    std::string m_sgKeyTrackJets;
    std::string m_sgKeyTracks;
    std::string m_sgKeyMissingEt;
    std::string m_sgKeyMissingEtLoose;

    std::string m_electronID;
    std::string m_electronIDLoose;
    std::string m_electronIsolation;
    std::string m_electronIsolationLoose;
    bool m_useElectronChargeIDSelection;
    bool m_useEgammaLeakageCorrection;
    bool m_enablePromptLeptonImprovedVetoStudies;

    std::string m_fwdElectronID;

    std::string m_muon_trigger_SF;
    std::string m_muonQuality;
    std::string m_muonQualityLoose;
    std::string m_muonIsolation;
    std::string m_muonIsolationLoose;

    std::string m_softmuonQuality;

    std::string m_trackQuality;

    std::vector<std::pair<std::string, std::string> > m_chosen_btaggingWP;

    std::map<std::size_t, std::string> m_systSgKeyMapPhotons;
    std::map<std::size_t, std::string> m_systSgKeyMapElectrons;
    std::map<std::size_t, std::string> m_systSgKeyMapFwdElectrons;
    std::map<std::size_t, std::string> m_systSgKeyMapMuons;
    std::map<std::size_t, std::string> m_systSgKeyMapSoftMuons;
    std::map<std::size_t, std::string> m_systSgKeyMapTaus;
    std::map<std::size_t, std::string> m_systSgKeyMapJets;
    std::map<std::size_t, std::string> m_systSgKeyMapJets_electronInJetSubtraction;
    std::map<std::size_t, std::string> m_systSgKeyMapJetsLoose_electronInJetSubtraction;
    std::map<std::size_t, std::string> m_systSgKeyMapLargeRJets;
    std::map<std::size_t, std::string> m_systSgKeyMapTrackJets;
    std::map<std::size_t, std::string> m_systSgKeyMapMissingET;
    std::map<std::size_t, std::string> m_systSgKeyMapMissingETLoose;
    std::map<std::size_t, std::string> m_systSgKeyMapKLFitter;
    std::map<std::size_t, std::string> m_systSgKeyMapKLFitterLoose;
    std::map<std::size_t, std::string> m_systSgKeyMapPseudoTop;
    std::map<std::size_t, std::string> m_systSgKeyMapPseudoTopLoose;
    std::map<std::size_t, std::string> m_systPersistantAllTTreeNames;

    std::map<std::size_t, std::string> m_systDecoKeyMapJetGhostTrack;
    std::map<std::size_t, std::string> m_systSgKeyMapTracks;

    std::vector<std::size_t> m_list_systHashAll;
    std::vector<std::size_t> m_list_systHash_electronInJetSubtraction;

    std::vector<std::string> m_allSelectionNames;

    std::unordered_map<std::string, std::vector<std::string> > m_allTriggers_Tight;
    std::unordered_map<std::string, std::vector<std::string> > m_electronTriggers_Tight;
    std::unordered_map<std::string, std::vector<std::string> > m_muonTriggers_Tight;
    std::unordered_map<std::string, std::vector<std::string> > m_tauTriggers_Tight;

    std::unordered_map<std::string, std::vector<std::string> > m_allTriggers_Loose;
    std::unordered_map<std::string, std::vector<std::string> > m_electronTriggers_Loose;
    std::unordered_map<std::string, std::vector<std::string> > m_muonTriggers_Loose;
    std::unordered_map<std::string, std::vector<std::string> > m_tauTriggers_Loose;
  };
}
// Dictonaries
CLASS_DEF(top::TopPersistentSettings, 117063636, 1)
#endif
