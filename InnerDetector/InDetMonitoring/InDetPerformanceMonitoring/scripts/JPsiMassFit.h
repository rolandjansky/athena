#ifndef JPSIMASSFIT_H
#define JPSIMASSFIT_H

// ROOT includes
#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMinuit.h"
#include "TString.h"
//#include "TF1Convolution.h"

// RooFit includes
#include "RooAddPdf.h"
#include "RooArgusBG.h"
#include "RooCBShape.h"
#include "RooExponential.h"
#include "RooChebychev.h"
#include "RooChi2Var.h"
#include "RooDataHist.h"
#include "RooFFTConvPdf.h"
#include "RooFitResult.h"
#include "RooMinuit.h"
#include "RooGaussian.h"
#include "RooNovosibirsk.h"
#include "RooPlot.h"
#include "RooPolynomial.h"
#include "RooRealVar.h"
#include "RooGaussModel.h"
#include "RooAddModel.h"

// Especial PDF for the Z line shape fit
#include "RooEggePdf.cxx"

double JPsiFit(Double_t *v, Double_t *par);
double UpsilonFit(Double_t *v, Double_t *par);
double ZFit(Double_t *v, Double_t *par);
double RooEgge(Double_t *v, Double_t *par);

//using namespace RooFit ;

class JPsiMassFit
{
  private:

  //// variables
  int m_printLevel;
  int m_analysisMode;
  int m_theMuonCharge;
  bool m_isMC;

  double m_massError;
  double m_fitSigma;
  double m_reference_mass;
  double m_chi2overndf;

  bool m_theMuonChargeSetByUser;
  bool m_saveFitsInOutputFile;
  TString m_outputFileName;
  TString m_histogramUserTitle;

  // mass range for fitting
  double m_massLowerValueForFit;
  double m_massUpperValueForFit;

  //double m_JPsi1SMCbias; // MeV 
  // std::vector<double> m_JPsi1SMCbias;

  const Double_t JPsi1SPDGMass = 3.096916; //PDG 2010 in GeV in GeV
  const Double_t JPsi2SPDGMass = 3.686109;
  const double KShortPDGMass = 0.497611;
  const double Upsilon1SPDGMass = 9.46030;
  const double Upsilon2SPDGMass = 10.02326;
  const double Upsilon3SPDGMass = 10.3552;
  const double ZPDGMass = 91.1876; 
  const double GeVtoMeV = 1000.;
  const double MeVtokeV = 1000.;
  const double MeVtoGeV = 0.001;
  const int    MinEntriesInHistoForFit = 100;
  

  TString m_AnalysisModeKeyWord[4];

  /// 
  TCanvas* m_Can_m_per_etaphi;
  TCanvas* m_Can_m_per_barrel;
  //TCanvas* m_Can_m_per_jpsi1smean;
  
  /// 
  TH3 * h_inv_mass_vs_etaphi;
  TH2 * h_inv_mass_vs_etaphi_fitresult;
  TH2 * h_inv_mass_vs_etaphi_fiterror;
  TH2 * h_inv_mass_vs_etaphi_fitresultVsPDG;
  TH2 * h_inv_mass_vs_etaphi_fitresultVsPDGpc; // same as above but in %
  TH2 * h_inv_mass_vs_etaphi_fitsigma;
  TH2 * h_inv_mass_vs_etaphi_chi2overndf;
  TH2 * h_mass_shift_significance_vs_etaphi;

  // histograms passed by other 
  TH2 * h_mcmassbiasmap;

  // functions
  void    SetHumanReadableKeys ();
  TH2 *   BuildHistogramForMassErrorMap ();
  TH2 *   BuildHistogramForMassMap ();
  TH2 *   BuildHistogramForMassMapVsPDG ();
  TH2 *   BuildHistogramForMassMapVsPDGpc ();
  TH2 *   BuildHistogramForMassShiftSignificance ();
  TH2 *   BuildHistogramForMassSigmaMap ();
  TH2 *   BuildHistogramForMassChi2();
  double* EstimatePol2Coeff_JPsiFit(TH1*);
  double  FitInvMassDistribution (TH1*);
  double  FitJPsiMassDistributionRooFit (TH1*);
  double  FitJPsiMassDistribution (TH1*);
  double  FitUpsilonMassDistribution (TH1*);
  double  FitZMassDistribution (TH1*);
  //double  NEW_FitZMassDistribution (TH1*, float theExtraVal = 0.9);
  double  FitZMassDistributionROOT (TH1*);
  double  FitKShortMassDistribution (TH1*);
  double  FindPeakValue (TH1*);
  void    HistogramPreConditioning (TH1*);
  void    SetOutputFileName ();
  //double UpsilonFit (double *, double *);

  /// public member functions
 public:
  JPsiMassFit();
  ~JPsiMassFit();

  void          Execute ();
  inline TH2 *  GetHistoInvMassFitError () {return h_inv_mass_vs_etaphi_fiterror;}
  inline TH2 *  GetHistoInvMassFitResult () {return h_inv_mass_vs_etaphi_fitresult;}
  inline TH2 *  GetHistoInvMassFitResultVsPDG () {return h_inv_mass_vs_etaphi_fitresultVsPDG;}
  inline TH2 *  GetHistoInvMassFitResultVsPDGpc () {return h_inv_mass_vs_etaphi_fitresultVsPDGpc;} // same as above but in %
  inline TH2 *  GetHistoInvMassFitSigma () {return h_inv_mass_vs_etaphi_fitsigma;}
  inline TH2 *  GetHistoInvMassChi2overNdf(){return h_inv_mass_vs_etaphi_chi2overndf;}
  inline TH2 *  GetHistoMassShiftSignificance () {return h_mass_shift_significance_vs_etaphi;}
  inline double GetReferenceMass () {return m_reference_mass;}
  double        GetTargetMass (int, int);
  void          SetAnalysisMode (int);
  inline void   SetHistogramTitle (TString newtitle) {m_histogramUserTitle.Clear(); m_histogramUserTitle.Append(newtitle.Data());}
  void          SetInputHistogram (TH3 * newth3);
  inline void   SetMC (bool newmc) {m_isMC = newmc;}
  // inline void   SetMCbias (std::vector <double>  newmcbias) {m_JPsi1SMCbias = newmcbias;}
  inline void   SetMCbias (TH2* hbiasmap) {h_mcmassbiasmap = hbiasmap;}
  inline void   SetMuonCharge (int newCharge) {m_theMuonCharge = newCharge; m_theMuonChargeSetByUser = true;}
  inline void   SetPrintLevel (int newprintlevel) {if (newprintlevel < 0) newprintlevel=0; m_printLevel = newprintlevel;}

  enum {ZMASS, JPSIMASS, UPSILONMASS, DIMUONMASS, KSHORTMASS};

  double  NEW_FitZMassDistribution (TH1*, float inputSigma=1.5, float narrowGW=0.75, float sigmaWF=2.5, float theAzls=8.0, float theBzls=-2.8);

};
#endif

