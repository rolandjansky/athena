/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_Resolution
#define INDETPHYSVALMONITORING_InDetPerfPlot_Resolution
/**
 * @file InDetPerfPlot_Resolution.h
 * @author nora pettersson
 **/





// local includes

#include "InDetPlotBase.h"
#include "TProfile.h"
// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetPlotBase.h"
#include "InDetPhysValMonitoring/ResolutionHelper.h"

#include "TFitResultPtr.h"
#include "TFitResult.h"
// std includes
#include <string>
#include <vector>
#include <map>

// fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_Resolution: public InDetPlotBase {
public:
  enum Param {
    D0, Z0, QOVERPT, THETA, PHI, PT, Z0SIN, NPARAMS
  };

  InDetPerfPlot_Resolution(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
//  virtual bool isDefined(TString t);
  virtual ~InDetPerfPlot_Resolution() {/** nop **/
  }

private:
  // defining global eta binning for resolution plots
  static const int m_nResHist = 4;

  static const int m_nEtaBins = 64;
  //const double  m_etaMin = -4.0;
  //const double  m_etaMax = 4.0;

// Settings for log-linear binning in pt (matching previous IDPVM definition)
  static const int m_nPtBins = 49;
  const double  m_ptMin = 0.745;
  const double  m_ptMax = 1000.0;

  float m_PtBins[m_nPtBins + 1];
   
  std::string m_paramProp[NPARAMS] = { "d0", "z0", "ptqopt", "theta", "phi", "pt", "z0sin" }; 
 
  IDPVM::ResolutionHelper m_resolutionHelper;
  IDPVM::ResolutionHelper::methods m_resolutionMethod;
  
  bool m_primTrk;
  bool m_secdTrk;
  bool m_allTrk;

  void initializePlots();
  void finalizePlots();


  void getTrackParameters(const xAOD::TruthParticle& truthprt);
  void getTrackParameters(const xAOD::TrackParticle& truthprt);
  void getPlotParameters();
  void getPlots();

  float m_trkP[NPARAMS];
  float m_truetrkP[NPARAMS];
  float m_trkErrP[NPARAMS];

  float m_resP[NPARAMS];
  float m_pullP[NPARAMS];
  float m_sigP[NPARAMS];

  TH1* m_pull[NPARAMS];
  TH1* m_res[NPARAMS];
  TH1* m_sigma[NPARAMS];

  TH2* m_resHelpereta[NPARAMS];
  TH2* m_resHelperpt[NPARAMS];
  TH2* m_pullHelpereta[NPARAMS]; 
  TH2* m_pullHelperpt[NPARAMS]; 

  TH1* m_reswidth_vs_eta[NPARAMS];
  TH1* m_resmean_vs_eta[NPARAMS];
  TH1* m_reswidth_vs_pt[NPARAMS];
  TH1* m_resmean_vs_pt[NPARAMS];

  TH1* m_pullwidth_vs_eta[NPARAMS];
  TH1* m_pullmean_vs_eta[NPARAMS];
  TH1* m_pullwidth_vs_pt[NPARAMS];
  TH1* m_pullmean_vs_pt[NPARAMS];

  TH2* m_resHelpereta_pos[NPARAMS];
  TH2* m_resHelpereta_neg[NPARAMS];
  TH2* m_resHelperpt_pos[NPARAMS]; 
  TH2* m_resHelperpt_neg[NPARAMS];

  TH1* m_reswidth_vs_eta_pos[NPARAMS];
  TH1* m_resmean_vs_eta_pos[NPARAMS];
  TH1* m_reswidth_vs_pt_pos[NPARAMS];
  TH1* m_resmean_vs_pt_pos[NPARAMS];

  TH1* m_reswidth_vs_eta_neg[NPARAMS];
  TH1* m_resmean_vs_eta_neg[NPARAMS];
  TH1* m_reswidth_vs_pt_neg[NPARAMS];
  TH1* m_resmean_vs_pt_neg[NPARAMS];

  TH1* m_pullProjections_vs_pt[NPARAMS][m_nPtBins];
  TH1* m_pullProjections_vs_eta[NPARAMS][m_nEtaBins];
  TH1* m_resProjections_vs_pt[NPARAMS][m_nPtBins];
  TH1* m_resProjections_vs_eta[NPARAMS][m_nEtaBins];

  TProfile* m_sigma_vs_eta[NPARAMS];
  TProfile* m_sigma_vs_pt[NPARAMS];

};


#endif
