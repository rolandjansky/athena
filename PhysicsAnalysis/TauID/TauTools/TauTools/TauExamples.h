/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEXAMPLE_H
#define TAUEXAMPLE_H

//////////////////////////////////////////////////////////////////////////////////////
/// Name    : TauExamples.h
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri and Dugan O'Neil
/// Created : May 2008
///
/// DESCRIPTION:
///
/// This class is a simple example for taus and inherits from AraToolBase from "AraTool", Dual Use Tools.
/// Uses the methods from TruthHandler.h file to get visible pT, hadronic truth taus etc.
/// and uses the methods from RecoHandler.h file to find and select taRec reco candidates..
/// It has methods to find tau reconstruction and selection efficiencies as a function of tau visible pT, 
/// eta and phi for 1 and 3 prongs.
/// This class was written to be run on events reconstructed in 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/RecoHandler.h"  
#include "TauTools/TruthHandler.h"
#include "TauTools/MatchingTools.h"

#include "TH1.h"
#include "TProfile.h"
#include "TFile.h"

#include <string>

class TauExamples { 
  
public:
  
  TauExamples();
  virtual ~TauExamples();

  /** find tau reconstruction and selection efficiencies as a function of tau visible pT, eta and phi for 1 and 3 prongs */
  StatusCode getTauRecProngEfficiency(const Analysis::TauJetContainer *tau, const TruthParticleContainer *mcpart);
  /** Dumps out histograms is a rootfile */
  StatusCode finalizeProng();

  /** Get the tauRec resolution */
  StatusCode getTauRecResolution(const Analysis::TauJetContainer *tau, const TruthParticleContainer *mcpart);
  /** Dumps out histograms is a rootfile */
  StatusCode finalizeResolution();

  /** Create all the histograms and profiles */
  void CreateHistos();
  /** Put all histos in one single file */
  void StoreHistos(TFile *rootfile);

  ///////////////////////////////////////
  /** Setters and Getters for Truthparticle Container */
  std::string getTruthContainer() { return m_truthParticleContainerName; }
  void        setTruthContainer( std::string c1 ) { m_truthParticleContainerName = c1; }

  /** Setters and Getters for TauJet Container */
  std::string getTauJetContainer() { return m_taujetContainerName; }
  void        setTauJetContainer( std::string c2 ) { m_taujetContainerName = c2; }

  /** Setters and Getters for TauJet Container */
  float getdeltaRMatchCut() { return m_deltaRMatchCut; }
  void  setdeltaRMatchCut( float p1 ) { m_deltaRMatchCut = p1; }
  
 protected:

  /** Cone matching cut */
  float m_deltaRMatchCut;

  /** name of the AOD truth particle container to retrieve from StoreGate or Transient tree */
  std::string m_truthParticleContainerName;
  
  /** name of the AOD taujet container to retrieve from StoreGate or Transient tree */
  std::string m_taujetContainerName;

 private:

  /** Instantiate TruthHandler object */
  TruthHandler truthHandler;

  /** Instantiate RecoHandler object */
  RecoHandler  recoHandler;

  /** Instantiate MatchingTools object */
  MatchingTools matchtool;
  
  /** Simple Counters*/
  int m_oneProng;
  int m_threeProng;

  TProfile  *m_eff_reco_1P_TauRec;
  TProfile  *m_eff_reco_3P_TauRec;
  TProfile  *m_eff_reco_13P_TauRec;

  TProfile  *m_eff_reco_1Pphi_TauRec;
  TProfile  *m_eff_reco_3Pphi_TauRec;
  TProfile  *m_eff_reco_13Pphi_TauRec;

  TProfile  *m_eff_reco_1Peta_TauRec;
  TProfile  *m_eff_reco_3Peta_TauRec;
  TProfile  *m_eff_reco_13Peta_TauRec;

  TProfile  *m_eff_id_1P_TauRec;
  TProfile  *m_eff_id_3P_TauRec;
  TProfile  *m_eff_id_13P_TauRec;

  TProfile  *m_eff_id_1Pphi_TauRec;
  TProfile  *m_eff_id_3Pphi_TauRec;
  TProfile  *m_eff_id_13Pphi_TauRec;

  TProfile  *m_eff_id_1Peta_TauRec;
  TProfile  *m_eff_id_3Peta_TauRec;
  TProfile  *m_eff_id_13Peta_TauRec;

  /**Resolutions */
  TH1F  *m_etRes_reco_TauRec;
  TH1F  *m_etaRes_reco_TauRec;
  TH1F  *m_phiRes_reco_TauRec;

  TH1F  *m_etRes_id_TauRec;
  TH1F  *m_etaRes_id_TauRec;
  TH1F  *m_phiRes_id_TauRec;

};

#endif //TAUEXAMPLE_H
