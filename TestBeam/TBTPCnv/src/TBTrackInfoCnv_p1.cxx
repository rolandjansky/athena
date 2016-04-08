/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTrackInfoCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBTrackInfo.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTrackInfo_p1.h"



void
TBTrackInfoCnv_p1::persToTrans(const TBTrackInfo_p1* pers, 
                                     TBTrackInfo* trans, MsgStream &/*log*/)
{

  trans->tb_Trigger         = pers->tb_Trigger;
  trans->tb_trk_nTracks     = pers->tb_trk_nTracks;
  trans->tb_trk_nTracksTRT  = pers->tb_trk_nTracksTRT;

  trans->tb_sADC_S1     		= pers->tb_sADC_S1;
  trans->tb_sADC_muHalo 	  = pers->tb_sADC_muHalo;
  trans->tb_sADC_muTag  	  = pers->tb_sADC_muTag;
  trans->tb_sADC_C2     	  = pers->tb_sADC_C2;
  trans->tb_sADC_TRTSci 	  = pers->tb_sADC_TRTSci;

  trans->tb_trk_nPixelHits  = pers->tb_trk_nPixelHits;
  trans->tb_trk_nSctHits    = pers->tb_trk_nSctHits;
  trans->tb_trk_nTrtHitsTRT = pers->tb_trk_nTrtHitsTRT;

  trans->tb_trk_Trt_HLTRT   = pers->tb_trk_Trt_HLTRT;

	trans->tileRec_XchN1      = pers->tileRec_XchN1;
  trans->tileRec_XchN2      = pers->tileRec_XchN2;
  trans->tileRec_Xcha0      = pers->tileRec_Xcha0;
  trans->tileRec_Xcha1      = pers->tileRec_Xcha1;
	
  trans->tileRec_btdc1  	  = pers->tileRec_btdc1;
  trans->tileRec_GainC0 	  = pers->tileRec_GainC0;
  trans->tileRec_GainC1 	  = pers->tileRec_GainC1;
  trans->tileRec_GainC2 	  = pers->tileRec_GainC2;

  trans->tileRec_MuBack     = pers->tileRec_MuBack;
  trans->tileRec_TfitC0     = pers->tileRec_TfitC0;
  trans->tileRec_TfitC1     = pers->tileRec_TfitC1;
  trans->tileRec_TfitC2     = pers->tileRec_TfitC2;
  trans->tileRec_EfitC0     = pers->tileRec_EfitC0;
  trans->tileRec_EfitC1     = pers->tileRec_EfitC1;
  trans->tileRec_EfitC2     = pers->tileRec_EfitC2;
  trans->tileRec_Chi2C0     = pers->tileRec_Chi2C0;
  trans->tileRec_Chi2C1     = pers->tileRec_Chi2C1;
  trans->tileRec_Chi2C2     = pers->tileRec_Chi2C2;
  
	trans->tileRec_SampleC0   = pers->tileRec_SampleC0;
  trans->tileRec_SampleC1   = pers->tileRec_SampleC1;
  trans->tileRec_SampleC2   = pers->tileRec_SampleC2;

}


void
TBTrackInfoCnv_p1::transToPers(const TBTrackInfo* trans, 
                                     TBTrackInfo_p1* pers, MsgStream &/*log*/)
{

  pers->tb_Trigger         = trans->tb_Trigger;
  pers->tb_trk_nTracks     = trans->tb_trk_nTracks;
  pers->tb_trk_nTracksTRT  = trans->tb_trk_nTracksTRT;

  pers->tb_sADC_S1     		 = trans->tb_sADC_S1;
  pers->tb_sADC_muHalo 	   = trans->tb_sADC_muHalo;
  pers->tb_sADC_muTag  	   = trans->tb_sADC_muTag;
  pers->tb_sADC_C2     	   = trans->tb_sADC_C2;
  pers->tb_sADC_TRTSci 	   = trans->tb_sADC_TRTSci;

  pers->tb_trk_nPixelHits  = trans->tb_trk_nPixelHits;
  pers->tb_trk_nSctHits    = trans->tb_trk_nSctHits;
  pers->tb_trk_nTrtHitsTRT = trans->tb_trk_nTrtHitsTRT;

  pers->tb_trk_Trt_HLTRT   = trans->tb_trk_Trt_HLTRT;

	pers->tileRec_XchN1      = trans->tileRec_XchN1;
  pers->tileRec_XchN2      = trans->tileRec_XchN2;
  pers->tileRec_Xcha0      = trans->tileRec_Xcha0;
  pers->tileRec_Xcha1      = trans->tileRec_Xcha1;
	
  pers->tileRec_btdc1  	   = trans->tileRec_btdc1;
  pers->tileRec_GainC0 	   = trans->tileRec_GainC0;
  pers->tileRec_GainC1 	   = trans->tileRec_GainC1;
  pers->tileRec_GainC2 	   = trans->tileRec_GainC2;

  pers->tileRec_MuBack     = trans->tileRec_MuBack;
  pers->tileRec_TfitC0     = trans->tileRec_TfitC0;
  pers->tileRec_TfitC1     = trans->tileRec_TfitC1;
  pers->tileRec_TfitC2     = trans->tileRec_TfitC2;
  pers->tileRec_EfitC0     = trans->tileRec_EfitC0;
  pers->tileRec_EfitC1     = trans->tileRec_EfitC1;
  pers->tileRec_EfitC2     = trans->tileRec_EfitC2;
  pers->tileRec_Chi2C0     = trans->tileRec_Chi2C0;
  pers->tileRec_Chi2C1     = trans->tileRec_Chi2C1;
  pers->tileRec_Chi2C2     = trans->tileRec_Chi2C2;

  pers->tileRec_SampleC0   = trans->tileRec_SampleC0;
  pers->tileRec_SampleC1   = trans->tileRec_SampleC1;
  pers->tileRec_SampleC2   = trans->tileRec_SampleC2;
}

