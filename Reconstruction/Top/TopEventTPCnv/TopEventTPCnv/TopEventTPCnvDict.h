/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPEVENTTPCNVDICT_H
#define TOPEVENTTPCNVDICT_H

// FIXME: a hack to avoid compilation errors in 15.6.1.6 based builds
#define private public
#define protected public

#include "TopEventTPCnv/TTbarJJContainer_p1.h"
#include "TopEventTPCnv/TTbarJJContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarJJ_p1.h"
#include "TopEventTPCnv/TTbarJetsBase_p1.h"

#include "TopEventTPCnv/TTbarJJTopContainer_p1.h"
#include "TopEventTPCnv/TTbarJJTopContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarJJTop_p1.h"

#include "TopEventTPCnv/TTbarLJContainer_p1.h"
#include "TopEventTPCnv/TTbarLJContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLJ_p1.h"

#include "TopEventTPCnv/TTbarLJMetContainer_p1.h"
#include "TopEventTPCnv/TTbarLJMetContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLJMet_p1.h"

#include "TopEventTPCnv/TTbarLJTopContainer_p1.h"
#include "TopEventTPCnv/TTbarLJTopContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLJTop_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsContainer_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLJDiTops_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsFittedContainer_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsFittedContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsFitted_p1.h"

#include "TopEventTPCnv/TTbarLLContainer_p1.h"
#include "TopEventTPCnv/TTbarLLContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLL_p1.h"

#include "TopEventTPCnv/TTbarLLJetsContainer_p1.h"
#include "TopEventTPCnv/TTbarLLJetsContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLLJets_p1.h"

#include "TopEventTPCnv/TTbarLLJetsMetContainer_p1.h"
#include "TopEventTPCnv/TTbarLLJetsMetContainerCnv_p1.h"
#include "TopEventTPCnv/TTbarLLJetsMet_p1.h"

#include "TopEventTPCnv/TopJetLeptonicContainer_p1.h"
#include "TopEventTPCnv/TopJetLeptonicContainerCnv_p1.h"
#include "TopEventTPCnv/TopJetLeptonic_p1.h"

#include "TopEventTPCnv/TopJetHadronicContainer_p1.h"
#include "TopEventTPCnv/TopJetHadronicContainerCnv_p1.h"
#include "TopEventTPCnv/TopJetHadronic_p1.h"
#include "TopEventTPCnv/TopJet_p1.h"

struct dict {

  /*
   * All Hadronic
   */

  // TTbarJJ
  T_TPCnv<TopRec::TTbarJJContainer, TTbarJJContainer_p1>                                 m_ttjjc;
  
  // TTbarJJTop
  T_TPCnv<TopRec::TTbarJJTopContainer, TTbarJJTopContainer_p1>                           m_ttjjtc;

  /*
   * Single Lepton
   */

  // TTbarLJ
  T_TPCnv<TopRec::TTbarEJContainer, TTbarEJContainer_p1>                                 m_ttejc;
  T_TPCnv<TopRec::TTbarMuJContainer, TTbarMuJContainer_p1>                               m_ttmjc;
  T_TPCnv<TopRec::TTbarTauJContainer, TTbarTauJContainer_p1>                             m_tttjc;
  T_TPCnv<TopRec::TTbarTruthJContainer, TTbarTruthJContainer_p1>                         m_tttrjc;

  // TTbarLJMet
  T_TPCnv<TopRec::TTbarEJMetContainer, TTbarEJMetContainer_p1>                           m_ttejmc;
  T_TPCnv<TopRec::TTbarMuJMetContainer, TTbarMuJMetContainer_p1>                         m_ttmjmc;
  T_TPCnv<TopRec::TTbarTauJMetContainer, TTbarTauJMetContainer_p1>                       m_tttjmc;
  T_TPCnv<TopRec::TTbarTruthJMetContainer, TTbarTruthJMetContainer_p1>                   m_tttrjmc;

  // TTBarLJTop
  T_TPCnv<TopRec::TTbarEJTopContainer, TTbarEJTopContainer_p1>                           m_ttejtc;
  T_TPCnv<TopRec::TTbarMuJTopContainer, TTbarMuJTopContainer_p1>                         m_ttmjtc;
  T_TPCnv<TopRec::TTbarTauJTopContainer, TTbarTauJTopContainer_p1>                       m_tttjtc;
  T_TPCnv<TopRec::TTbarTruthJTopContainer, TTbarTruthJTopContainer_p1>                   m_tttrjtc;

  // TTbarLJDiTops
  T_TPCnv<TopRec::TTbarEJDiTopsContainer, TTbarEJDiTopsContainer_p1>                     m_ttejdtc;
  T_TPCnv<TopRec::TTbarMuJDiTopsContainer, TTbarMuJDiTopsContainer_p1>                   m_ttmjdtc;
  T_TPCnv<TopRec::TTbarTauJDiTopsContainer, TTbarTauJDiTopsContainer_p1>                 m_tttjdtc;
  T_TPCnv<TopRec::TTbarTruthJDiTopsContainer, TTbarTruthJDiTopsContainer_p1>             m_tttrjdtc;

  // TTBarLJDiTopsFitted
  T_TPCnv<TopRec::TTbarEJDiTopsFittedContainer, TTbarEJDiTopsFittedContainer_p1>         m_ttejdtfc;
  T_TPCnv<TopRec::TTbarMuJDiTopsFittedContainer, TTbarMuJDiTopsFittedContainer_p1>       m_ttmjdtfc;
  T_TPCnv<TopRec::TTbarTauJDiTopsFittedContainer, TTbarTauJDiTopsFittedContainer_p1>     m_tttjdtfc;
  T_TPCnv<TopRec::TTbarTruthJDiTopsFittedContainer, TTbarTruthJDiTopsFittedContainer_p1> m_tttrjdtfc;

  /*
   * Dilepton
   */

  // TTbarLL
  T_TPCnv<TopRec::TTbarEEContainer, TTbarEEContainer_p1>                                 m_tteec;
  T_TPCnv<TopRec::TTbarEMuContainer, TTbarEMuContainer_p1>                               m_ttemc;
  T_TPCnv<TopRec::TTbarETauContainer, TTbarETauContainer_p1>                             m_ttetc;
  T_TPCnv<TopRec::TTbarETrackContainer, TTbarETrackContainer_p1>                         m_ttetkc;
  T_TPCnv<TopRec::TTbarMuMuContainer, TTbarMuMuContainer_p1>                             m_ttmmc;
  T_TPCnv<TopRec::TTbarMuTauContainer, TTbarMuTauContainer_p1>                           m_ttmtc;
  T_TPCnv<TopRec::TTbarMuTrackContainer, TTbarMuTrackContainer_p1>                       m_ttmtkc;
  T_TPCnv<TopRec::TTbarTauTauContainer, TTbarTauTauContainer_p1>                         m_ttttc;
  T_TPCnv<TopRec::TTbarTruthTruthContainer, TTbarTruthTruthContainer_p1>                 m_tttrtrc;

  // TTbarLLJets
  T_TPCnv<TopRec::TTbarEEJetsContainer, TTbarEEJetsContainer_p1>                         m_tteejc;
  T_TPCnv<TopRec::TTbarEMuJetsContainer, TTbarEMuJetsContainer_p1>                       m_ttemjc;
  T_TPCnv<TopRec::TTbarETauJetsContainer, TTbarETauJetsContainer_p1>                     m_ttetjc;
  T_TPCnv<TopRec::TTbarETrackJetsContainer, TTbarETrackJetsContainer_p1>                 m_ttetkjc;
  T_TPCnv<TopRec::TTbarMuMuJetsContainer, TTbarMuMuJetsContainer_p1>                     m_ttmmjc;
  T_TPCnv<TopRec::TTbarMuTauJetsContainer, TTbarMuTauJetsContainer_p1>                   m_ttmtjc;
  T_TPCnv<TopRec::TTbarMuTrackJetsContainer, TTbarMuTrackJetsContainer_p1>               m_ttmtkjc;
  T_TPCnv<TopRec::TTbarTauTauJetsContainer, TTbarTauTauJetsContainer_p1>                 m_ttttjc;
  T_TPCnv<TopRec::TTbarTruthTruthJetsContainer, TTbarTruthTruthJetsContainer_p1>         m_tttrtrjc;

  // TTbarLLJetsMet
  T_TPCnv<TopRec::TTbarEEJetsMetContainer, TTbarEEJetsMetContainer_p1>                   m_tteejmc;
  T_TPCnv<TopRec::TTbarEMuJetsMetContainer, TTbarEMuJetsMetContainer_p1>                 m_ttemjmc;
  T_TPCnv<TopRec::TTbarETauJetsMetContainer, TTbarETauJetsMetContainer_p1>               m_ttetjmc;
  T_TPCnv<TopRec::TTbarETrackJetsMetContainer, TTbarETrackJetsMetContainer_p1>           m_ttetkjmc;
  T_TPCnv<TopRec::TTbarMuMuJetsMetContainer, TTbarMuMuJetsMetContainer_p1>               m_ttmmjmc;
  T_TPCnv<TopRec::TTbarMuTauJetsMetContainer, TTbarMuTauJetsMetContainer_p1>             m_ttmtjmc;
  T_TPCnv<TopRec::TTbarMuTrackJetsMetContainer, TTbarMuTrackJetsMetContainer_p1>         m_ttmtkjmc;
  T_TPCnv<TopRec::TTbarTauTauJetsMetContainer, TTbarTauTauJetsMetContainer_p1>           m_ttttjmc;
  T_TPCnv<TopRec::TTbarTruthTruthJetsMetContainer, TTbarTruthTruthJetsMetContainer_p1>   m_tttrtrjmc;

  /*
   * High Pt TopJets
   */

  // TopJetHadronic
  T_TPCnv<TopRec::TopJetHadronicContainer, TopJetHadronicContainer_p1>                   m_tjhc;

  // TopJetLeptonic
  T_TPCnv<TopRec::TopJetElectronContainer, TopJetElectronContainer_p1>                   m_tjec;
  T_TPCnv<TopRec::TopJetMuonContainer, TopJetMuonContainer_p1>                           m_tjmc;
  T_TPCnv<TopRec::TopJetTauContainer, TopJetTauContainer_p1>                             m_tjtc;
  T_TPCnv<TopRec::TopJetTruthContainer, TopJetTruthContainer_p1>                         m_tjtrc;
};

#endif // TOPEVENTTPCNVDICT_H
