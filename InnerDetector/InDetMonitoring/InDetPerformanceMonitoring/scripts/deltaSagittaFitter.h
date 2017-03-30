#ifndef DELTASAGITTAFITTER_H
#define DELTASAGITTAFITTER_H

#include <iostream>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH3F.h"
#include "TLorentzVector.h"
#include "TMinuit.h"
#include "TPad.h"
#include "TString.h"
#include "TLatex.h"


///////////////////////////////////////////////////////////////
class deltaSagittaFitter
{
  ////////////////////////
 private:
  
  // variables
  int m_printLevel;
  int m_NetaBins;
  int m_NphiBins;
  int m_NptBins;

  double m_fittedDeltaSagitta;

  bool m_CurrentStatus;

  TString m_outputFileName;

  double m_lastDeltaSagitta;
  int    m_callCount;
  int    m_eventCount;
  int    m_usedMethod;
  int    m_thisPass;

  std::vector<float> v_deltaSagitta_testValues;
  std::vector<TH3F*> v_h3DPtPosMu;
  std::vector<TH3F*> v_h3DPtNegMu;

  bool m_saveOutputGraphics;

  TH3* h_3DPtPosMu; 
  TH3* h_3DPtNegMu;
  TH1* h_PtPosMu; // store the reco pt distributions 
  TH1* h_PtNegMu;
  TH1* h_PtPosMu_corr; // to store the corrected pt distributions for a given deltaSagitta
  TH1* h_PtNegMu_corr;
  TH1* h_PtPosMu_corr_aux; // auxiliar h_PtPosMu_corr counting entries per bin
  TH1* h_PtNegMu_corr_aux;
  TH1* h_PosMinusNeg;
  TH1* h_Target;
  TH2* h_deltaSagittaMap;

  // private member functions
  void BuildHTarget ();
  TH1* ComputeDifference2 ();
  TH1* ComputeDifference ();
  TH1* ComputeRatio (TH1 *, TH1 *);
  void FitEtaPhiCell (int etabin, int phibin);
  TH1* ProjectPt (TH3* inputhisto, int etabin, int phibin, int charge, TString theOptions = "");

  /// public member functions
 public:
  deltaSagittaFitter();
  ~deltaSagittaFitter();

  double        DiffVsDeltaSaggitaFunction(Double_t *v, Double_t *par);
  void          Execute ();
  void          Fill (float pxpos, float pypos, float pzpos, float mpos, float pxneg, float pyneg, float pzneg, float mneg);
  inline TH2 *  GetDeltaSagittaMap () {return h_deltaSagittaMap;}
  void          Initialization (TH3 * sampleHist);
  void          PrepareIteration (int iternumber);
  void          SetInputHistograms (TH3 * newth3pos, TH3 * newth3neg);
  inline bool   Status () {return m_CurrentStatus;}

  enum {QNEG=-1, QPOS=1};
  enum {CHISQUARE=0, CUMULATIVE=1, PTPEAK=2};
};

#endif
