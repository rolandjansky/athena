/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOPEVENTDICT_H
#define TOPEVENTDICT_H

#include <vector>

#include "TopEvent/TTbarJetsBase.h"
#include "TopEvent/TTbarHadronicTop.h"
#include "TopEvent/TTbarHadronicTopB.h"
#include "TopEvent/TTbarLeptonicTop.h"

#include "TopEvent/TTbarJJ.h"
#include "TopEvent/TTbarJJTop.h"
#include "TopEvent/TTbarJJContainer.h"
#include "TopEvent/TTbarJJTopContainer.h"

#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarLJMet.h"
#include "TopEvent/TTbarLJTop.h"
#include "TopEvent/TTbarLJDiTops.h"
#include "TopEvent/TTbarLJDiTopsFitted.h"
#include "TopEvent/TTbarLJContainer.h"
#include "TopEvent/TTbarLJMetContainer.h"
#include "TopEvent/TTbarLJTopContainer.h"
#include "TopEvent/TTbarLJDiTopsContainer.h"
#include "TopEvent/TTbarLJDiTopsFittedContainer.h"

#include "TopEvent/TTbarLL.h"
#include "TopEvent/TTbarLLJets.h"
#include "TopEvent/TTbarLLJetsMet.h"
#include "TopEvent/TTbarLLContainer.h"
#include "TopEvent/TTbarLLJetsContainer.h"
#include "TopEvent/TTbarLLJetsMetContainer.h"

#include "TopEvent/TopJetHadronic.h"
#include "TopEvent/TopJetLeptonic.h"
#include "TopEvent/TopJetHadronicContainer.h"
#include "TopEvent/TopJetLeptonicContainer.h"

// Dictionary generation for templates
namespace TopEventDict {
  struct dict {

    /*
     * Base classes
     */
    
    // TTbarLeptonicTop
    TopRec::TTbarElectronicTop    m_ttelt;
    TopRec::TTbarMuonicTop        m_ttmlt;
    TopRec::TTbarTauTop           m_tttlt;
    TopRec::TTbarTruthLeptonicTop m_tttrlt;

    /*
     * All hadronic
     */

    // TTbarJJ
    std::vector<TopRec::TTbarJJ>                                                       m_ttjjv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarJJ*,std::vector<TopRec::TTbarJJ> >       m_ttjji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarJJ*,std::vector<TopRec::TTbarJJ> > m_cttjji;
    
    // TTbarJJTop
    std::vector<TopRec::TTbarJJTop>                                                          m_ttjjtv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarJJTop*,std::vector<TopRec::TTbarJJTop> >       m_ttjjti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarJJTop*,std::vector<TopRec::TTbarJJTop> > m_cttjjti; 

    /*
     * Single lepton
     */

    // TTbarLJ
    TopRec::TTbarEJ                                                                            m_ttej;
    TopRec::TTbarMuJ                                                                           m_ttmj;
    TopRec::TTbarTauJ                                                                          m_tttj;
    TopRec::TTbarTruthJ                                                                        m_tttrj;
    std::vector<TopRec::TTbarEJ>                                                               m_ttejv;
    std::vector<TopRec::TTbarMuJ>                                                              m_ttmjv;
    std::vector<TopRec::TTbarTauJ>                                                             m_tttjv;
    std::vector<TopRec::TTbarTruthJ>                                                           m_tttrjv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEJ*,std::vector<TopRec::TTbarEJ> >               m_tteji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuJ*,std::vector<TopRec::TTbarMuJ> >             m_ttmji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauJ*,std::vector<TopRec::TTbarTauJ> >           m_tttji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthJ*,std::vector<TopRec::TTbarTruthJ> >       m_tttrji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEJ*,std::vector<TopRec::TTbarEJ> >         m_ctteji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuJ*,std::vector<TopRec::TTbarMuJ> >       m_cttmji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauJ*,std::vector<TopRec::TTbarTauJ> >     m_ctttji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthJ*,std::vector<TopRec::TTbarTruthJ> > m_ctttrji;
    TopRec::TTbarEJContainer                                                                   m_ttejc;
    TopRec::TTbarMuJContainer                                                                  m_ttmjc;
    TopRec::TTbarTauJContainer                                                                 m_tttjc;
    TopRec::TTbarTruthJContainer                                                               m_tttrjc;

    // TTbarLJMet
    TopRec::TTbarEJMet                                                                               m_ttejm;
    TopRec::TTbarMuJMet                                                                              m_ttmjm;
    TopRec::TTbarTauJMet                                                                             m_tttjm;
    TopRec::TTbarTruthJMet                                                                           m_tttrjm;
    std::vector<TopRec::TTbarEJMet>                                                                  m_ttejmv;
    std::vector<TopRec::TTbarMuJMet>                                                                 m_ttmjmv;
    std::vector<TopRec::TTbarTauJMet>                                                                m_tttjmv;
    std::vector<TopRec::TTbarTruthJMet>                                                              m_tttrjmv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEJMet*,std::vector<TopRec::TTbarEJMet> >               m_ttejmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuJMet*,std::vector<TopRec::TTbarMuJMet> >             m_ttmjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauJMet*,std::vector<TopRec::TTbarTauJMet> >           m_tttjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthJMet*,std::vector<TopRec::TTbarTruthJMet> >       m_tttrjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEJMet*,std::vector<TopRec::TTbarEJMet> >         m_cttejmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuJMet*,std::vector<TopRec::TTbarMuJMet> >       m_cttmjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauJMet*,std::vector<TopRec::TTbarTauJMet> >     m_ctttjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthJMet*,std::vector<TopRec::TTbarTruthJMet> > m_ctttrjmi;
    TopRec::TTbarEJMetContainer                                                                      m_ttejmc;
    TopRec::TTbarMuJMetContainer                                                                     m_ttmjmc;
    TopRec::TTbarTauJMetContainer                                                                    m_tttjmc;
    TopRec::TTbarTruthJMetContainer                                                                  m_tttrjmc;

    // TTbarLJTop
    TopRec::TTbarEJTop                                                                               m_ttejt;
    TopRec::TTbarMuJTop                                                                              m_ttmjt;
    TopRec::TTbarTauJTop                                                                             m_tttjt;
    TopRec::TTbarTruthJTop                                                                           m_tttrjt;
    std::vector<TopRec::TTbarEJTop>                                                                  m_ttejtv;
    std::vector<TopRec::TTbarMuJTop>                                                                 m_ttmjtv;
    std::vector<TopRec::TTbarTauJTop>                                                                m_tttjtv;
    std::vector<TopRec::TTbarTruthJTop>                                                              m_tttrjtv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEJTop*,std::vector<TopRec::TTbarEJTop> >               m_ttejti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuJTop*,std::vector<TopRec::TTbarMuJTop> >             m_ttmjti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauJTop*,std::vector<TopRec::TTbarTauJTop> >           m_tttjti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthJTop*,std::vector<TopRec::TTbarTruthJTop> >       m_tttrjti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEJTop*,std::vector<TopRec::TTbarEJTop> >         m_cttejti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuJTop*,std::vector<TopRec::TTbarMuJTop> >       m_cttmjti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauJTop*,std::vector<TopRec::TTbarTauJTop> >     m_ctttjti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthJTop*,std::vector<TopRec::TTbarTruthJTop> > m_ctttrjti;
    TopRec::TTbarEJTopContainer                                                                      m_ttejtc;
    TopRec::TTbarMuJTopContainer                                                                     m_ttmjtc;
    TopRec::TTbarTauJTopContainer                                                                    m_tttjtc;
    TopRec::TTbarTruthJTopContainer                                                                  m_tttrjtc;

    // TTbarLJDiTops
    TopRec::TTbarEJDiTops                                                                                  m_ttejdt;
    TopRec::TTbarMuJDiTops                                                                                 m_ttmjdt;
    TopRec::TTbarTauJDiTops                                                                                m_tttjdt;
    TopRec::TTbarTruthJDiTops                                                                              m_tttrjdt;
    std::vector<TopRec::TTbarEJDiTops>                                                                     m_ttejdtv;
    std::vector<TopRec::TTbarMuJDiTops>                                                                    m_ttmjdtv;
    std::vector<TopRec::TTbarTauJDiTops>                                                                   m_tttjdtv;
    std::vector<TopRec::TTbarTruthJDiTops>                                                                 m_tttrjdtv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEJDiTops*,std::vector<TopRec::TTbarEJDiTops> >               m_ttejdti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuJDiTops*,std::vector<TopRec::TTbarMuJDiTops> >             m_ttmjdti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauJDiTops*,std::vector<TopRec::TTbarTauJDiTops> >           m_tttjdti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthJDiTops*,std::vector<TopRec::TTbarTruthJDiTops> >       m_tttrjdti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEJDiTops*,std::vector<TopRec::TTbarEJDiTops> >         m_cttejdti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuJDiTops*,std::vector<TopRec::TTbarMuJDiTops> >       m_cttmjdti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauJDiTops*,std::vector<TopRec::TTbarTauJDiTops> >     m_ctttjdti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthJDiTops*,std::vector<TopRec::TTbarTruthJDiTops> > m_ctttrjdti;
    TopRec::TTbarEJDiTopsContainer                                                                         m_ttejdtc;
    TopRec::TTbarMuJDiTopsContainer                                                                        m_ttmjdtc;
    TopRec::TTbarTauJDiTopsContainer                                                                       m_tttjdtc;
    TopRec::TTbarTruthJDiTopsContainer                                                                     m_tttrjdtc;

    // TTbarLJDiTopsFitted
    boost::shared_ptr<JetCollection>                                                                                   m_bsp;
    TopRec::TTbarEJDiTopsFitted                                                                                        m_ttejdtf;
    TopRec::TTbarMuJDiTopsFitted                                                                                       m_ttmjdtf;
    TopRec::TTbarTauJDiTopsFitted                                                                                      m_tttjdtf;
    TopRec::TTbarTruthJDiTopsFitted                                                                                    m_tttrjdtf;
//     TopRec::TTbarLJDiTopsFittedHelper<ElectronContainer>                                                               m_ttejdtfh;
//     TopRec::TTbarLJDiTopsFittedHelper<Analysis::MuonContainer>                                                         m_ttmjdtfh;
//     TopRec::TTbarLJDiTopsFittedHelper<Analysis::TauJetContainer>                                                       m_tttjdtfh;
//     TopRec::TTbarLJDiTopsFittedHelper<TruthParticleContainer>                                                          m_tttrjdtfh;
    std::vector<TopRec::TTbarEJDiTopsFitted>                                                                           m_ttejdtfv;
    std::vector<TopRec::TTbarMuJDiTopsFitted>                                                                          m_ttmjdtfv;
    std::vector<TopRec::TTbarTauJDiTopsFitted>                                                                         m_tttjdtfv;
    std::vector<TopRec::TTbarTruthJDiTopsFitted>                                                                       m_tttrjdtfv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEJDiTopsFitted*,std::vector<TopRec::TTbarEJDiTopsFitted> >               m_ttejdtfi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuJDiTopsFitted*,std::vector<TopRec::TTbarMuJDiTopsFitted> >             m_ttmjdtfi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauJDiTopsFitted*,std::vector<TopRec::TTbarTauJDiTopsFitted> >           m_tttjdtfi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthJDiTopsFitted*,std::vector<TopRec::TTbarTruthJDiTopsFitted> >       m_tttrjdtfi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEJDiTopsFitted*,std::vector<TopRec::TTbarEJDiTopsFitted> >         m_cttejdtfi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuJDiTopsFitted*,std::vector<TopRec::TTbarMuJDiTopsFitted> >       m_cttmjdtfi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauJDiTopsFitted*,std::vector<TopRec::TTbarTauJDiTopsFitted> >     m_ctttjdtfi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthJDiTopsFitted*,std::vector<TopRec::TTbarTruthJDiTopsFitted> > m_ctttrjdtfi;
    TopRec::TTbarEJDiTopsFittedContainer                                                                               m_ttejdtfc;
    TopRec::TTbarMuJDiTopsFittedContainer                                                                              m_ttmjdtfc;
    TopRec::TTbarTauJDiTopsFittedContainer                                                                             m_tttjdtfc;
    TopRec::TTbarTruthJDiTopsFittedContainer                                                                           m_tttrjdtfc;

    /*
     * Dilepton
     */
    
    // TTbarLL
    TopRec::TTbarEE                                                                                    m_ttee;
    TopRec::TTbarEMu                                                                                   m_ttem;
    TopRec::TTbarETau                                                                                  m_ttet;
    TopRec::TTbarETrack                                                                                m_ttetk;
    TopRec::TTbarMuMu                                                                                  m_ttmm;
    TopRec::TTbarMuTau                                                                                 m_ttmt;
    TopRec::TTbarMuTrack                                                                               m_ttmtk;
    TopRec::TTbarTauTau                                                                                m_tttt;
    TopRec::TTbarTruthTruth                                                                            m_tttrtr;
    std::vector<TopRec::TTbarEE>                                                                       m_tteev;
    std::vector<TopRec::TTbarEMu>                                                                      m_ttemv;
    std::vector<TopRec::TTbarETau>                                                                     m_ttetv;
    std::vector<TopRec::TTbarETrack>                                                                   m_ttetkv;
    std::vector<TopRec::TTbarMuMu>                                                                     m_ttmmv;
    std::vector<TopRec::TTbarMuTau>                                                                    m_ttmtv;
    std::vector<TopRec::TTbarMuTrack>                                                                  m_ttmtkv;
    std::vector<TopRec::TTbarTauTau>                                                                   m_ttttv;
    std::vector<TopRec::TTbarTruthTruth>                                                               m_tttrtrv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEE*,std::vector<TopRec::TTbarEE> >                       m_tteei;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEMu*,std::vector<TopRec::TTbarEMu> >                     m_ttemi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETau*,std::vector<TopRec::TTbarETau> >                   m_tteti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETrack*,std::vector<TopRec::TTbarETrack> >               m_ttetki;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuMu*,std::vector<TopRec::TTbarMuMu> >                   m_ttmmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTau*,std::vector<TopRec::TTbarMuTau> >                 m_ttmti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTrack*,std::vector<TopRec::TTbarMuTrack> >             m_ttmtki;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauTau*,std::vector<TopRec::TTbarTauTau> >               m_tttti;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthTruth*,std::vector<TopRec::TTbarTruthTruth> >       m_tttrtri;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEE*,std::vector<TopRec::TTbarEE> >                 m_ctteei;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEMu*,std::vector<TopRec::TTbarEMu> >               m_cttemi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETau*,std::vector<TopRec::TTbarETau> >             m_ctteti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETrack*,std::vector<TopRec::TTbarETrack> >         m_cttetki;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuMu*,std::vector<TopRec::TTbarMuMu> >             m_cttmmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTau*,std::vector<TopRec::TTbarMuTau> >           m_cttmti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTrack*,std::vector<TopRec::TTbarMuTrack> >       m_cttmtki;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauTau*,std::vector<TopRec::TTbarTauTau> >         m_ctttti;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthTruth*,std::vector<TopRec::TTbarTruthTruth> > m_ctttrtri;
    TopRec::TTbarEEContainer                                                                           m_tteec;
    TopRec::TTbarEMuContainer                                                                          m_ttemc;
    TopRec::TTbarETauContainer                                                                         m_ttetc;
    TopRec::TTbarETrackContainer                                                                       m_ttetkc;
    TopRec::TTbarMuMuContainer                                                                         m_ttmmc;
    TopRec::TTbarMuTauContainer                                                                        m_ttmtc;
    TopRec::TTbarMuTrackContainer                                                                      m_ttmtkc;
    TopRec::TTbarTauTauContainer                                                                       m_ttttc;
    TopRec::TTbarTruthTruthContainer                                                                   m_tttrtrc;

    // TTbarLLJets
    TopRec::TTbarEEJets                                                                                        m_tteej;
    TopRec::TTbarEMuJets                                                                                       m_ttemj;
    TopRec::TTbarETauJets                                                                                      m_ttetj;
    TopRec::TTbarETrackJets                                                                                    m_ttetkj;
    TopRec::TTbarMuMuJets                                                                                      m_ttmmj;
    TopRec::TTbarMuTauJets                                                                                     m_ttmtj;
    TopRec::TTbarMuTrackJets                                                                                   m_ttmtkj;
    TopRec::TTbarTauTauJets                                                                                    m_ttttj;
    TopRec::TTbarTruthTruthJets                                                                                m_tttrtrj;
    std::vector<TopRec::TTbarEEJets>                                                                           m_tteejv;
    std::vector<TopRec::TTbarEMuJets>                                                                          m_ttemjv;
    std::vector<TopRec::TTbarETauJets>                                                                         m_ttetjv;
    std::vector<TopRec::TTbarETrackJets>                                                                       m_ttetkjv;
    std::vector<TopRec::TTbarMuMuJets>                                                                         m_ttmmjv;
    std::vector<TopRec::TTbarMuTauJets>                                                                        m_ttmtjv;
    std::vector<TopRec::TTbarMuTrackJets>                                                                      m_ttmtkjv;
    std::vector<TopRec::TTbarTauTauJets>                                                                       m_ttttjv;
    std::vector<TopRec::TTbarTruthTruthJets>                                                                   m_tttrtrjv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEEJets*,std::vector<TopRec::TTbarEEJets> >                       m_tteeji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEMuJets*,std::vector<TopRec::TTbarEMuJets> >                     m_ttemji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETauJets*,std::vector<TopRec::TTbarETauJets> >                   m_ttetji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETrackJets*,std::vector<TopRec::TTbarETrackJets> >               m_ttetkji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuMuJets*,std::vector<TopRec::TTbarMuMuJets> >                   m_ttmmji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTauJets*,std::vector<TopRec::TTbarMuTauJets> >                 m_ttmtji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTrackJets*,std::vector<TopRec::TTbarMuTrackJets> >             m_ttmtkji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauTauJets*,std::vector<TopRec::TTbarTauTauJets> >               m_ttttji;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthTruthJets*,std::vector<TopRec::TTbarTruthTruthJets> >       m_tttrtrji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEEJets*,std::vector<TopRec::TTbarEEJets> >                 m_ctteeji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEMuJets*,std::vector<TopRec::TTbarEMuJets> >               m_cttemji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETauJets*,std::vector<TopRec::TTbarETauJets> >             m_cttetji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETrackJets*,std::vector<TopRec::TTbarETrackJets> >         m_cttetkji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuMuJets*,std::vector<TopRec::TTbarMuMuJets> >             m_cttmmji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTauJets*,std::vector<TopRec::TTbarMuTauJets> >           m_cttmtji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTrackJets*,std::vector<TopRec::TTbarMuTrackJets> >       m_cttmtkji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauTauJets*,std::vector<TopRec::TTbarTauTauJets> >         m_cttttji;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthTruthJets*,std::vector<TopRec::TTbarTruthTruthJets> > m_ctttrtrji;
    TopRec::TTbarEEJetsContainer                                                                               m_tteejc;
    TopRec::TTbarEMuJetsContainer                                                                              m_ttemjc;
    TopRec::TTbarETauJetsContainer                                                                             m_ttetjc;
    TopRec::TTbarETrackJetsContainer                                                                           m_ttetkjc;
    TopRec::TTbarMuMuJetsContainer                                                                             m_ttmmjc;
    TopRec::TTbarMuTauJetsContainer                                                                            m_ttmtjc;
    TopRec::TTbarMuTrackJetsContainer                                                                          m_ttmtkjc;
    TopRec::TTbarTauTauJetsContainer                                                                           m_ttttjc;
    TopRec::TTbarTruthTruthJetsContainer                                                                       m_tttrtrjc;

    // TTbarLLJetsMet
    TopRec::TTbarEEJetsMet                                                                                           m_tteejm;
    TopRec::TTbarEMuJetsMet                                                                                          m_ttemjm;
    TopRec::TTbarETauJetsMet                                                                                         m_ttetjm;
    TopRec::TTbarETrackJetsMet                                                                                       m_ttetkjm;
    TopRec::TTbarMuMuJetsMet                                                                                         m_ttmmjm;
    TopRec::TTbarMuTauJetsMet                                                                                        m_ttmtjm;
    TopRec::TTbarMuTrackJetsMet                                                                                      m_ttmtkjm;
    TopRec::TTbarTauTauJetsMet                                                                                       m_ttttjm;
    TopRec::TTbarTruthTruthJetsMet                                                                                   m_tttrtrjm;
    std::vector<TopRec::TTbarEEJetsMet>                                                                              m_tteejmv;
    std::vector<TopRec::TTbarEMuJetsMet>                                                                             m_ttemjmv;
    std::vector<TopRec::TTbarETauJetsMet>                                                                            m_ttetjmv;
    std::vector<TopRec::TTbarETrackJetsMet>                                                                          m_ttetkjmv;
    std::vector<TopRec::TTbarMuMuJetsMet>                                                                            m_ttmmjmv;
    std::vector<TopRec::TTbarMuTauJetsMet>                                                                           m_ttmtjmv;
    std::vector<TopRec::TTbarMuTrackJetsMet>                                                                         m_ttmtkjmv;
    std::vector<TopRec::TTbarTauTauJetsMet>                                                                          m_ttttjmv;
    std::vector<TopRec::TTbarTruthTruthJetsMet>                                                                      m_tttrtrjmv;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEEJetsMet*,std::vector<TopRec::TTbarEEJetsMet> >                       m_tteejmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarEMuJetsMet*,std::vector<TopRec::TTbarEMuJetsMet> >                     m_ttemjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETauJetsMet*,std::vector<TopRec::TTbarETauJetsMet> >                   m_ttetjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarETrackJetsMet*,std::vector<TopRec::TTbarETrackJetsMet> >               m_ttetkjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuMuJetsMet*,std::vector<TopRec::TTbarMuMuJetsMet> >                   m_ttmmjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTauJetsMet*,std::vector<TopRec::TTbarMuTauJetsMet> >                 m_ttmtjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarMuTrackJetsMet*,std::vector<TopRec::TTbarMuTrackJetsMet> >             m_ttmtkjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTauTauJetsMet*,std::vector<TopRec::TTbarTauTauJetsMet> >               m_ttttjmi;
    __gnu_cxx::__normal_iterator<TopRec::TTbarTruthTruthJetsMet*,std::vector<TopRec::TTbarTruthTruthJetsMet> >       m_tttrtrjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEEJetsMet*,std::vector<TopRec::TTbarEEJetsMet> >                 m_ctteejmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarEMuJetsMet*,std::vector<TopRec::TTbarEMuJetsMet> >               m_cttemjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETauJetsMet*,std::vector<TopRec::TTbarETauJetsMet> >             m_cttetjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarETrackJetsMet*,std::vector<TopRec::TTbarETrackJetsMet> >         m_cttetkjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuMuJetsMet*,std::vector<TopRec::TTbarMuMuJetsMet> >             m_cttmmjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTauJetsMet*,std::vector<TopRec::TTbarMuTauJetsMet> >           m_cttmtjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarMuTrackJetsMet*,std::vector<TopRec::TTbarMuTrackJetsMet> >       m_cttmtkjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTauTauJetsMet*,std::vector<TopRec::TTbarTauTauJetsMet> >         m_cttttjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TTbarTruthTruthJetsMet*,std::vector<TopRec::TTbarTruthTruthJetsMet> > m_ctttrtrjmi;
    TopRec::TTbarEEJetsMetContainer                                                                                  m_tteejmc;
    TopRec::TTbarEMuJetsMetContainer                                                                                 m_ttemjmc;
    TopRec::TTbarETauJetsMetContainer                                                                                m_ttetjmc;
    TopRec::TTbarETrackJetsMetContainer                                                                              m_ttetkjmc;
    TopRec::TTbarMuMuJetsMetContainer                                                                                m_ttmmjmc;
    TopRec::TTbarMuTauJetsMetContainer                                                                               m_ttmtjmc;
    TopRec::TTbarMuTrackJetsMetContainer                                                                             m_ttmtkjmc;
    TopRec::TTbarTauTauJetsMetContainer                                                                              m_ttttjmc;
    TopRec::TTbarTruthTruthJetsMetContainer                                                                          m_tttrtrjmc;

    /*
     * High Pt TopJet
     */
    
    // TopJetHadronic
    std::vector<TopRec::TopJetHadronic>                                                              m_tjhv;
    __gnu_cxx::__normal_iterator<TopRec::TopJetHadronic*,std::vector<TopRec::TopJetHadronic> >       m_tjhi;
    __gnu_cxx::__normal_iterator<const TopRec::TopJetHadronic*,std::vector<TopRec::TopJetHadronic> > m_ctjhi;
    
    // TopJetLeptonic
    TopRec::TopJetElectron                                                                           m_tje;
    TopRec::TopJetMuon                                                                               m_tjm;
    TopRec::TopJetTau                                                                                m_tjt;
    TopRec::TopJetTruth                                                                              m_tjtr;
    std::vector<TopRec::TopJetElectron>                                                              m_tjev;
    std::vector<TopRec::TopJetMuon>                                                                  m_tjmv;
    std::vector<TopRec::TopJetTau>                                                                   m_tjtv;
    std::vector<TopRec::TopJetTruth>                                                                 m_tjtrv;
    __gnu_cxx::__normal_iterator<TopRec::TopJetElectron*,std::vector<TopRec::TopJetElectron> >       m_tjei;
    __gnu_cxx::__normal_iterator<TopRec::TopJetMuon*,std::vector<TopRec::TopJetMuon> >               m_tjmi;
    __gnu_cxx::__normal_iterator<TopRec::TopJetTau*,std::vector<TopRec::TopJetTau> >                 m_tjti;
    __gnu_cxx::__normal_iterator<TopRec::TopJetTruth*,std::vector<TopRec::TopJetTruth> >             m_tjtri;
    __gnu_cxx::__normal_iterator<const TopRec::TopJetElectron*,std::vector<TopRec::TopJetElectron> > m_ctjei;
    __gnu_cxx::__normal_iterator<const TopRec::TopJetMuon*,std::vector<TopRec::TopJetMuon> >         m_ctjmi;
    __gnu_cxx::__normal_iterator<const TopRec::TopJetTau*,std::vector<TopRec::TopJetTau> >           m_ctjti;
    __gnu_cxx::__normal_iterator<const TopRec::TopJetTruth*,std::vector<TopRec::TopJetTruth> >       m_ctjtri;
    TopRec::TopJetElectronContainer                                                                  m_tjec;
    TopRec::TopJetMuonContainer                                                                      m_tjmc;
    TopRec::TopJetTauContainer                                                                       m_tjtc;
    TopRec::TopJetTruthContainer                                                                     m_tjtrc;
  };
}

#endif // TOPEVENTDICT_H
