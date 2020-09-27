/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_res
#define INDETPHYSVALMONITORING_InDetPerfPlot_res
/**
 * @file InDetPerfPlot_res.h
 * @author Max Baugh
 **/




// local includes


#include "InDetPlotBase.h"

// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetPhysValMonitoring/ResolutionHelper.h"

// std includes
#include <string>
#include <vector>

// fwd declaration
class TH1;
class TH2;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_res: public InDetPlotBase {
public:
  enum Param {
    D0, Z0, PHI, THETA, Z0SIN_THETA, QOPT, NPARAMS
  };
  InDetPerfPlot_res(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
  virtual ~InDetPerfPlot_res() {/** nop **/
  }

private:
  std::vector<TH2*> m_meanbasePlots;
  TH2* m_meanbase_vs_eta_d0;
  TH2* m_meanbase_vs_eta_z0;
  TH2* m_meanbase_vs_eta_phi;
  TH2* m_meanbase_vs_eta_theta;
  TH2* m_meanbase_vs_eta_z0_sin_theta;
  TH2* m_meanbase_vs_eta_qopt;

  std::vector<TH1*> m_meanPlots;
  TH1* m_resmean_vs_eta_d0;
  TH1* m_resmean_vs_eta_z0;
  TH1* m_resmean_vs_eta_phi;
  TH1* m_resmean_vs_eta_theta;
  TH1* m_resmean_vs_eta_z0_sin_theta;
  TH1* m_resmean_vs_eta_qopt;

  std::vector<TH1*> m_resoPlots;
  TH1* m_reswidth_vs_eta_d0;
  TH1* m_reswidth_vs_eta_z0;
  TH1* m_reswidth_vs_eta_phi;
  TH1* m_reswidth_vs_eta_theta;
  TH1* m_reswidth_vs_eta_z0_sin_theta;
  TH1* m_reswidth_vs_eta_qopt;

  std::vector<TH2*> m_mean_vs_ptbasePlots;
  TH2* m_base_vs_pt_d0;
  TH2* m_base_vs_pt_z0;
  TH2* m_base_vs_pt_phi;
  TH2* m_base_vs_pt_theta;
  TH2* m_base_vs_pt_z0_sin_theta;
  TH2* m_base_vs_pt_qopt;

  std::vector<TH1*> m_mean_vs_ptPlots;
  TH1* m_resmean_vs_pt_d0;
  TH1* m_resmean_vs_pt_z0;
  TH1* m_resmean_vs_pt_phi;
  TH1* m_resmean_vs_pt_theta;
  TH1* m_resmean_vs_pt_z0_sin_theta;
  TH1* m_resmean_vs_pt_qopt;

  std::vector<TH1*> m_resptPlots;
  TH1* m_reswidth_vs_pt_d0;
  TH1* m_reswidth_vs_pt_z0;
  TH1* m_reswidth_vs_pt_phi;
  TH1* m_reswidth_vs_pt_theta;
  TH1* m_reswidth_vs_pt_z0_sin_theta;
  TH1* m_reswidth_vs_pt_qopt;

  std::vector<TH1*> m_pullPlots;
  TH1* m_pull_d0;
  TH1* m_pull_z0;
  TH1* m_pull_phi;
  TH1* m_pull_theta;
  TH1* m_pull_z0_sin_theta;
  TH1* m_pull_qopt;

  std::vector<TH2*> m_pullbasePlots;
  TH2* m_pullbase_d0;
  TH2* m_pullbase_z0;
  TH2* m_pullbase_phi;
  TH2* m_pullbase_theta;
  TH2* m_pullbase_z0_sin_theta;
  TH2* m_pullbase_qopt;

  std::vector<TH1*> m_pullmeanPlots;
  TH1* m_pullmean_d0;
  TH1* m_pullmean_z0;
  TH1* m_pullmean_phi;
  TH1* m_pullmean_theta;
  TH1* m_pullmean_z0_sin_theta;
  TH1* m_pullmean_qopt;

  std::vector<TH1*> m_pullwidthPlots;
  TH1* m_pullwidth_d0;
  TH1* m_pullwidth_z0;
  TH1* m_pullwidth_phi;
  TH1* m_pullwidth_theta;
  TH1* m_pullwidth_z0_sin_theta;
  TH1* m_pullwidth_qopt;


  std::vector<std::string> m_paramNames;

  // class and methods for evaluating mean and width of distributions
  IDPVM::ResolutionHelper m_resolutionHelper;
  IDPVM::ResolutionHelper::methods m_resolutionMethod;
  
  void initializePlots();
  void Refinement(TH1D* temp, IDPVM::ResolutionHelper::methods p_method,
		  int var, int j, const std::vector<TH1*>& tvec,
		  const std::vector<TH1*>& rvec);
  void finalizePlots();

};


#endif
