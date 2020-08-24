/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_HitResidual.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_HitResidual.h"

using namespace TMath;

InDetPerfPlot_HitResidual::InDetPerfPlot_HitResidual(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(
    pParent, sDir),
  m_residualx_1hit{},
  m_residualx_2ormorehits{},
  m_residualx{},
  m_residualy_1hit{},
  m_residualy_2ormorehits{},
  m_residualy{},
  m_pullx{},
  m_pully{},
  m_phiWidth{},
  m_etaWidth{} {
//
}

void
InDetPerfPlot_HitResidual::initializePlots() {
  // const bool prependDirectory(false);
  // x residuals
  book(m_residualx[L0PIXBARR][BARREL], "residualx_l0pix_barrel");
  book(m_residualx_1hit[L0PIXBARR][BARREL], "residualx_l0pix_barrel_1hit");
  book(m_residualx_2ormorehits[L0PIXBARR][BARREL], "residualx_l0pix_barrel_2ormorehits");
  //
  book(m_residualx[PIXEL][BARREL], "residualx_pixel_barrel");
  book(m_residualx_1hit[PIXEL][BARREL], "residualx_pixel_barrel_1hit");
  book(m_residualx_2ormorehits[PIXEL][BARREL], "residualx_pixel_barrel_2ormorehits");
  //
  book(m_residualx[SCT][BARREL], "residualx_sct_barrel");
  book(m_residualx_1hit[SCT][BARREL], "residualx_sct_barrel_1hit");
  book(m_residualx_2ormorehits[SCT][BARREL], "residualx_sct_barrel_2ormorehits");
  //
  book(m_residualx[TRT][BARREL], "residualx_trt_barrel");
  // ..now endcaps
  book(m_residualx[PIXEL][ENDCAP], "residualx_pixel_endcap");
  book(m_residualx_1hit[PIXEL][ENDCAP], "residualx_pixel_endcap_1hit");
  book(m_residualx_2ormorehits[PIXEL][ENDCAP], "residualx_pixel_endcap_2ormorehits");
  //
  book(m_residualx[SCT][ENDCAP], "residualx_sct_endcap");
  book(m_residualx_1hit[SCT][ENDCAP], "residualx_sct_endcap_1hit");
  book(m_residualx_2ormorehits[SCT][ENDCAP], "residualx_sct_endcap_2ormorehits");
  //
  book(m_residualx[TRT][ENDCAP], "residualx_trt_endcap");
  //

  // y residuals
  book(m_residualy[L0PIXBARR][BARREL], "residualy_l0pix_barrel");
  book(m_residualy_1hit[L0PIXBARR][BARREL], "residualy_l0pix_barrel_1hit");
  book(m_residualy_2ormorehits[L0PIXBARR][BARREL], "residualy_l0pix_barrel_2ormorehits");
  //
  book(m_residualy[PIXEL][BARREL], "residualy_pixel_barrel");
  book(m_residualy_1hit[PIXEL][BARREL], "residualy_pixel_barrel_1hit");
  book(m_residualy_2ormorehits[PIXEL][BARREL], "residualy_pixel_barrel_2ormorehits");
  //
  // SCT and TRT do not have y-residuals/pulls
  // ..now endcaps
  book(m_residualy[PIXEL][ENDCAP], "residualy_pixel_endcap");
  book(m_residualy_1hit[PIXEL][ENDCAP], "residualy_pixel_endcap_1hit");
  book(m_residualy_2ormorehits[PIXEL][ENDCAP], "residualy_pixel_endcap_2ormorehits");
  //
  // SCT and TRT do not have y-residuals/pulls
  // pulls
  // barrel
  book(m_pullx[L0PIXBARR][BARREL], "pullx_l0pix_barrel");
  book(m_pullx[PIXEL][BARREL], "pullx_pixel_barrel");
  book(m_pullx[SCT][BARREL], "pullx_sct_barrel");
  book(m_pullx[TRT][BARREL], "pullx_trt_barrel");
  //
  book(m_pullx[PIXEL][ENDCAP], "pullx_pixel_endcap");
  book(m_pullx[SCT][ENDCAP], "pullx_sct_endcap");
  book(m_pullx[TRT][ENDCAP], "pullx_trt_endcap");
  //
  // barrel
  book(m_pully[L0PIXBARR][BARREL], "pully_l0pix_barrel");
  book(m_pully[PIXEL][BARREL], "pully_pixel_barrel");
  //
  // SCT and TRT do not have y-residuals/pulls
  book(m_pully[PIXEL][ENDCAP], "pully_pixel_endcap");
  //
  ////SCT and TRT do not have y-residuals/pulls
  // introduce cluster width histograms
  book(m_phiWidth[PIXEL][BARREL], "clusterPhiWidth_pixel_barrel");
  book(m_phiWidth[PIXEL][ENDCAP], "clusterPhiWidth_pixel_endcap");
  book(m_etaWidth[PIXEL][BARREL], "clusterEtaWidth_pixel_barrel");
  book(m_etaWidth[PIXEL][ENDCAP], "clusterEtaWidth_pixel_endcap");
  //
  book(m_phiWidth[SCT][BARREL], "clusterPhiWidth_sct_barrel");
  book(m_phiWidth[SCT][ENDCAP], "clusterPhiWidth_sct_endcap");
}

void
InDetPerfPlot_HitResidual::fill(const xAOD::TrackParticle& trkprt) {
  const static bool hitDetailsAvailable = trkprt.isAvailable<std::vector<int> >("measurement_region");

  if (!hitDetailsAvailable) {
    if (m_warnCount++ < 10) {
      ATH_MSG_WARNING("The hit res plots dont see any data (note:only 10 warnings issued)");
    }
  } else {
    const std::vector<int>& result_det = trkprt.auxdata< std::vector<int> >("measurement_det");

    if (!result_det.empty()) {
      const std::vector<int>& result_measureType = trkprt.auxdata< std::vector<int> >("measurement_type");
      const std::vector<int>& result_region = trkprt.auxdata< std::vector<int> >("measurement_region");
      // const std::vector<int> &result_iLayer = trkprt.auxdata< std::vector<int> >("HitResiduals_iLayer");
      const std::vector<float>& result_residualLocX = trkprt.auxdata< std::vector<float> >("hitResiduals_residualLocX");
      const std::vector<float>& result_pullLocX = trkprt.auxdata< std::vector<float> >("hitResiduals_pullLocX");
      const std::vector<float>& result_residualLocY = trkprt.auxdata< std::vector<float> >("hitResiduals_residualLocY");
      const std::vector<float>& result_pullLocY = trkprt.auxdata< std::vector<float> >("hitResiduals_pullLocY");
      const std::vector<int>& result_phiWidth = trkprt.auxdata< std::vector<int> >("hitResiduals_phiWidth");
      const std::vector<int>& result_etaWidth = trkprt.auxdata< std::vector<int> >("hitResiduals_etaWidth");

      // NP: this should be fine... resiudal filled with -1 if not hit
      if (result_det.size() != result_residualLocX.size()) {
        ATH_MSG_WARNING("Vectors of results are not matched in size!");
      }
      const auto resultSize = result_region.size();
      for (unsigned int idx = 0; idx < resultSize; ++idx) {
        const int measureType = result_measureType[idx];
        if (measureType != 4) {
          continue; // NP: Only use unbiased hits for the hit residuals ;)
        }
        const int det = result_det[idx];
        const int region = result_region[idx];
        // const int layer = result_iLayer.at(idx);
        const int width = result_phiWidth[idx];
        const int etaWidth = result_etaWidth[idx];
        const float residualLocX = result_residualLocX[idx];
        const float pullLocX = result_pullLocX[idx];
        const float residualLocY = result_residualLocY[idx];
        const float pullLocY = result_pullLocY[idx];
        if ((det == INVALID_DETECTOR)or(region == INVALID_REGION)) {
          continue;
        }
        if ((width > 0) or (det ==TRT)){//TRT does not have defined cluster width 
          // introduce cluster width histograms
          fillHisto(m_phiWidth[det][region], width);
          fillHisto(m_etaWidth[det][region], etaWidth);
          fillHisto(m_residualx[det][region], residualLocX);
          const bool hasYCoordinate = (det != SCT)and(det != TRT); // SCT & TRT do not have LocY
          if (hasYCoordinate) {
            fillHisto(m_residualy[det][region], residualLocY);
          }
          fillHisto(m_pullx[det][region], pullLocX);
          if (hasYCoordinate) { // SCT & TRT do not have LocY
            fillHisto(m_pully[det][region], pullLocY);
          }
          if ((det == TRT) or (width < 0)) {
            continue;
          }
          if (width == 1) {
            fillHisto(m_residualx_1hit[det][region], residualLocX);
            if (hasYCoordinate) {
              fillHisto(m_residualy_1hit[det][region], residualLocY);
            }
          } else {
            fillHisto(m_residualx_2ormorehits[det][region], residualLocX);
            if (hasYCoordinate) {
              fillHisto(m_residualy_2ormorehits[det][region], residualLocY);
            }
          }
        }
      }
    }
  }
}
