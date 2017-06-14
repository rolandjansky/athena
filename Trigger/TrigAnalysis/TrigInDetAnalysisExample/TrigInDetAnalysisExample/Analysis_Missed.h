/** @file Analysis_Missed.h */

#ifndef TrigInDetAnalysisExample_Analysis_Missed_H
#define TrigInDetAnalysisExample_Analysis_Missed_H


#include <iostream>

#include "TH1.h"
#include "TH2.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Missed : public TrackAnalysis { 
  
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
  TH1* m_h_ntrack;
  TH1* m_h_ntrackfrac;

  // Track parameter histograms
  TH1* m_h_eta;
  TH1* m_h_phi;
  TH1* m_h_d0;
  TH1* m_h_z0;
  TH1* m_h_pT;
  TH1* m_h_chi2;
  
  // Track hit histograms
  TH1* m_h_blay;
  TH1* m_h_pixel;
  TH1* m_h_sct;
  TH1* m_h_silicon;
  TH1* m_h_straw;
  TH1* m_h_tr;
  TH1* m_h_patt;
  TH1* m_h_firstpatt;

  // Bidimensional histograms
  TH2* m_h_etaphi;
  TH2* m_h_etapatt;
  TH2* m_h_phipatt;
};


#endif // TrigInDetAnalysisExample_Analysis_Missed_H
