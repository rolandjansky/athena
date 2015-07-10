/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_res
#define INDETPHYSVALMONITORING_InDetPerfPlot_res
/**
 * @file InDetPerfPlot_res.h
 * @author shaun roe
**/


//std includes
#include <string>
#include <vector>
#include "TProfile.h"

//local includes

#include "TrkValHistUtils/PlotBase.h"

//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

//fwd declaration
class IToolSvc;
class IExtrapolator;


///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_res:public PlotBase {
public:

  InDetPerfPlot_res(PlotBase * pParent, const std::string & dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
  ~InDetPerfPlot_res(){/** nop **/}

  void SetEtaBinning(int trackEtaBins, float etaMin, float etaMax) { m_trackEtaBins = trackEtaBins; m_etaMin = etaMin; m_etaMax = etaMax; };
  void SetTruthEtaCut(int etaCut ) { m_truthEtaCut = etaCut; };
	
private:
  unsigned int m_trackEtaBins;
  int m_etaNBins;
  float m_etaMin, m_etaMax;
  float m_truthEtaCut;

  void initializePlots();
//  void finalizePlots();

  TProfile* m_mean_vs_eta_d0;
//  TProfile* m_res_vs_eta_d0; // Track Resolution: d_{0}versus #eta
  TProfile* m_mean_vs_eta_z0; // Track Measurement Bias: z_{0}versus #eta
//  TProfile* m_res_vs_eta_z0; // Track Resolution: z_{0}versus #eta
  TProfile* m_mean_vs_eta_phi; // Track Measurement Bias: #phiversus #eta
//  TProfile* m_res_vs_eta_phi; // Track Resolution: #phiversus #eta
  TProfile* m_mean_vs_eta_theta; // Track Measurement Bias: #thetaversus #eta
//  TProfile* m_res_vs_eta_theta; // Track Resolution: #thetaversus #eta
  TProfile* m_mean_vs_eta_z0st; // Track Measurement Bias: z_{0}*sin(#theta)versus #eta
//  TProfile* m_res_vs_eta_z0st; // Track Resolution: z_{0}*sin(#theta)versus #eta
  TProfile* m_mean_vs_eta_qopt; // Track Measurement Bias: 1/p_{T}versus #eta
//  TProfile* m_res_vs_eta_qopt; // Track Resolution: 1/p_{T}versus #eta

};


#endif
