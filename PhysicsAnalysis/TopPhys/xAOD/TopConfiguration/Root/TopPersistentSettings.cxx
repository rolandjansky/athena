/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopConfiguration/TopPersistentSettings.h"

namespace top {
  TopPersistentSettings::TopPersistentSettings() :
    m_isMC(false),
    m_isAFII(false),
    m_applyElectronInJetSubtraction(false),
    m_doOverlapRemovalOnLooseLeptonDef(false),
    m_doKLFitter(false),
    m_doPseudoTop(false),
    m_sgKeyTopSystematicEvents("SetMe"),
    m_sgKeyTopSystematicEventsLoose("SetMe"),

    m_sgKeyEventInfo("SetMe"),
    m_sgKeyPrimaryVertices("SetMe"),
    m_sgKeyPhotons("SetMe"),
    m_sgKeyElectrons("SetMe"),
    m_sgKeyFwdElectrons("SetMe"),
    m_sgKeyMuons("SetMe"),
    m_sgKeySoftMuons("SetMe"),
    m_sgKeyTaus("SetMe"),
    m_sgKeyJets("SetMe"),
    m_sgKeyLargeRJets("SetMe"),
    m_sgKeyTrackJets("SetMe"),
    m_sgKeyTracks("SetMe"),
    m_sgKeyMissingEt("SetMe"),
    m_sgKeyMissingEtLoose("SetMe"),

    m_electronID("SetMe"),
    m_electronIDLoose("SetMe"),
    m_electronIsolation("SetMe"),
    m_electronIsolationLoose("SetMe"),
    m_useElectronChargeIDSelection(false),
    m_useEgammaLeakageCorrection(true),
    m_enablePromptLeptonImprovedVetoStudies(false),

    m_fwdElectronID("SetMe"),

    m_muon_trigger_SF("SetMe"),
    m_muonQuality("SetMe"),
    m_muonQualityLoose("SetMe"),
    m_muonIsolation("SetMe"),
    m_muonIsolationLoose("SetMe"),

    m_softmuonQuality("SetMe"),

    m_trackQuality("SetMe"),

    m_chosen_btaggingWP(),

    m_systSgKeyMapPhotons(),
    m_systSgKeyMapElectrons(),
    m_systSgKeyMapFwdElectrons(),
    m_systSgKeyMapMuons(),
    m_systSgKeyMapSoftMuons(),
    m_systSgKeyMapTaus(),
    m_systSgKeyMapJets(),
    m_systSgKeyMapJets_electronInJetSubtraction(),
    m_systSgKeyMapJetsLoose_electronInJetSubtraction(),
    m_systSgKeyMapLargeRJets(),
    m_systSgKeyMapTrackJets(),
    m_systSgKeyMapMissingET(),
    m_systSgKeyMapMissingETLoose(),
    m_systSgKeyMapKLFitter(),
    m_systSgKeyMapKLFitterLoose(),
    m_systSgKeyMapPseudoTop(),
    m_systSgKeyMapPseudoTopLoose(),
    m_systPersistantAllTTreeNames(),
    m_systDecoKeyMapJetGhostTrack(),
    m_systSgKeyMapTracks(),

    m_list_systHashAll(),
    m_list_systHash_electronInJetSubtraction(),

    m_allTriggers_Tight(),
    m_electronTriggers_Tight(),
    m_muonTriggers_Tight(),
    m_tauTriggers_Tight(),

    m_allTriggers_Loose(),
    m_electronTriggers_Loose(),
    m_muonTriggers_Loose(),
    m_tauTriggers_Loose()
  {}
}
