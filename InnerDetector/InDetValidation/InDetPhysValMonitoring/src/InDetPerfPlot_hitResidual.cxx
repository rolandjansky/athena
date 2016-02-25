/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_hitResidual.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_hitResidual.h"
#include <iostream>



InDetPerfPlot_hitResidual::InDetPerfPlot_hitResidual(InDetPlotBase* pParent, const std::string & sDir)  :InDetPlotBase(pParent, sDir), 
   m_residualx_1hit{},
   m_residualx_2ormorehits{},
   m_residualx{},
   m_residualy_1hit{},
   m_residualy_2ormorehits{},
   m_residualy{},
   m_residualpullx{},
   m_residualpully{}
   {
//
}


void InDetPerfPlot_hitResidual::initializePlots() {
 //const bool prependDirectory(false);
 //x residuals
  book(m_residualx[PIXEL][BARREL],"residualx_pixel_barrel");
  book(m_residualx_1hit[PIXEL][BARREL],"residualx_pixel_barrel_1hit");
	book(m_residualx_2ormorehits[PIXEL][BARREL],"residualx_pixel_barrel_2ormorehits");
	//
	book(m_residualx[SCT][BARREL],"residualx_sct_barrel");
  book(m_residualx_1hit[SCT][BARREL],"residualx_sct_barrel_1hit");
	book(m_residualx_2ormorehits[SCT][BARREL],"residualx_sct_barrel_2ormorehits");
	//
	book(m_residualx[BLAYER][BARREL],"residualx_blayer_barrel");
  book(m_residualx_1hit[BLAYER][BARREL],"residualx_blayer_barrel_1hit");
	book(m_residualx_2ormorehits[BLAYER][BARREL],"residualx_blayer_barrel_2ormorehits");
	//
  book(m_residualx[TRT][BARREL],"residualx_trt_barrel");
	//..now endcaps
	book(m_residualx[PIXEL][ENDCAP],"residualx_pixel_endcap");
  book(m_residualx_1hit[PIXEL][ENDCAP],"residualx_pixel_endcap_1hit");
	book(m_residualx_2ormorehits[PIXEL][ENDCAP],"residualx_pixel_endcap_2ormorehits");
	//
	book(m_residualx[SCT][ENDCAP],"residualx_sct_endcap");
  book(m_residualx_1hit[SCT][ENDCAP],"residualx_sct_endcap_1hit");
	book(m_residualx_2ormorehits[SCT][ENDCAP],"residualx_sct_endcap_2ormorehits");
	
	book(m_residualx[BLAYER][ENDCAP],"residualx_blayer_endcap");
  book(m_residualx_1hit[BLAYER][ENDCAP],"residualx_blayer_endcap_1hit");
	book(m_residualx_2ormorehits[BLAYER][ENDCAP],"residualx_blayer_endcap_2ormorehits");
	//
  book(m_residualx[TRT][ENDCAP],"residualx_trt_endcap");
	//DBM
	book(m_residualx[DBM][0],"residualx_dbm_neg");
  book(m_residualx[DBM][1],"residualx_dbm_pos");
  //
  // y residuals
  book(m_residualy[PIXEL][BARREL],"residualy_pixel_barrel");
  book(m_residualy_1hit[PIXEL][BARREL],"residualy_pixel_barrel_1hit");
	book(m_residualy_2ormorehits[PIXEL][BARREL],"residualy_pixel_barrel_2ormorehits");
	//
	book(m_residualy[SCT][BARREL],"residualy_sct_barrel");
  book(m_residualy_1hit[SCT][BARREL],"residualy_sct_barrel_1hit");
	book(m_residualy_2ormorehits[SCT][BARREL],"residualy_sct_barrel_2ormorehits");
	//
	book(m_residualy[BLAYER][BARREL],"residualy_blayer_barrel");
  book(m_residualy_1hit[BLAYER][BARREL],"residualy_blayer_barrel_1hit");
	book(m_residualy_2ormorehits[BLAYER][BARREL],"residualy_blayer_barrel_2ormorehits");
	//
  book(m_residualy[TRT][BARREL],"residualy_trt_barrel");
	//..now endcaps
	book(m_residualy[PIXEL][ENDCAP],"residualy_pixel_endcap");
  book(m_residualy_1hit[PIXEL][ENDCAP],"residualy_pixel_endcap_1hit");
	book(m_residualy_2ormorehits[PIXEL][ENDCAP],"residualy_pixel_endcap_2ormorehits");
	//
	book(m_residualy[SCT][ENDCAP],"residualy_sct_endcap");
  book(m_residualy_1hit[SCT][ENDCAP],"residualy_sct_endcap_1hit");
	book(m_residualy_2ormorehits[SCT][ENDCAP],"residualy_sct_endcap_2ormorehits");
	
	book(m_residualy[BLAYER][ENDCAP],"residualy_blayer_endcap");
  book(m_residualy_1hit[BLAYER][ENDCAP],"residualy_blayer_endcap_1hit");
	book(m_residualy_2ormorehits[BLAYER][ENDCAP],"residualy_blayer_endcap_2ormorehits");
	//
  book(m_residualy[TRT][ENDCAP],"residualy_trt_endcap");
	//DBM
	book(m_residualy[DBM][0],"residualy_dbm_neg");
  book(m_residualy[DBM][1],"residualy_dbm_pos");
	// pulls
	//barrel
	book(m_residualpullx[BLAYER][BARREL],"residualpullx_blayer_barrel");
	book(m_residualpullx[PIXEL][BARREL],"residualpullx_pixel_barrel");
	book(m_residualpullx[SCT][BARREL],"residualpullx_sct_barrel");
  book(m_residualpullx[TRT][BARREL],"residualpullx_trt_barrel");
  book(m_residualpullx[DBM][0],"residualpullx_dbm_barrel");
  //
  book(m_residualpullx[BLAYER][ENDCAP],"residualpullx_blayer_endcap");
  book(m_residualpullx[PIXEL][ENDCAP],"residualpullx_pixel_endcap");
	book(m_residualpullx[SCT][ENDCAP],"residualpullx_sct_endcap");
  book(m_residualpullx[TRT][ENDCAP],"residualpullx_trt_endcap");
  book(m_residualpullx[DBM][1],"residualpullx_dbm_endcap");
  //
  //barrel
  book(m_residualpully[BLAYER][BARREL],"residualpully_blayer_barrel");
	book(m_residualpully[PIXEL][BARREL],"residualpully_pixel_barrel");
	book(m_residualpully[SCT][BARREL],"residualpully_sct_barrel");
  book(m_residualpully[TRT][BARREL],"residualpully_trt_barrel");
  book(m_residualpully[DBM][0],"residualpully_dbm_barrel");
  //
  book(m_residualpully[BLAYER][ENDCAP],"residualpully_blayer_endcap");
  book(m_residualpully[PIXEL][ENDCAP],"residualpully_pixel_endcap");
	book(m_residualpully[SCT][ENDCAP],"residualpully_sct_endcap");
  book(m_residualpully[TRT][ENDCAP],"residualpully_trt_endcap");
  book(m_residualpully[DBM][1],"residualpully_dbm_endcap");

}

void InDetPerfPlot_hitResidual::fill(const xAOD::TrackParticle& trkprt) {
  const bool hitDetailsAvailable= trkprt.isAvailable<std::vector<int> >("hitResiduals_region");
  if (!hitDetailsAvailable){
     ATH_MSG_WARNING("The hit res plots dont see any data");
  } else {
    const std::vector<int> &result_det = trkprt.auxdata< std::vector<int> >("hitResiduals_det");
		if (!result_det.empty()) {
			 const std::vector<int> & result_region=trkprt.auxdata< std::vector<int> >("hitResiduals_region");
			 //const std::vector<int> &result_iLayer = trkprt.auxdata< std::vector<int> >("hitResiduals_iLayer");
			 const std::vector<float> &result_residualLocX = trkprt.auxdata< std::vector<float> >("hitResiduals_residualLocX");
			 const std::vector<float> &result_pullLocX = trkprt.auxdata< std::vector<float> >("hitResiduals_pullLocX");
			 const std::vector<float> &result_residualLocY = trkprt.auxdata< std::vector<float> >("hitResiduals_residualLocY");
			 const std::vector<float> &result_pullLocY = trkprt.auxdata< std::vector<float> >("hitResiduals_pullLocY");
			 const std::vector<int> &result_phiWidth = trkprt.auxdata< std::vector<int> >("hitResiduals_phiWidth");
			 for (unsigned int idx=0; idx < result_region.size(); ++idx){
				 const int det = result_det.at(idx);
				 const int region = result_region.at(idx);
				 // const int layer = result_iLayer.at(idx);
				 const int width = result_phiWidth.at(idx);
				 const float residualLocX = result_residualLocX.at(idx);
				 const float pullLocX = result_pullLocX.at(idx);
				 const float residualLocY = result_residualLocY.at(idx);
				 const float pullLocY = result_pullLocY.at(idx);
				if ((det == INVALID_DETECTOR) or (region==INVALID_REGION)) continue;
				(m_residualx[det][region])->Fill(residualLocX);
				if (not std::isnan(residualLocY)) (m_residualy[det][region])->Fill(residualLocY);
				(m_residualpullx[det][region])->Fill(pullLocX);
				if (not std::isnan(pullLocY)) (m_residualpully[det][region])->Fill(pullLocY);
				if ((det == TRT) or (det == DBM) or (width<0)){
					continue;
				}
				if (width == 1){ 
					(m_residualx_1hit[det][region])->Fill(residualLocX);
					if (not std::isnan(residualLocY)) (m_residualy_1hit[det][region])->Fill(residualLocY);
				} else {
				(m_residualx_2ormorehits[det][region])->Fill(residualLocX);
				if (not std::isnan(residualLocY)) (m_residualy_2ormorehits[det][region])->Fill(residualLocY);

				}
			}
		}
	}
  
}



