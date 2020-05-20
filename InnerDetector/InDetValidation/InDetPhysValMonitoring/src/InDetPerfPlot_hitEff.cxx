/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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




InDetPerfPlot_hitEff::InDetPerfPlot_hitEff(InDetPlotBase* pParent, const std::string& sDir)  : InDetPlotBase(pParent,                                                                                                          sDir), 
  m_fillAdditionalITkPlots(false),
  m_hitEfficiencyVsEta{},   
  m_hitEfficiencyVsEta_PerLayer{},
  m_hitEfficiencyVsEta_PerType{},
  m_debug{false} {
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

void InDetPerfPlot_hitEff::FillAdditionalITkPlots(bool fill) {

  m_fillAdditionalITkPlots = fill;
  
  book(m_hitEfficiencyVsEta_PerLayer[0][L0PIXBARR][BARREL], "eff_hit_vs_eta_pix_barrel_layer0");
  book(m_hitEfficiencyVsEta_PerLayer[0][PIXEL][ENDCAP]    , "eff_hit_vs_eta_pix_endcap_layer0");
  
  book(m_hitEfficiencyVsEta_PerType[INNERMOST]        , "eff_hit_vs_eta_pix_innermost");
  book(m_hitEfficiencyVsEta_PerType[NEXT_TO_INNERMOST], "eff_hit_vs_eta_pix_nextToInnermost");
  book(m_hitEfficiencyVsEta_PerType[OTHER]            , "eff_hit_vs_eta_pix_others"); 
    
  for (int layer=1; layer < N_LAYERS; layer++) {
    book(m_hitEfficiencyVsEta_PerLayer[layer][PIXEL][BARREL], "eff_hit_vs_eta_pix_barrel_layer"+std::to_string(layer));
    book(m_hitEfficiencyVsEta_PerLayer[layer][PIXEL][ENDCAP], "eff_hit_vs_eta_pix_endcap_layer"+std::to_string(layer));
  }
  
  for (int layer=0; layer < N_SCTLAYERS; layer++) {
    book(m_hitEfficiencyVsEta_PerLayer[layer][SCT][BARREL], "eff_hit_vs_eta_sct_barrel_layer"+std::to_string(layer));
    book(m_hitEfficiencyVsEta_PerLayer[layer][SCT][ENDCAP], "eff_hit_vs_eta_sct_endcap_layer"+std::to_string(layer));
  }  
  
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
      std::vector<int> result_iLayer;
      SG::AuxElement::ConstAccessor<std::vector<int>> iLayer ("measurement_iLayer");
      if (!iLayer.isAvailable(trkprt) && m_fillAdditionalITkPlots) result_iLayer = trkprt.auxdecor<std::vector<int> >("measurement_iLayer");
      std::vector<int> result_lType;
      SG::AuxElement::ConstAccessor<std::vector<int>> lType ("measurement_lType");
      if (!lType.isAvailable(trkprt) && m_fillAdditionalITkPlots) result_lType = trkprt.auxdecor<std::vector<int> >("measurement_lType");
      // NP: this should be fine... residual filled with -1 if not hit
      //Only do this if decoration doesn't already exist

      for (unsigned int idx = 0; idx < result_region.size(); ++idx) {
        const int measureType = result_measureType[idx];
        const bool isHit((measureType == 0)or(measureType == 4));
        const int det = result_det[idx]; // LAYER TYPE L0PIXBARR / PIXEL / ...
        const int region = result_region[idx]; // BARREL OR ENDCAP
        int layer = 0; int type = 0;
        if (m_fillAdditionalITkPlots){ 
          if(!iLayer.isAvailable(trkprt)) layer = result_iLayer[idx];
          else layer  = (iLayer(trkprt))[idx];
          if(!lType.isAvailable(trkprt)) type = result_lType[idx];
          else type  = (lType(trkprt))[idx];
        }
        float eta = std::fabs(trkprt.eta());
        if (det == DBM) {
          continue; // ignore DBM
        }
        //fillHisto(m_eff_hit_vs_eta[det][region], eta, int(isHit));
        fillHisto(m_hitEfficiencyVsEta[det][region], eta, isHit);
        if (m_fillAdditionalITkPlots and (det==PIXEL or det==L0PIXBARR or (det==SCT and layer<N_SCTLAYERS))) {
          fillHisto(m_hitEfficiencyVsEta_PerLayer[layer][det][region], eta, isHit);
          if (det==PIXEL or det==L0PIXBARR)
            fillHisto(m_hitEfficiencyVsEta_PerType[type], eta, isHit);
        }
      }
    }
  }
  /** for testing
  const float binValue = std::rand() % 75;
  const bool passed = (binValue - 5 + (std::rand() % 10)) > 30;
  fillHisto(m_testEff, passed, binValue);
  **/
}
