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
#include <vector>




InDetPerfPlot_hitEff::InDetPerfPlot_hitEff(InDetPlotBase *pParent, const std::string &sDir)  : InDetPlotBase(pParent,
                                                                                                             sDir),
  m_eff_hit_vs_eta{}, hit_DEBUG{false} {
  //
}

void
InDetPerfPlot_hitEff::initializePlots() {
  // const bool prependDirectory(false);
  // eff plots for IBL, PIXEL, SCT, TRT
  // Barrel
  book(m_eff_hit_vs_eta[IBL][BARREL], "eff_hit_vs_eta_ibl_barrel");
  book(m_eff_hit_vs_eta[PIXEL][BARREL], "eff_hit_vs_eta_pix_barrel");
  book(m_eff_hit_vs_eta[SCT][BARREL], "eff_hit_vs_eta_sct_barrel");
  book(m_eff_hit_vs_eta[TRT][BARREL], "eff_hit_vs_eta_trt_barrel");

  book(m_eff_hit_vs_eta[PIXEL][ENDCAP], "eff_hit_vs_eta_pix_endcap");
  book(m_eff_hit_vs_eta[SCT][ENDCAP], "eff_hit_vs_eta_sct_endcap");
  book(m_eff_hit_vs_eta[TRT][ENDCAP], "eff_hit_vs_eta_trt_endcap");
}

void
InDetPerfPlot_hitEff::fill(const xAOD::TrackParticle &trkprt) {
  if (hit_DEBUG) {
    ATH_MSG_INFO("Filling hitEff");
  }

  const bool hitDetailsAvailable = trkprt.isAvailable<std::vector<int> >("measurement_region");

  if (!hitDetailsAvailable) {
    ATH_MSG_WARNING("The hit res plots dont see any data");
  } else {
    const std::vector<int> &result_det = trkprt.auxdata< std::vector<int> >("measurement_det");

    if (!result_det.empty()) {
      const std::vector<int> &result_measureType = trkprt.auxdata< std::vector<int> >("measurement_type");
      const std::vector<int> &result_region = trkprt.auxdata< std::vector<int> >("measurement_region");
      // const std::vector<int> &result_iLayer = trkprt.auxdata< std::vector<int> >("hitResiduals_iLayer");
      // NP: this should be fine... resiudal filled with -1 if not hit

      for (unsigned int idx = 0; idx < result_region.size(); ++idx) {
        const int measureType = result_measureType.at(idx);
        bool isHit = false;
        if (measureType == 0) {
          isHit = true;
        }
        const int det = result_det.at(idx); // LAYER TYPE IBL / PIXEL / ...
        const int region = result_region.at(idx); // BARREL OR ENDCAP
        float eta = fabs(trkprt.eta());
        if ((det == IBL && region == ENDCAP) || det == DBM) {
          continue; // IBL have no endcaps! and ignore DBM
        }
        if (isHit) {
          m_eff_hit_vs_eta[det][region]->Fill(eta, 1);
        }else {
          if (measureType != 4) {
            m_eff_hit_vs_eta[det][region]->Fill(eta, 0);
          }
        }
      }
    }
  }
}
