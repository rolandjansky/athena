/* emacs: this is -*- c++ -*- */
/**
 **     @file    Analysis_ResolutionCosmics.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_Analysis_ResolutionCosmics_H
#define TrigInDetAnalysisExample_Analysis_ResolutionCosmics_H


#include <iostream>

#include "TH1.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_ResolutionCosmics : public TrackAnalysis { 

 public:
  
  using TrackAnalysis::execute;
  
 public:
  
  Analysis_ResolutionCosmics(const std::string& name);
  
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

};


#endif // TrigInDetAnalysisExample_Analysis_ResolutionCosmics_H
