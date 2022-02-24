/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_resITk
#define INDETPHYSVALMONITORING_InDetPerfPlot_resITk
/**
 * @file InDetPerfPlot_resITk.h
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
#include "TEfficiency.h"
#include <array>

// fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_resITk: public InDetPlotBase {
public:
  enum Param {
    D0, Z0, QOVERP, QOVERPT, THETA, PHI, PT, Z0SIN, NPARAMS
  };
  struct pCfg {
    std::string paraName;
    std::string paraLabel;
    std::string paraUnit;
    std::string resUnit;
    int nBinsRes;
    std::vector<float> limRes;
    int nBinsPrp;
    std::vector<float> limPrp;
    int nBinsSig;
    std::vector<float> limSig;
    // use loglinear binning? 
    bool useLogLinBins; 
  };


  InDetPerfPlot_resITk(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);

  void fillITkTailsPlots(bool filltail=false);
  void inResoFileForTailPlots(std::string resoFileName);

//  virtual bool isDefined(TString t);
  virtual ~InDetPerfPlot_resITk() {/** nop **/
  }

private:
  // defining global eta binning for resolution plots
  static const int m_nEtaBins = 64;
  const double  m_etaMin = -4.0;
  const double  m_etaMax = 4.0;
  static const int m_nPtBins = 9;
  static const int m_nResHist = 2;
  
  std::array<float, m_nEtaBins + 1> m_EtaBins = {};
  std::array<float, m_nPtBins + 1> m_PtBins = {0.0, 0.5, 0.7, 1.0, 2.0, 3.0, 5.0, 10.0, 20.0, 50.0};
  
  std::array<double, m_nPtBins + 1> m_PtBins_eff = {};
  
  std::array<std::string, m_nResHist> m_resHisto = {"resolutionRMS", "meanRMS"};
  
  IDPVM::ResolutionHelper m_resolutionHelper;
  IDPVM::ResolutionHelper::methods m_resolutionMethod;
  
  bool m_primTrk;
  bool m_secdTrk;
  bool m_allTrk;
  bool m_fillITkTailsPlots;
  std::string m_resoFileName;
  std::shared_ptr<TFile> m_resoFile;

  void initializePlots();
  void finalizePlots();

  void getMeanWidthResultsModUnits(TH1* p_input_hist, std::vector<float>& p_result,
				   IDPVM::ResolutionHelper::methods p_method);
  void getTrackParameters(const xAOD::TruthParticle& truthprt);
  void getTrackParameters(const xAOD::TrackParticle& truthprt);
  void getPlotParameters();
  void getPlots();
  void cloneHistogram(TH1D* h, TH1* hcopy);

  float m_trkP[NPARAMS];
  float m_truetrkP[NPARAMS];
  float m_trkErrP[NPARAMS];

  float m_resP[NPARAMS];
  float m_pullP[NPARAMS];
  float m_sigP[NPARAMS];

  pCfg m_paramProp[NPARAMS];

  TH1* m_resITk_pull[NPARAMS];
  TH1* m_resITk_res[NPARAMS];
  TH1* m_resITk_reco[NPARAMS];
  TH1* m_resITk_true[NPARAMS];
  TH1* m_resITk_sigma[NPARAMS];

  TH1* m_resITk_chargeID;
  TH1* m_resITk_chargeID_vs_eta;
  TH1* m_resITk_chargeID_vs_pt;
  TH1* m_resITk_chargeID_vs_phi;

  TH2* m_resITk_tail_vs_eta[NPARAMS];
  TH2* m_resITk_tail_vs_pt[NPARAMS];

  TEfficiency* m_resITk_tailfrac_vs_eta[NPARAMS];
  TEfficiency* m_resITk_tailfrac_vs_pt[NPARAMS];

  TH1* m_eta_tail[NPARAMS];
  TH1* m_pT_tail[NPARAMS];

  TH1* m_InnermostLayerHits_tail[NPARAMS];
  TH1* m_InnermostLayerOutliers_tail[NPARAMS];
  TH1* m_InnermostLayerSharedHits_tail[NPARAMS];
  TH1* m_InnermostLayerSplitHits_tail[NPARAMS];
  TH1* m_NextToInnermostLayerHits_tail[NPARAMS];
  TH1* m_NextToInnermostLayerOutliers_tail[NPARAMS];
  TH1* m_NextToInnermostLayerSharedHits_tail[NPARAMS];
  TH1* m_NextToInnermostLayerSplitHits_tail[NPARAMS];
  TH1* m_PixelHits_tail[NPARAMS];
  TH1* m_PixelHoles_tail[NPARAMS];
  TH1* m_PixelOutliers_tail[NPARAMS];
  TH1* m_PixelContribLayers_tail[NPARAMS];
  TH1* m_PixelSharedHits_tail[NPARAMS];
  TH1* m_PixelSplitHits_tail[NPARAMS];
  TH1* m_PixelGangedHits_tail[NPARAMS];
  TH1* m_PixelGangedHitsFlaggedFakes_tail[NPARAMS];
  TH1* m_SCTHits_tail[NPARAMS];
  TH1* m_SCTHoles_tail[NPARAMS];
  TH1* m_SCTOutliers_tail[NPARAMS];
  TH1* m_SCTDoubleHoles_tail[NPARAMS];
  TH1* m_SCTSharedHits_tail[NPARAMS];


  TH2* m_resITk_chargeID_chgvschg;

  TH2* m_resITk_resHelpereta[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta[NPARAMS][m_nResHist];
  TH1* m_resITk_ResProjections_vs_eta[NPARAMS][m_nEtaBins];

  TH2* m_resITk_resHelpereta_pos[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta_pos[NPARAMS][m_nResHist];
  TH2* m_resITk_resHelpereta_neg[NPARAMS];
  TH1* m_resITk_Resolution_vs_eta_neg[NPARAMS][m_nResHist];

  TH2* m_resITk_resHelperpt[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt[NPARAMS][m_nResHist];
  TH1* m_resITk_ResProjections_vs_pt[NPARAMS][m_nPtBins];

  TH2* m_resITk_resHelperpt_pos[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt_pos[NPARAMS][m_nResHist];
  TH2* m_resITk_resHelperpt_neg[NPARAMS];
  TH1* m_resITk_Resolution_vs_pt_neg[NPARAMS][m_nResHist];


//  TH3* m_resITk_resHelperetapt[NPARAMS];

  TH2* m_resITk_pullHelperpt[NPARAMS]; // pull width as a function of pT
  TH2* m_resITk_pullHelpereta[NPARAMS]; // pull width as a function of pT

  TH1* m_resITk_pullResolution_vs_pt[NPARAMS][m_nResHist];
  TH1* m_resITk_pullResolution_vs_eta[NPARAMS][m_nResHist];

  TH1* m_resITk_pullProjections_vs_pt[NPARAMS][m_nPtBins];
  TH1* m_resITk_pullProjections_vs_eta[NPARAMS][m_nEtaBins];


  TH1* m_resITk_Resolution_vs_pt_EtaBin[NPARAMS][4][m_nResHist];
  TH1* m_resITk_Resolution_vs_eta_PtBin[NPARAMS][4][m_nResHist];

  TProfile* m_resITk_meanProfeta[NPARAMS];
  TProfile* m_resITk_meanProfpt[NPARAMS];
  TH2* m_resITk_sigmaVsEta[NPARAMS];

  TH1* m_trk_chi2ndof;
  TProfile* m_trk_chi2ndof_vs_eta;
  TProfile* m_trk_chi2ndof_vs_totHits;
  TProfile* m_trk_chi2ndof_vs_totHits_prob;

  TH1* m_trk_chi2;
  TH1* m_trk_ndof;

  TH2* m_significance_d0;
  TH2* m_significance_z0;

  TH1* m_significance_d0_vs_eta;
  TH1* m_significance_z0_vs_eta;

};


#endif
