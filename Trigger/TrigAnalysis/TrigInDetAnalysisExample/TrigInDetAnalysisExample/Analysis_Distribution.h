/** @file Analysis_Resolution.h */

#ifndef TrigInDetAnalysisExample_Analysis_Distribution_H
#define TrigInDetAnalysisExample_Analysis_Distribution_H


#include <iostream>

#include "TH1.h"

#include "TrigInDetAnalysis/TrackAnalysis.h"


class Analysis_Distribution : public TrackAnalysis { 
  
 public:
  
  Analysis_Distribution(const std::string& name);
  //Analysis_Distribution(const std::string& name, double pTCut, double etaCut, double d0Cut, double z0Cut);  
  
  virtual void initialise();
  
  virtual void execute(const std::vector<TIDA::Track*>& referenceTracks,
		       const std::vector<TIDA::Track*>& testTracks,
		       TrackAssociator* associator);
  
  virtual void finalise();
  
 private:

  double deltaR(double eta1, double eta2, double phi1, double phi2);
  double phi(double p);

 private:
  
  // Track parameter histograms
  TH1* h_eta;
  TH1* h_phi;
  TH1* h_d0;
  TH1* h_z0;
  TH1* h_pT;
  
  // Track hit histograms
  TH1* h_blay;
  TH1* h_pixel;
  TH1* h_sct;
  TH1* h_silicon;
  TH1* h_silicon_match;
  TH1* h_straw;
  TH1* h_tr;

};


#endif // TrigInDetAnalysisExample_Analysis_Distribution_H
