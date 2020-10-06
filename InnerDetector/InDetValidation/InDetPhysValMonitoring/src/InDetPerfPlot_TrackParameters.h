/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_TRACKPARAMETERS
#define INDETPHYSVALMONITORING_INDETPERFPLOT_TRACKPARAMETERS
/**
 * @file InDetPerfPlot_TrackParameters.h
 * @author nora pettersson
 **/


// std includes
#include <string>
#include <array>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_TrackParameters: public InDetPlotBase {
public:
  InDetPerfPlot_TrackParameters(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TruthParticle& particle);
private:

  TH1* m_reco_d0;
  TH1* m_reco_z0;
  TH1* m_reco_z0sin;
  TH1* m_reco_phi;
  TH1* m_reco_theta;
  TH1* m_reco_eta;
  TH1* m_reco_qoverp;
  TH1* m_reco_pt;
  TH1* m_reco_lowpt;
  TH1* m_reco_chi2;
  TH1* m_reco_ndof;
  TH1* m_reco_chi2Overndof;
  TH1* m_reco_author;

  TH1* m_truth_d0;
  TH1* m_truth_z0;
  TH1* m_truth_z0sin;
  TH1* m_truth_phi;
  TH1* m_truth_theta;
  TH1* m_truth_eta;
  TH1* m_truth_qoverp;
  TH1* m_truth_pt;
  TH1* m_truth_lowpt;
  TH1* m_truth_prodR;
  TH1* m_truth_prodZ;

  TH2* m_reco_pt_vs_eta;
  TH2* m_reco_phi_vs_eta;

  TH2* m_reco_d0_z0;
  TH2* m_reco_d0_z0sin;

  TH2* m_truth_pt_vs_eta;
  TH2* m_truth_phi_vs_eta;



  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  // print out final stats on exact zeroes
  void finalizePlots();
};




#endif
