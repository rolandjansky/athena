/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_hitEff.cxx
 * @author nora pettersson
 **/

#include "InDetPerfPlot_hitEff.h"
#include "InDetPhysHitDecoratorTool.h"
#include "xAODTracking/TrackParticle.h"
#include "TProfile.h"
#include "TEfficiency.h"
#include <vector>
#include <cmath> // for std::fabs




InDetPerfPlot_hitEff::InDetPerfPlot_hitEff(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(pParent,                                                                                                          sDir), m_hitEfficiencyVsEta{},  m_debug{false} {
  //
}

void
InDetPerfPlot_hitEff::initializePlots() {
  // const bool prependDirectory(false);
  // eff plots for L0PIXBARR, PIXEL, SCT, TRT
  // Barrel
  /**
  book(m_eff_hit_vs_eta[L0PIXBARR][BARREL], "eff_hit_vs_eta_l0pix_barrel");
  book(m_eff_hit_vs_eta[PIXEL][BARREL], "eff_hit_vs_eta_pix_barrel");
  book(m_eff_hit_vs_eta[SCT][BARREL], "eff_hit_vs_eta_sct_barrel");
  book(m_eff_hit_vs_eta[TRT][BARREL], "eff_hit_vs_eta_trt_barrel");

  book(m_eff_hit_vs_eta[PIXEL][ENDCAP], "eff_hit_vs_eta_pix_endcap");
  book(m_eff_hit_vs_eta[SCT][ENDCAP], "eff_hit_vs_eta_sct_endcap");
  book(m_eff_hit_vs_eta[TRT][ENDCAP], "eff_hit_vs_eta_trt_endcap");
  **/
  //
  book(m_hitEfficiencyVsEta[L0PIXBARR][BARREL], "eff_hit_vs_eta_l0pix_barrel");
  book(m_hitEfficiencyVsEta[PIXEL][BARREL], "eff_hit_vs_eta_pix_barrel");
  book(m_hitEfficiencyVsEta[SCT][BARREL], "eff_hit_vs_eta_sct_barrel");
  book(m_hitEfficiencyVsEta[TRT][BARREL], "eff_hit_vs_eta_trt_barrel");

  book(m_hitEfficiencyVsEta[PIXEL][ENDCAP], "eff_hit_vs_eta_pix_endcap");
  book(m_hitEfficiencyVsEta[SCT][ENDCAP], "eff_hit_vs_eta_sct_endcap");
  book(m_hitEfficiencyVsEta[TRT][ENDCAP], "eff_hit_vs_eta_trt_endcap");
}

void
InDetPerfPlot_hitEff::fill(const xAOD::TrackParticle& trkprt) {
  if (m_debug) {
    ATH_MSG_INFO("Filling hitEff");
  }

  const bool hitDetailsAvailable = trkprt.isAvailable<std::vector<int> >("measurement_region");
  static int warnCount(0);
  if (!hitDetailsAvailable) {
    if (warnCount++ < 10) {
      ATH_MSG_WARNING("The hitEff plots dont see any data (note: only 10 warnings issued)");
    }
  } else {
    const std::vector<int>& result_det = trkprt.auxdata< std::vector<int> >("measurement_det");
    if (!result_det.empty()) {
      const std::vector<int>& result_measureType = trkprt.auxdata< std::vector<int> >("measurement_type");
      const std::vector<int>& result_region = trkprt.auxdata< std::vector<int> >("measurement_region");
      // const std::vector<int> &result_iLayer = trkprt.auxdata< std::vector<int> >("hitResiduals_iLayer");
      // NP: this should be fine... residual filled with -1 if not hit

      for (unsigned int idx = 0; idx < result_region.size(); ++idx) {
        const int measureType = result_measureType[idx];
        const bool isHit((measureType == 0)or(measureType == 4));
        const int det = result_det[idx]; // LAYER TYPE L0PIXBARR / PIXEL / ...
        const int region = result_region[idx]; // BARREL OR ENDCAP
        float eta = std::fabs(trkprt.eta());
        if (det == DBM) {
          continue; // ignore DBM
        }
        //fillHisto(m_eff_hit_vs_eta[det][region], eta, int(isHit));
        fillHisto(m_hitEfficiencyVsEta[det][region], eta, isHit);
      }
    }
  }
  /** for testing
  const float binValue = std::rand() % 75;
  const bool passed = (binValue - 5 + (std::rand() % 10)) > 30;
  fillHisto(m_testEff, passed, binValue);
  **/
}
