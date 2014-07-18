/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RealTrackTree_h
#define RealTrackTree_h

// Remark A. Wildauer: when recreating the base class with root MakeClass all you need to do is to add
// using namespace std;
// to the header.

#include "InDetTrackTree.h"
#include <TString.h>

class TH1F;
class TH2F;

class RealTrackTree : public InDetTrackTree
{
 
 public:
 
  RealTrackTree(TTree *tree=0, const TString& outfileName="outputFile.root");
  void     Loop();
 
  TH1F*    defineHistogram(TString name, unsigned int bins, Double_t lbin, Double_t ubin, TString xaxistitle, TString yaxistitle);
 
  TH2F*    define2DHistogram(TString name, unsigned int xbins, Double_t lxbin, Double_t uxbin, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle);
 
  bool     trigger_L1_MBTS_1();
 
  bool     good_run();
  
  bool     split_bin_test(unsigned int nbin);
 
  Double_t   error_func(float x, Double_t *par);
 
  std::vector<float> stableGaussianFit(TH1 * histo);

  void fitFailed();
 
 private:
  TString m_outfileName;
};

#endif
