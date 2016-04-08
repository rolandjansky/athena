/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_OBJECT_MATCHING_DICT_H
#define TRIG_OBJECT_MATCHING_DICT_H

#include "TrigObjectMatching/TrigMatchTool.h"

#include "EventKernel/INavigable4Momentum.h"

#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/JetCollection.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/PhotonContainer.h"
#include "muonEvent/MuonContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticle/TrigElectronContainer.h"

#define FORCEOBJECTMATCH(type) { \
  m1->matchToObject<Analysis::Electron,type>((type*)0, dvElectron); \
  m1->matchToObject<Analysis::Photon,type>((type*)0, dvPhoton); \
  m1->matchToObject<Analysis::Muon,type>((type*)0, dvMuon); \
  m1->matchToObject<Analysis::TauJet,type>((type*)0, dvTau); \
  m1->matchToObject<Jet,type>((type*)0, dvJet); \
  m1->matchToObject<INavigable4Momentum,type>((type*)0, dvNavigable); \
  m1->matchToObjects<Analysis::Electron,type>((type*)0, dvElectron); \
  m1->matchToObjects<Analysis::Photon,type>((type*)0, dvPhoton); \
  m1->matchToObjects<Analysis::Muon,type>((type*)0, dvMuon); \
  m1->matchToObjects<Analysis::TauJet,type>((type*)0, dvTau); \
  m1->matchToObjects<Jet,type>((type*)0, dvJet); \
  m1->matchToObjects<INavigable4Momentum,type>((type*)0, dvNavigable); \
}

#define FORCETRIGGERMATCH(type) { \
  FORCEOBJECTMATCH(type) \
  m1->matchToTriggerObjects<type>((INavigable4Momentum*)0, ""); \
  m1->matchToTriggerObject<type>((INavigable4Momentum*)0, ""); \
  m1->matchToTriggerObject<type>((INavigable4Momentum*)0, "", false); \
  m1->chainsPassedByObject<type>((INavigable4Momentum*)0); \
  m1->chainPassedByObject<type>((INavigable4Momentum*)0, ""); \
  m1->objectsInChain<type>(v, ""); \
  m1->getTriggerObjects<type>("", false); \
}

void nevercalled_TrigObjectMatchingARA(TrigMatchTool* m1) {

  std::vector<const INavigable4Momentum*> v;
  DataVector<INavigable4Momentum> dvNavigable;
  DataVector<Analysis::Electron> dvElectron;
  DataVector<Analysis::Photon> dvPhoton;
  DataVector<Analysis::Muon> dvMuon;
  DataVector<Analysis::TauJet> dvTau;
  DataVector<Jet> dvJet;

  FORCEOBJECTMATCH(Analysis::Electron);
  FORCEOBJECTMATCH(Analysis::Photon);
  FORCEOBJECTMATCH(Analysis::Muon);
  FORCEOBJECTMATCH(Analysis::TauJet);
  FORCEOBJECTMATCH(Jet);

  FORCETRIGGERMATCH(TrigMatch::TrigMuonL1);
  FORCETRIGGERMATCH(TrigMatch::TrigMuonL2);
  FORCETRIGGERMATCH(TrigMatch::TrigMuonEF);
  FORCETRIGGERMATCH(TrigMuonEFInfo);
  FORCETRIGGERMATCH(CombinedMuonFeature);
  FORCETRIGGERMATCH(TrigMatch::TrigElectronL1);
  FORCETRIGGERMATCH(TrigMatch::TrigElectronL2);
  FORCETRIGGERMATCH(TrigMatch::TrigElectronEF);
  FORCETRIGGERMATCH(TrigMatch::TrigPhotonL1);
  FORCETRIGGERMATCH(TrigMatch::TrigPhotonL2);
  FORCETRIGGERMATCH(TrigMatch::TrigPhotonEF);
  FORCETRIGGERMATCH(TrigMatch::TrigJetL1);
  FORCETRIGGERMATCH(TrigMatch::TrigJetL2);
  FORCETRIGGERMATCH(TrigMatch::TrigJetEF);
  FORCETRIGGERMATCH(TrigMatch::TrigTauL1);
  FORCETRIGGERMATCH(TrigMatch::TrigTauL2);
  FORCETRIGGERMATCH(TrigMatch::TrigTauEF);

}

#endif
