/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_DUPLICATE
#define INDETPHYSVALMONITORING_INDETPERFPLOT_DUPLICATE
/**
 * @file InDetPerfPlot_Pt.h
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "InDetPhysValMonitoring/IHistogramDefinitionSvc.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"



///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_duplicate: public InDetPlotBase {
public:
  InDetPerfPlot_duplicate(InDetPlotBase* pParent, const std::string& dirName);
  //	void fill1(const xAOD::TrackParticle_v1* & trackParticle);
  void fillSingleMatch(const xAOD::TrackParticle& trackParticle);
  //	void fillTwoMatch(Float_t prob1,Float_t prob2,const xAOD::TrackParticle & particle1,const xAOD::TrackParticle &
  // particle2);
  void fillTwoMatchDuplicate(const float prob1, const float prob2, const xAOD::TrackParticle& trackParticle,
                             const xAOD::TrackParticle& particle, const xAOD::TruthParticle& tp);
private:
  ///Pt Histogram with full pt range
  TH1* m_duplicateDeltaPt;
  TH1* m_duplicateDeltaPtZoomed;
  TH1* m_duplicateDeltaEta;
  TH1* m_duplicateDeltaPhi;
  TH2* m_duplicateLPTvsHPT;
  TH2* m_duplicateLEtavsHEta;
  TH2* m_duplicateLPhivsHPhi;
  TH2* m_duplicateDeltaPTvsTruthPT;
  TH2* m_duplicateDeltaPTvsTruthPTZoomed;
  // spectrum plots
  TH1* m_duplicateLPT;
  TH1* m_duplicateLEta;
  TH1* m_duplicateLPhi;
  TH1* m_duplicateHPT;
  TH1* m_duplicateHEta;
  TH1* m_duplicateHPhi;
  TH1* m_duplicateTruthPT;
  TH1* m_duplicateTruthEta;
  TH1* m_duplicateTruthPhi;

  // resolution plots
  TH1* m_duplicateResLPT;
  TH1* m_duplicateResLEta;
  TH1* m_duplicateResLPhi;
  TH1* m_duplicateResHPT;
  TH1* m_duplicateResHEta;
  TH1* m_duplicateResHPhi;


  TH1* m_duplicateLPTHoles;
  TH1* m_duplicateHPTHoles;
  TH1* m_singleMatchPTHoles;
  TH2* m_duplicateProbSpectrum;
  TH2* m_duplicateHPixelvsSCTShared;
  TH2* m_duplicateLPixelvsSCTShared;
  TH2* m_twoMatchHPixelvsSCTShared;
  TH2* m_twoMatchLPixelvsSCTShared;
  TH2* m_singleMatchPixelvsSCTShared;

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};

#endif
