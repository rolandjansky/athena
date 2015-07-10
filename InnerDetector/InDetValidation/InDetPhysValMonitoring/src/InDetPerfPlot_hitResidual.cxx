/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_hitResidual.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_hitResidual.h"
#include <iostream>



InDetPerfPlot_hitResidual::InDetPerfPlot_hitResidual(PlotBase* pParent, const std::string & sDir)  :PlotBase(pParent, sDir), 
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
 const bool prependDirectory(false);
 //x residuals
  m_residualx[PIXEL][BARREL] = Book1D("residualx_pixel_barrel","Residual: Pixel Barrel X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx_1hit[PIXEL][BARREL] = Book1D("residualx_pixel_barrel_1hit","Residual: Pixel Barrel X, 1 hit;x residual(#mum)",120,-3000,3000,prependDirectory);
	m_residualx_2ormorehits[PIXEL][BARREL] = Book1D("residualx_pixel_barrel_2ormorehits","Residual: Pixel Barrel X, >=2 hits;x residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualx[SCT][BARREL] = Book1D("residualx_sct_barrel","Residual: SCT Barrel X;x residual(#mum)",120,-50,50,prependDirectory);
  m_residualx_1hit[SCT][BARREL] = Book1D("residualx_sct_barrel_1hit","Residual: SCT Barrel X, 1 hit;x residual(#mum)",120,-3000,3000,prependDirectory);
	m_residualx_2ormorehits[SCT][BARREL] = Book1D("residualx_sct_barrel_2ormorehits","Residual: SCT Barrel X, >=2 hits;x residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualx[BLAYER][BARREL] = Book1D("residualx_blayer_barrel","Residual: B-Layer Barrel X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx_1hit[BLAYER][BARREL] = Book1D("residualx_blayer_barrel_1hit","Residual: B-Layer Barrel X, 1 hit;x residual(#mum)",120,-50,50,prependDirectory);
	m_residualx_2ormorehits[BLAYER][BARREL] = Book1D("residualx_blayer_barrel_2ormorehits","Residual: B-Layer Barrel X, >=2 hits;x residual(#mum)",120,-50,50,prependDirectory);
	//
  m_residualx[TRT][BARREL] = Book1D("residualx_trt_barrel","Residual: TRT Barrel X;x residual(#mum)",120,-3000,3000,prependDirectory);
	//..now endcaps
	m_residualx[PIXEL][ENDCAP] = Book1D("residualx_pixel_endcap","Residual: Pixel Endcap X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx_1hit[PIXEL][ENDCAP] = Book1D("residualx_pixel_endcap_1hit","Residual: Pixel Endcap X, 1 hit;x residual(#mum)",120,-3000,3000,prependDirectory);
	m_residualx_2ormorehits[PIXEL][ENDCAP] = Book1D("residualx_pixel_endcap_2ormorehits","Residual: Pixel Endcap X, >=2 hits;x residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualx[SCT][ENDCAP] = Book1D("residualx_sct_endcap","Residual: SCT Endcap X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx_1hit[SCT][ENDCAP] = Book1D("residualx_sct_endcap_1hit","Residual: SCT Endcap X, 1 hit;x residual(#mum)",120,-3000,3000,prependDirectory);
	m_residualx_2ormorehits[SCT][ENDCAP] = Book1D("residualx_sct_endcap_2ormorehits","Residual: SCT Endcap X, >=2 hits;x residual(#mum)",120,-3000,3000,prependDirectory);
	//
	m_residualx[BLAYER][ENDCAP] = Book1D("residualx_blayer_endcap","Residual: B-Layer Endcap X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx_1hit[BLAYER][ENDCAP] = Book1D("residualx_blayer_endcap_1hit","Residual: B-Layer Endcap X, 1 hit;x residual(#mum)",120,-3000,3000,prependDirectory);
	m_residualx_2ormorehits[BLAYER][ENDCAP] = Book1D("residualx_blayer_endcap_2ormorehits","Residual: B-Layer Endcap X, >=2 hits;x residual(#mum)",120,-3000,3000,prependDirectory);
	//
  m_residualx[TRT][ENDCAP] = Book1D("residualx_trt_endcap","Residual: TRT Endcap X;x residual(#mum)",120,-50,50,prependDirectory);
	//DBM
	m_residualx[DBM][0] = Book1D("residualx_dbm_neg","Residual: DBM -side X;x residual(#mum)",120,-3000,3000,prependDirectory);
  m_residualx[DBM][1] = Book1D("residualx_dbm_pos","Residual: DBM +side X;x residual(#mum)",120,-3000,3000,prependDirectory);
  //
  // y residuals
  m_residualy[PIXEL][BARREL] = Book1D("residualy_pixel_barrel","Residual: Pixel Barrel Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[PIXEL][BARREL] = Book1D("residualy_pixel_barrel_1hit","Residual: Pixel Barrel Y, 1 hit;Y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[PIXEL][BARREL] = Book1D("residualy_pixel_barrel_2ormorehits","Residual: Pixel Barrel Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualy[SCT][BARREL] = Book1D("residualy_sct_barrel","Residual: SCT Barrel Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[SCT][BARREL] = Book1D("residualy_sct_barrel_1hit","Residual: SCT Barrel Y, 1 hit;y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[SCT][BARREL] = Book1D("residualy_sct_barrel_2ormorehits","Residual: SCT Barrel Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualy[BLAYER][BARREL] = Book1D("residualy_blayer_barrel","Residual: B-Layer Barrel Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[BLAYER][BARREL] = Book1D("residualy_blayer_barrel_1hit","Residual: B-Layer Barrel Y, 1 hit;y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[BLAYER][BARREL] = Book1D("residualy_blayer_barrel_2ormorehits","Residual: B-Layer Barrel Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
  m_residualy[TRT][BARREL] = Book1D("residualy_trt_barrel","Residual: TRT Barrel Y;y residual(#mum)",120,0,2500,prependDirectory);
	//..now endcaps
	m_residualy[PIXEL][ENDCAP] = Book1D("residualy_pixel_endcap","Residual: Pixel Endcap Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[PIXEL][ENDCAP] = Book1D("residualy_pixel_endcap_1hit","Residual: Pixel Endcap Y, 1 hit;y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[PIXEL][ENDCAP] = Book1D("residualy_pixel_endcap_2ormorehits","Residual: Pixel Endcap Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualy[SCT][ENDCAP] = Book1D("residualy_sct_endcap","Residual: SCT Endcap Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[SCT][ENDCAP] = Book1D("residualy_sct_endcap_1hit","Residual: SCT Endcap Y, 1 hit;y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[SCT][ENDCAP] = Book1D("residualy_sct_endcap_2ormorehits","Residual: SCT Endcap Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
	m_residualy[BLAYER][ENDCAP] = Book1D("residualy_blayer_endcap","Residual: B-Layer Endcap Y;y residual(#mum)",120,-50,50,prependDirectory);
  m_residualy_1hit[BLAYER][ENDCAP] = Book1D("residualy_blayer_endcap_1hit","Residual: B-Layer Endcap Y, 1 hit;y residual(#mum)",120,-50,50,prependDirectory);
	m_residualy_2ormorehits[BLAYER][ENDCAP] = Book1D("residualy_blayer_endcap_2ormorehits","Residual: B-Layer Endcap Y, >=2 hits;y residual(#mum)",120,-50,50,prependDirectory);
	//
  m_residualy[TRT][ENDCAP] = Book1D("residualy_trt_endcap","Residual: TRT Endcap Y;y residual(#mum)",120,-50,50,prependDirectory);
	//DBM
	m_residualy[DBM][0] = Book1D("residualy_dbm_neg","Residual: DBM -side Y;y residual(#mum)",10,-5,5,prependDirectory);
  m_residualy[DBM][1] = Book1D("residualy_dbm_pos","Residual: DBM +side Y;y residual(#mum)",10,-5,5,prependDirectory);
	// pulls
	//barrel
	m_residualpullx[BLAYER][BARREL] = Book1D("residualpullx_blayer_barrel","Residualpull: B-Layer Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
	m_residualpullx[PIXEL][BARREL] = Book1D("residualpullx_pixel_barrel","Residualpull: Pixel Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
	m_residualpullx[SCT][BARREL] = Book1D("residualpullx_sct_barrel","Residualpull: SCT Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpullx[TRT][BARREL] = Book1D("residualpullx_trt_barrel","Residualpull: TRT Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpullx[DBM][0] = Book1D("residualpullx_dbm_barrel","Residualpull: DBM -side X;x residual(#mum)",10,-5,5,prependDirectory);
  //
  m_residualpullx[BLAYER][ENDCAP] = Book1D("residualpullx_blayer_endcap","Residualpull: B-Layer Endcap X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpullx[PIXEL][ENDCAP] = Book1D("residualpullx_pixel_endcap","Residualpull: Pixel Endcap X;x residual(#mum)",10,-5,5,prependDirectory);
	m_residualpullx[SCT][ENDCAP] = Book1D("residualpullx_sct_endcap","Residualpull: SCT Endcap X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpullx[TRT][ENDCAP] = Book1D("residualpullx_trt_endcap","Residualpull: TRT Endcap X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpullx[DBM][1] = Book1D("residualpullx_dbm_endcap","Residualpull: DBM +side X;x residual(#mum)",10,-5,5,prependDirectory);
  //
  //barrel
  m_residualpully[BLAYER][BARREL] = Book1D("residualpully_blayer_barrel","Residualpull: B-Layer Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
	m_residualpully[PIXEL][BARREL] = Book1D("residualpully_pixel_barrel","Residualpull: Pixel Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
	m_residualpully[SCT][BARREL] = Book1D("residualpully_sct_barrel","Residualpull: SCT Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpully[TRT][BARREL] = Book1D("residualpully_trt_barrel","Residualpull: TRT Barrel X;x residual(#mum)",10,-5,5,prependDirectory);
  m_residualpully[DBM][0] = Book1D("residualpully_dbm_barrel","Residualpull: DBM -side X;x residual(#mum)",10,-5,5,prependDirectory);
  //
  m_residualpully[BLAYER][ENDCAP] = Book1D("residualpully_blayer_endcap","Residualpull: B-Layer Endcap Y;y residual(#mum)",10,-5,5,prependDirectory);
  m_residualpully[PIXEL][ENDCAP] = Book1D("residualpully_pixel_endcap","Residualpull: Pixel Endcap Y;y residual(#mum)",10,-5,5,prependDirectory);
	m_residualpully[SCT][ENDCAP] = Book1D("residualpully_sct_endcap","Residualpull: SCT Endcap Y;y residual(#mum)",10,-5,5,prependDirectory);
  m_residualpully[TRT][ENDCAP] = Book1D("residualpully_trt_endcap","Residualpull: TRT Endcap Y;y residual(#mum)",10,-5,5,prependDirectory);
  m_residualpully[DBM][1] = Book1D("residualpully_dbm_endcap","Residualpull: DBM +side Y;y residual(#mum)",10,-5,5,prependDirectory);

}

void InDetPerfPlot_hitResidual::fill(const xAOD::TrackParticle& trkprt) {
  //det, r, iLayer, residualLocX, pullLocX, residualLocY, pullLocY, phiWidth
	typedef std::tuple<int,int,int, float, float, float, float, int> SingleResult_t;
	typedef std::vector<SingleResult_t> TrackResult_t;
  const bool hitDetailsAvailable= trkprt.isAvailable<TrackResult_t>("hitResiduals");
  if (hitDetailsAvailable){
    const TrackResult_t & result=trkprt.auxdata< TrackResult_t >("hitResiduals");
    if (not result.empty()){
    	for (const auto i:result){
				const int det = std::get<0>(i);
				const int region = std::get<1>(i);
				//const int layer = std::get<2>(i);
				const int width = std::get<7>(i);
				const float residualLocX = std::get<3>(i);
				const float pullLocX = std::get<4>(i);
				const float residualLocY = std::get<5>(i);
				const float pullLocY = std::get<6>(i);
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



