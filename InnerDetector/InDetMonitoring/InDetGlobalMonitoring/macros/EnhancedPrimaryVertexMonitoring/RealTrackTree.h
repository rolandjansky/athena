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
  TString triggerName; ///< Trigger name used for filtering events
  Bool_t  m_mcAvailable;//whether to plot the MC-related histograms
  Long64_t m_maxEvents; //maximum number of events
  
 protected:
  TTree *trigMetaDataTree; ///< Store D3PD trigger tree
 
 public:
 
  RealTrackTree(TTree *tree, TTree* trigTree);
  void     Loop();

  TH1F*    define1DHistogram(TString name, unsigned int bins, Double_t lbin, Double_t ubin, TString xaxistitle, TString yaxistitle);
 
  TH2F*    define2DHistogram(TString name, unsigned int xbins, Double_t lxbin, Double_t uxbin, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle);

  // variable bin size
  TH2F*    define2DHistogram(TString name, unsigned int xbins, double* xrange, unsigned int ybins, Double_t lybin, Double_t uybin, TString xaxistitle, TString yaxistitle);
 
  Double_t   error_func(float x, Double_t *par);
 
  std::vector<float> stableGaussianFit(TH1 * histo);

  Float_t GetSplitMatchDistance(Int_t splitVxIndex, Int_t recoVxIndex, Int_t cfgMetric);
  enum VtxType {
    VtxSimulated, // MC vertex
    VtxReconstructedNoBC, // non-BC reco vertex
    VtxReconstructedBC, // BC reco vertex
    VtxReconstructedSplit // Split vertex
  };
  enum VtxDistanceMetric {
    VtxDistM_deltaZ, // Delta Z
    VtxDistM_deltaZsig, // Delta Z / sigma(Delta Z)
    VtxDistM_3Dsig // Quadratic sum of Delta x_i/Sigma(Delta x_i) (i=x,y,z)
  };
  Bool_t isGoodVertex(Int_t idx, VtxType vtxType);
  Float_t GetVtxDistance(Int_t VxIndex1, VtxType VxType1, Int_t VxIndex2, VtxType VxType2, VtxDistanceMetric cfgMetric);

  Bool_t isGoodRun(UInt_t run, UInt_t lb); ///< apply GRL

 private:
  TString m_outfileName;

 public:
  //MC based simulated-reconstructed vertex matching categories
  enum VtxMatch {
    VtxM_Match, //1-1 association
    VtxM_Split, //1-2
    VtxM_Merge, //2-1
    VtxM_Ineff, //1-0
    VtxM_Fake,  //0-1
    VtxM_Match_2, //2-2 association
    VtxM_Others, //other combinations
    VtxM_NMatch //Number of possibilities
  };
};

#endif
