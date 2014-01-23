/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// TrigJpsiRootAnalysis class
///---------------------------------------------------
/// B-trigger analysis
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TriggerJpsiAnalysis
///===================================================

#ifndef TrigJpsiRootAnalysis_H
#define TrigJpsiRootAnalysis_H

// c++ stuff
#include <Riostream.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

// Root stuff
#include <TSystem.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

// BPhys helper scripts
#include "NtupleReader.h"

// TrigJpsiAnalysis stuff
#include "Log.h"
#include "Utils.h"
#include "BCompositeParticle.h"
#include "Chain.h"
#include "Calib.h"
#include "Details.h"
#include "TrigHits.h"
#include "AnalysisBase.h"
#include "TrigDiMuonExtrapolator.h"
#include "SimpleJpsi.h"
#include "CutsJpsi.h"
#include "HistogramHolder.h"

///===================================================
class TrigJpsiRootAnalysis : public AnalysisBase, public HistogramHolder {

  public:
    /// constructor
    TrigJpsiRootAnalysis(int olIn, std::vector<std::string> files) :
      AnalysisBase(olIn), HistogramHolder(olIn), m_currentEntry(-1),m_currentNEntries(-1), m_ntupleReader(), m_nCandidates(0), m_nEntries(0)
    {
      Log log("TrigJpsiRootAnalysis::TrigJpsiRootAnalysis", ol());
      //load files
      loadFiles(files);
    }

    TrigJpsiRootAnalysis(int olIn, std::string dirPath) :
        AnalysisBase(olIn), HistogramHolder(olIn), m_currentEntry(-1),m_currentNEntries(-1), m_ntupleReader(), m_nCandidates(0), m_nEntries(0)
    {
      Log log("TrigJpsiRootAnalysis::TrigJpsiRootAnalysis", ol());
      //load files
      loadDirectory(dirPath);
    }

    /// analyzes

    void analyzeJpsi(std::string settings,
                     std::string cutStr);                   // plot a lot of offline J/psi histogram

    void analyzeChain(std::string chain,
                      std::string settings,
                      std::string cutStr);                  // analyze single chain accessing different stages of the algoritm

    void compareChains(std::vector<std::string> chains,
                       std::string settings,
                       std::string cutStr);                 // compare over-all efficiency of different chains

    void analyzeRate(std::string chain,
                     std::string settings,
                     std::string cutStr);                   // calculate Level-1 rate fraction accepted by TrigDiMuon with different nHits cut

    void analyzeRateSimple(std::string chain,
                           std::string settings,
                           std::string cutStr);             // calculate rate fraction from trigger decision

    /// entries iterators
    bool iterateJpsi(std::string nameJpsi,
                     TTree** tJpsi,
                     BCompositeParticle* jpsi,
                     std::string nameTracks = "",
                     TTree** tTracks = NULL,
                     TrackBag* tracks = NULL,
                     std::string nameMuons = "",
                     TTree** tMuons = NULL,
                     MuonBag* muons = NULL,
                     std::string nameTrig = "",
                     TTree** tTrig = NULL,
                     TriggerBag* trigger = NULL,
                     std::string nameChain = "",
                     TTree** tChain = NULL,
                     Chain** trigChain = NULL);              // iterate through all the Jpsi entries reading requested objects from the ntuples

    bool iterateChain(std::string nameTrig,
                      TTree** tTrig,
                      TriggerBag* trigger,
                      std::string nameChain = "",
                      TTree** tChain = NULL,
                      Chain** trigChain = NULL,
                      bool doAllTracks=true,
                      bool doMatchedTracks=true);            // iterate through all Trigger entries in the ntuple

    /// some usefull functions

    TF1* fitGaus(std::string name, std::string settings);    // fit J/psi peak

    bool getEfficiencies(std::vector<double>& num,
                         double& denom,
                         std::vector<double>& eff,
                         std::vector<std::string> numHists,
                         std::string denomHist,
                         int loBin, int hiBin);              // calculate efficiencies

    int getPrescale(std::string chain,
                    TriggerBag& trigDecisions);             // get trigger prescale

    /// inline functions
    int  getCurrentEntry()    { return m_currentEntry; }
    int  getCurrentNEntries() { return m_currentNEntries; }

    int  getNCandidates()     { return m_nCandidates; }
    int  getNEntries()        { return m_nEntries; }

    void resetIterator() {
      AnalysisBase::resetIterator();
      m_currentEntry = -1;
      m_currentNEntries = -1;
    }

    TH1D* getHist(std::string name) { return HistogramHolder::getHist(name); }  // this is just to have this method in the dictionary

  private:
    // private attributes
    int                       m_currentEntry;            // current entry
    int                       m_currentNEntries;         // current number of entries
    NtupleReader              m_ntupleReader;            // b-physics helper tool

    int                       m_nCandidates;             // total no of selected candidates
    int                       m_nEntries;                // total no of processed entries

    // histograming variables
    std::vector<double> m_jpsi;
    std::vector<double> m_jpsi_trk;
    std::vector<double> m_jpsi_fex;
    std::vector<double> m_jpsi_muID;
    std::vector<double> m_jpsi_vtx;

    std::vector<double> m_jpsi_chi2Prob;
    std::vector<double> m_jpsi_dPhi;
    std::vector<double> m_jpsi_pt;
    std::vector<double> m_jpsi_eta;

    std::vector<double> m_jpsiPt[3];
    std::vector<double> m_jpsiPt_trk[3];
    std::vector<double> m_jpsiPt_fex[3];
    std::vector<double> m_jpsiPt_hypo[3];

    std::vector<double> m_jpsiEta[3];
    std::vector<double> m_jpsiEta_trk[3];
    std::vector<double> m_jpsiEta_fex[3];
    std::vector<double> m_jpsiEta_hypo[3];

    std::vector<double> m_trk_chi2;
    std::vector<double> m_mu_eta;
    std::vector<double> m_mu_pt;
    std::vector<double> m_muMax_pt;
    std::vector<double> m_muMin_pt;
    std::vector<double> m_mu_e;
    std::vector<double> m_mu_pixHits;
    std::vector<double> m_mu_sctHits;
    std::vector<double> m_mu_mdtHits;

    std::vector<double> m_trigTrk_eta;
    std::vector<double> m_trigTrk_pt;

    std::vector<double> m_trig_chi2;

    std::vector<double> m_num_muID;
    std::vector<double> m_num_vtx;

    std::vector<double> m_rate_muID;
    std::vector<double> m_rate_vtx;

};
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
