/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_TRTEXTENSION
#define INDETPHYSVALMONITORING_INDETPERFPLOT_TRTEXTENSION
/**
 * @file InDetPerfPlot_TRTExtension.h
 * @author nora pettersson
 **/


// std includes
#include <string>
#include <array>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetPhysValMonitoring/ResolutionHelper.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_TRTExtension: public InDetPlotBase {
public:
  InDetPerfPlot_TRTExtension(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nVtx);

  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthie);

private:

  IDPVM::ResolutionHelper m_resolutionHelper;
  IDPVM::ResolutionHelper::methods m_resolutionMethod;


  TEfficiency* m_fracTRTExtensions_vs_eta;
  TEfficiency* m_fracTRTExtensions_vs_pt;
  TEfficiency* m_fracTRTExtensions_vs_mu;
  TEfficiency* m_fracTRTExtensions_vs_nvertices;

  TEfficiency* m_fracTRTExtensions_matched_vs_eta;
  TEfficiency* m_fracTRTExtensions_matched_vs_pt;

  TH1* m_chi2ndofTRTExtensions;
  TH1* m_chi2ndofNoTRTExtensions;

  TH2* m_ptresTRTExtensions_vs_eta;
  TH2* m_ptresTRTExtensions_vs_pt;
  TH2* m_ptresNoTRTExtensions_vs_eta;
  TH2* m_ptresNoTRTExtensions_vs_pt;

  TH1* m_reswidthTRTExtensions_vs_eta;
  TH1* m_resmeanTRTExtensions_vs_eta;
  TH1* m_reswidthTRTExtensions_vs_pt;
  TH1* m_resmeanTRTExtensions_vs_pt;

  TH1* m_reswidthNoTRTExtensions_vs_eta;
  TH1* m_resmeanNoTRTExtensions_vs_eta;
  TH1* m_reswidthNoTRTExtensions_vs_pt;
  TH1* m_resmeanNoTRTExtensions_vs_pt;

  TH2* m_ptpullTRTExtensions_vs_eta;
  TH2* m_ptpullTRTExtensions_vs_pt;
  TH2* m_ptpullNoTRTExtensions_vs_eta;
  TH2* m_ptpullNoTRTExtensions_vs_pt;

  TH1* m_pullwidthTRTExtensions_vs_eta;
  TH1* m_pullmeanTRTExtensions_vs_eta;
  TH1* m_pullwidthTRTExtensions_vs_pt;
  TH1* m_pullmeanTRTExtensions_vs_pt;

  TH1* m_pullwidthNoTRTExtensions_vs_eta;
  TH1* m_pullmeanNoTRTExtensions_vs_eta;
  TH1* m_pullwidthNoTRTExtensions_vs_pt;
  TH1* m_pullmeanNoTRTExtensions_vs_pt;


  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  // print out final stats on exact zeroes
  void finalizePlots();
};




#endif
