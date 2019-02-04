/* emacs: this is -*- c++ -*- */
/**
 **     @file    Analysis_Missed.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_Analysis_Missed_H
#define TrigInDetAnalysisExample_Analysis_Missed_H


#include <iostream>

#include "TH1.h"
#include "TH2.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Missed : public TrackAnalysis { 
  
 public:

  using TrackAnalysis::execute;

 public:
  
  Analysis_Missed(const std::string& name);  
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();
  
 private:

  double deltaR(double eta1, double eta2, double phi1, double phi2);
  double phi(double p);

 private:
  
  // Track number histograms
  TH1* h_ntrack;
  TH1* h_ntrackfrac;

  // Track parameter histograms
  TH1* h_eta;
  TH1* h_phi;
  TH1* h_d0;
  TH1* h_z0;
  TH1* h_pT;
  TH1* h_chi2;
  
  // Track hit histograms
  TH1* h_blay;
  TH1* h_pixel;
  TH1* h_sct;
  TH1* h_silicon;
  TH1* h_straw;
  TH1* h_tr;
  TH1* h_patt;
  TH1* h_firstpatt;

  // Bidimensional histograms
  TH2* h_etaphi;
  TH2* h_etapatt;
  TH2* h_phipatt;
};


#endif // TrigInDetAnalysisExample_Analysis_Missed_H
