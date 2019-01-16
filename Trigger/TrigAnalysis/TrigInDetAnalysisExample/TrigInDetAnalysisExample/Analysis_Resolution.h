/* emacs: this is -*- c++ -*- */
/**
 **     @file    Analysis_Resolution.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_Analysis_Resolution_H
#define TrigInDetAnalysisExample_Analysis_Resolution_H


#include <iostream>

#include "TH1.h"
#include "TH2.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Resolution : public TrackAnalysis { 

 public:

  using TrackAnalysis::execute;
  
 public:
  
  // Analysis_Resolution(const std::string& name) : TrackAnalysis(name) {}  
 Analysis_Resolution(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);  


  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();
  
 private:

  double phi(double p);

 private:
  
  // Resolution Histograms
  TH1* h_res_eta;
  TH1* h_res_phi;
  TH1* h_res_d0;
  TH1* h_res_z0;
  TH1* h_res_invpT;
  
  // Pull Histograms
  TH1* h_pull_eta;
  TH1* h_pull_phi;
  TH1* h_pull_d0;
  TH1* h_pull_z0;
  TH1* h_pull_invpT;

  // 2D histograms
  TH2* h_res_d0VsPt; 
  TH2* h_res_z0VsPt;
  TH2* h_res_d0VsD0;
  TH2* h_res_z0VsEta;
  TH2* h_res_d0VsPixelHits_withBLayer;
  TH2* h_res_d0VsPixelHits_withoutBLayer;
  TH2* h_pull_d0VsEta;
  TH2* h_pull_z0VsEta;
  TH2* h_pull_d0VsPixelHits_withoutBLayer;
  TH2* h_pull_z0VsPixelHits_withoutBLayer;
  TH2* h_pull_d0VsPixelHits_withBLayer;
  TH2* h_pull_z0VsPixelHits_withBLayer;

  // Efficiency platau cuts
  double m_pTCut;
  double m_etaCut;
  double m_d0Cut;
  double m_z0Cut;




};


#endif // TrigInDetAnalysisExample_Analysis_Resolution_H
