#ifndef IPRESOLSTUDIES_H
#define IPRESOLSTUDIES_H

// ROOT includes
#include "TCanvas.h"
#include "TColor.h"
#include "TError.h"
#include "TFile.h"
#include "TH3.h"
#include "TLorentzVector.h"
#include "TString.h"
#include "TVirtualFitter.h"

//using namespace RooFit ;

class IPResolStudies
{
  private:

  //// variables
  int m_printLevel;
  int m_EtaBins;
  float m_EtaLower, m_EtaUpper;
  int m_d0Bins;
  float m_d0Lower, m_d0Upper;
  int m_z0Bins;
  float m_z0Lower, m_z0Upper;
  int m_PtBins;
  float m_PtLower, m_PtUpper;
  double m_RunNumber;
  int m_Iteration;
  TFile * m_tfile;
  bool m_isMC_IPResol;
  float m_GaussianFitRange;

  /// 
  TH3 * h_d0_vs_pt_eta;
  TH3 * h_d0_vs_pt_eta_pos;
  TH3 * h_d0_vs_pt_eta_neg;
  TH3 * h_z0_vs_pt_eta;
  TH3 * h_z0_vs_pt_eta_pos;
  TH3 * h_z0_vs_pt_eta_neg;

  TH2 *h_entries_vs_pt_eta;

  // 
  TH2 *h_d0sigma;
  TH2 *h_d0sigma_pos;
  TH2 *h_d0sigma_neg;
  TH2 *h_z0sigma;
  TH2 *h_z0sigma_pos;
  TH2 *h_z0sigma_neg;

  TH3 * h_d0_vs_pt_eta_unfold;
  TH3 * h_d0_vs_pt_eta_pos_unfold;
  TH3 * h_d0_vs_pt_eta_neg_unfold;
  TH3 * h_z0_vs_pt_eta_unfold;
  TH3 * h_z0_vs_pt_eta_pos_unfold;
  TH3 * h_z0_vs_pt_eta_neg_unfold;

  TH2 *h_d0sigma_unfold;
  TH2 *h_d0sigma_pos_unfold;
  TH2 *h_d0sigma_neg_unfold;
  TH2 *h_z0sigma_unfold;
  TH2 *h_z0sigma_pos_unfold;
  TH2 *h_z0sigma_neg_unfold;

  TH3 * h_d0_vs_pt_eta_unfold_kpvup;
  TH3 * h_d0_vs_pt_eta_unfold_kpvdown;
  TH2 * h_d0sigma_unfold_kpvup;
  TH2 * h_d0sigma_unfold_kpvdown;

  TH3 * h_z0_vs_pt_eta_unfold_kpvup;
  TH3 * h_z0_vs_pt_eta_unfold_kpvdown;
  TH2 * h_z0sigma_unfold_kpvup;
  TH2 * h_z0sigma_unfold_kpvdown;

  TH3 * h_d0_vs_pt_eta_unfold_ktrkup;
  TH3 * h_d0_vs_pt_eta_unfold_ktrkdown;
  TH2 * h_d0sigma_unfold_ktrkup;
  TH2 * h_d0sigma_unfold_ktrkdown;

  TH3 * h_z0_vs_pt_eta_unfold_ktrkup;
  TH3 * h_z0_vs_pt_eta_unfold_ktrkdown;
  TH2 * h_z0sigma_unfold_ktrkup;
  TH2 * h_z0sigma_unfold_ktrkdown;

  TH2 * h_d0sigma_unfold_widerfit;
  TH2 * h_z0sigma_unfold_widerfit;


  //K_trk histograms
  TH2  *h_Ktrk_d0_pt_eta;
  TH2  *h_Ktrk_z0_pt_eta;
  
  //mean histograms
  TH2 *h_d0mean;
  TH2 *h_z0mean;
  TH2 *h_d0mean_unfold;
  TH2 *h_z0mean_unfold;
  TH2 *h_d0mean_neg;
  TH2 *h_z0mean_neg;
  TH2 *h_d0mean_neg_unfold;
  TH2 *h_z0mean_neg_unfold;
  TH2 *h_d0mean_pos;
  TH2 *h_z0mean_pos;
  TH2 *h_d0mean_pos_unfold;
  TH2 *h_z0mean_pos_unfold;
  TH2 *h_d0mean_unfold_kpvup;
  TH2 *h_d0mean_unfold_kpvdown;
  TH2 *h_z0mean_unfold_kpvup;
  TH2 *h_z0mean_unfold_kpvdown;
  TH2 *h_d0mean_unfold_ktrkup;
  TH2 *h_d0mean_unfold_ktrkdown;
  TH2 *h_z0mean_unfold_ktrkup;
  TH2 *h_z0mean_unfold_ktrkdown;

  TH2 *h_d0mean_unfold_widerfit;
  TH2 *h_z0mean_unfold_widerfit;

  //chi2 histograms
  TH2 *h_d0chi2;
  TH2 *h_z0chi2;
  TH2 *h_d0chi2_unfold;
  TH2 *h_z0chi2_unfold;
  TH2 *h_d0chi2_neg;
  TH2 *h_z0chi2_neg;
  TH2 *h_d0chi2_neg_unfold;
  TH2 *h_z0chi2_neg_unfold;
  TH2 *h_d0chi2_pos;
  TH2 *h_z0chi2_pos;
  TH2 *h_d0chi2_pos_unfold;
  TH2 *h_z0chi2_pos_unfold;
  TH2 *h_d0chi2_unfold_kpvup;
  TH2 *h_d0chi2_unfold_kpvdown;
  TH2 *h_z0chi2_unfold_kpvup;
  TH2 *h_z0chi2_unfold_kpvdown;
  TH2 *h_d0chi2_unfold_ktrkup;
  TH2 *h_d0chi2_unfold_ktrkdown;
  TH2 *h_z0chi2_unfold_ktrkup;
  TH2 *h_z0chi2_unfold_ktrkdown;

  TH2 *h_d0chi2_unfold_widerfit;
  TH2 *h_z0chi2_unfold_widerfit;


  // track parameters
  TLorentzVector m_mupos_4mom;
  double m_mupos_d0, m_mupos_z0; 
  double m_mupos_d0Err, m_mupos_z0Err;
  TLorentzVector m_muneg_4mom;
  double m_muneg_d0, m_muneg_z0; 
  double m_muneg_d0Err, m_muneg_z0Err;

  // primary vertex parameters
  double m_PV_d0Sigma_pos, m_PV_d0Sigma_neg;
  double m_PV_z0Sigma_pos, m_PV_z0Sigma_neg;

  // functions
  float ComputeUnfoldFactor(float K_PV, float K_trk, float sigma_PV, float sigma_trk);
  void  DrawHistos ();
  void  FormVectorEtaValues (float *);
  void  FormVectord0Values (float *);
  void  FormVectorz0Values (float *);
  void  FormVectorPtValues (float *);
  float GetKPVFactor (int RunNumber, int trkpar, int selectCase);
  float GetKtrkFactor (float pt, float eta, int trkpar, int selectCase);

  bool  IterativeGaussianFit (TH1D *);

  /// public member functions
 public:
  IPResolStudies();
  ~IPResolStudies();

  void          BookHistograms ();
  void          Execute ();
  void          FillHistograms ();
  inline TH2 *  GetHistoKTrkd0Factors () {return h_Ktrk_d0_pt_eta;}
  inline TH2 *  GetHistoKTrkz0Factors () {return h_Ktrk_z0_pt_eta;}
  inline TH2 *  GetHistoPtEtaEntries () {return h_entries_vs_pt_eta;}
  inline TH2 *  GetHistoRecod0Chi2 () {return h_d0chi2;}
  inline TH2 *  GetHistoRecoz0Chi2 () {return h_z0chi2;}
  inline TH2 *  GetHistoRecod0Chi2Unfold () {return h_d0chi2_unfold;}
  inline TH2 *  GetHistoRecoz0Chi2Unfold () {return h_z0chi2_unfold;}
  inline TH2 *  GetHistoRecod0Mean () {return h_d0mean;}
  inline TH2 *  GetHistoRecoz0Mean () {return h_z0mean;}
  inline TH2 *  GetHistoRecod0MeanUnfold () {return h_d0mean_unfold;}
  inline TH2 *  GetHistoRecoz0MeanUnfold () {return h_z0mean_unfold;}
  inline TH2 *  GetHistoRecod0Sigma () {return h_d0sigma;}
  inline TH2 *  GetHistoRecod0SigmaMuPos () {return h_d0sigma_pos;}
  inline TH2 *  GetHistoRecod0SigmaMuNeg () {return h_d0sigma_neg;}
  inline TH2 *  GetHistoRecod0SigmaMuPosUnfold () {return h_d0sigma_pos_unfold;}
  inline TH2 *  GetHistoRecod0SigmaMuNegUnfold () {return h_d0sigma_neg_unfold;}
  inline TH2 *  GetHistoRecod0SigmaUnfold () {return h_d0sigma_unfold;}
  inline TH2 *  GetHistoRecod0SigmaUnfoldKPVup () {return h_d0sigma_unfold_kpvup;}
  inline TH2 *  GetHistoRecod0SigmaUnfoldKPVdown () {return h_d0sigma_unfold_kpvdown;}
  inline TH2 *  GetHistoRecod0SigmaUnfoldKtrkup () {return h_d0sigma_unfold_ktrkup;}
  inline TH2 *  GetHistoRecod0SigmaUnfoldKtrkdown () {return h_d0sigma_unfold_ktrkdown;}
  inline TH2 *  GetHistoRecod0SigmaUnfoldWideFit () {return h_d0sigma_unfold_widerfit;}
  inline TH2 *  GetHistoRecoz0Sigma () {return h_z0sigma;}
  inline TH2 *  GetHistoRecoz0SigmaMuPos () {return h_z0sigma_pos;}
  inline TH2 *  GetHistoRecoz0SigmaMuNeg () {return h_z0sigma_neg;}
  inline TH2 *  GetHistoRecoz0SigmaMuPosUnfold () {return h_z0sigma_pos_unfold;}
  inline TH2 *  GetHistoRecoz0SigmaMuNegUnfold () {return h_z0sigma_neg_unfold;}
  inline TH2 *  GetHistoRecoz0SigmaUnfold () {return h_z0sigma_unfold;}
  inline TH2 *  GetHistoRecoz0SigmaUnfoldKPVup () {return h_z0sigma_unfold_kpvup;}
  inline TH2 *  GetHistoRecoz0SigmaUnfoldKPVdown () {return h_z0sigma_unfold_kpvdown;}
  inline TH2 *  GetHistoRecoz0SigmaUnfoldKtrkup () {return h_z0sigma_unfold_ktrkup;}
  inline TH2 *  GetHistoRecoz0SigmaUnfoldKtrkdown () {return h_z0sigma_unfold_ktrkdown;}
  inline TH2 *  GetHistoRecoz0SigmaUnfoldWideFit () {return h_z0sigma_unfold_widerfit;}

  inline void   SetGaussianFitRange (float newrange) {m_GaussianFitRange = newrange;}
  void          SetInputIteration(int iteration); 
  void          SetMuPosTrackparams (TLorentzVector * newfourmom, double newd0, double newz0, double newd0Err, double newz0Err);
  void          SetMuNegTrackparams (TLorentzVector * newfourmom, double newd0, double newz0, double newd0Err, double newz0Err);
  void          SetPVParams (double possigmad0, double possigmaz0, double negsigmad0, double negsigmaz0);
  void          SetRunNumber (double runnumber);
  inline void   SetIsMC (bool newmc) {m_isMC_IPResol = newmc;}
  inline void   SetTFile (TFile * newTFile) {m_tfile = newTFile;}

  //inline TH2 *  GetHistoInvMassFitError () {return h_inv_mass_vs_etaphi_fiterror;}

  enum {D0, Z0};
  enum {CENTRAL_VALUE, UNCERT_UP, UNCERT_DOWN};

};
#endif

