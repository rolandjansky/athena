/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// TrigDiMuonRootCalib class
///---------------------------------------------------
/// TrigDiMuon calibration and detailed
/// performance study
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TrigDiMuonCalib
///===================================================

#ifndef TrigDiMuonRootCalib_H
#define TrigDiMuonRootCalib_H

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
#include <TH1F.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

// BPhys helper scripts
#include "NtupleReader.h"

// TrigDiMuonCalib stuff
#include "Log.h"
#include "Utils.h"
#include "Chain.h"
#include "Details.h"
#include "AnalysisBase.h"
#include "HistogramHolder.h"
#include "Hits.h"
#include "Calib.h"
#include "SimpleJpsi.h"
#include "TrigHits.h"
#include "TrigDiMuonExtrapolator.h"
#include "SimpleMuon.h"
#include "CutsJpsi.h"

///===================================================
class TrigDiMuonRootCalib : public AnalysisBase, public HistogramHolder {

  public:
    /// constructor
    TrigDiMuonRootCalib(int olIn, std::vector<std::string> files) :
      AnalysisBase(olIn), HistogramHolder(olIn), m_currentEntry(-1),m_currentNEntries(-1), m_ntupleReader(""), m_nCandidates(0), m_nEntries(0)
    {
      Log log("TrigDiMuonRootCalib::TrigDiMuonRootCalib", ol());
      //load files
      loadFiles(files);
    }

    TrigDiMuonRootCalib(int olIn, std::string dirPath) :
        AnalysisBase(olIn), HistogramHolder(olIn), m_currentEntry(-1),m_currentNEntries(-1), m_ntupleReader(""), m_nCandidates(0), m_nEntries(0)
    {
      Log log("TrigDiMuonRootCalib::TrigDiMuonRootCalib", ol());
      //load files
      loadDirectory(dirPath);
    }

    /// analyzes

    void analyzeAllHitMatching(std::string chain,
                               std::string settings,
                               std::string cutStr);                     // analyze matching between hits and all trigger tracks (without offline muon confirmation, just like in the online monitoring)

    void convert(std::string settings, std::string cutStr);             // convert ntuple into a simple one
    void calibrate(std::string settings, std::string cutStr);           // calibrate TrigDiMuon propagator with offline muons
    void testExtrapolator(std::string chain);                           // test extrapolator

    // efficiency studies
    void convertChain(std::string chain,
                      std::string settings,
                      std::string cutStr);                              // analyze single chain accessing different stages of the algoritm
    void analyzeChain(std::string chain,
                      std::string settings,
                      std::string cutStr);                              // analyze single chain accessing different stages of the algoritm
    void analyzeRate(std::string chain,
                     std::string settings,
                     std::string cutStr);                   // calculate Level-1 rate fraction accepted by TrigDiMuon with different nHits cut


    /// entries iterators

    bool iterateDetails(std::string nameDetails,
                        TTree** tDetails,
                        Details** details,
                        std::string nameJpsi = "",
                        TTree** tJpsi = NULL,
                        ParticleBag* jpsi = NULL,
                        std::string nameTracks = "",
                        TTree** tTracks = NULL,
                        TrackBag* tracks = NULL,
                        std::string nameMuons = "",
                        TTree** tMuons = NULL,
                        MuonBag* muons = NULL);                         // iterate through all entries in the Details ntuple

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
                     Chain** trigChain = NULL,
                     std::string nameDetails = "",
                     TTree** tDetails = NULL,
                     Details** details = NULL );              // iterate through all the Jpsi entries reading requested objects from the ntuples

    bool iterateChain(std::string nameChain,
                      TTree** tChain,
                      Chain** trigChain,
                      std::string nameTrig = "",
                      TTree** tTrig = NULL,
                      TriggerBag* trigger = NULL,
                      std::string nameDetails = "",
                      TTree** tDetails = NULL,
                      Details** details = NULL,
                      std::string nameMuons = "",
                      TTree** tMuons = NULL,
                      MuonBag* muons = NULL,
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

    /// inline functions
    int  getCurrentEntry()    { return m_currentEntry; }
    int  getCurrentNEntries() { return m_currentNEntries; }

    int  getNCandidates()     { return m_nCandidates; }
    int  getNEntries()        { return m_nEntries; }

    void resetIterator() {
      AnalysisBase::resetIterator();
      m_currentEntry = -1;
      m_currentNEntries = -1;
      m_detailsEntry = -1;
    }

    TH1F* getHist(std::string name) { return HistogramHolder::getHist(name); }  // this is just to have this method in the dictionary

  private:
    // private attributes
    int                       m_currentEntry;             // current entry
    int                       m_currentNEntries;          // current number of entries
    NtupleReader              m_ntupleReader;             // b-physics helper tool

    int                       m_nCandidates;              // total no of selected candidates
    int                       m_nEntries;                 // total no of processed entries
    int                       m_detailsEntry;             // entry number for Details tree

    // histograming variables
    std::vector<double> m_hit_Endcap_mdt_inner_dEta [5];
    std::vector<double> m_hit_Endcap_mdt_middle_dEta[5];
    std::vector<double> m_hit_Barrel_mdt_inner_dEta [5];
    std::vector<double> m_hit_Barrel_mdt_middle_dEta[5];
    std::vector<double> m_hit_rpc_middle_dEta       [5];
    std::vector<double> m_hit_rpc_middle_dPhi       [5];
    std::vector<double> m_hit_tgc_wire_inner_dEta   [5];
    std::vector<double> m_hit_tgc_wire_middle_dEta  [5];
    std::vector<double> m_hit_tgc_wire_inner_dPhi   [5];
    std::vector<double> m_hit_tgc_wire_middle_dPhi  [5];
    std::vector<double> m_hit_tgc_strip_inner_dPhi  [5];
    std::vector<double> m_hit_tgc_strip_middle_dPhi [5];
    std::vector<double> m_hit_tgc_strip_middle2_dPhi[5];

    std::vector<double> m_hitOld_Endcap_mdt_inner_dEta ;
    std::vector<double> m_hitOld_Endcap_mdt_middle_dEta;
    std::vector<double> m_hitOld_Barrel_mdt_inner_dEta ;
    std::vector<double> m_hitOld_Barrel_mdt_middle_dEta;
    std::vector<double> m_hitOld_rpc_middle_dEta       ;
    std::vector<double> m_hitOld_rpc_middle_dPhi       ;
    std::vector<double> m_hitOld_tgc_wire_inner_dEta   ;
    std::vector<double> m_hitOld_tgc_wire_middle_dEta  ;
    std::vector<double> m_hitOld_tgc_wire_inner_dPhi   ;
    std::vector<double> m_hitOld_tgc_wire_middle_dPhi  ;
    std::vector<double> m_hitOld_tgc_strip_inner_dPhi  ;
    std::vector<double> m_hitOld_tgc_strip_middle_dPhi ;
    std::vector<double> m_hitOld_tgc_strip_middle2_dPhi ;

    // matching in different eta bins
    std::vector<double> m_hit2_tgc_wire_inner_dPhi   [4];
    std::vector<double> m_hit2_tgc_strip_inner_dPhi  [4];

    std::vector<double> m_hit2_Endcap_mdt_middle_dEta[6];

    std::vector<double> m_hit2_tgc_wire_middle_dEta  [6];
    std::vector<double> m_hit2_tgc_wire_middle_dPhi  [6];

    std::vector<double> m_hit2_tgc_strip_middle_dPhi [6] ;

    // histograming variables
    std::vector<double> m_jpsi;
    std::vector<double> m_jpsi_trk;
    std::vector<double> m_jpsi_fex;
    std::vector<double> m_jpsi_muID;
    std::vector<double> m_jpsi_vtx;

    std::vector<double> m_jpsi_pt;
    std::vector<double> m_jpsi_pt_trk;
    std::vector<double> m_jpsi_pt_fex;
    std::vector<double> m_jpsi_pt_muID;
    std::vector<double> m_jpsi_pt_vtx;

    std::vector<double> m_jpsi_eta;
    std::vector<double> m_jpsi_eta_trk;
    std::vector<double> m_jpsi_eta_fex;
    std::vector<double> m_jpsi_eta_muID;
    std::vector<double> m_jpsi_eta_vtx;

    std::vector<double> m_jpsi_dR;
    std::vector<double> m_jpsi_dR_trk;
    std::vector<double> m_jpsi_dR_fex;
    std::vector<double> m_jpsi_dR_muID;
    std::vector<double> m_jpsi_dR_vtx;

    std::vector<double> m_jpsi_chi2Prob;
    std::vector<double> m_jpsi_dPhi;

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
    std::vector<double> m_rate_mu_muID;
    std::vector<double> m_rate_diMu_muID;
    std::vector<double> m_rate_vtx;
    std::vector<double> m_rate_mu_vtx;
    std::vector<double> m_rate_diMu_vtx;

};
///===================================================
int nextEvent(int entry, TrigHits* trig);

///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
