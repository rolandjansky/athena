/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTrackInfo.h"

TBTrackInfo::TBTrackInfo()
  : tb_Trigger (0),
    tb_trk_nTracks(0),
    tb_trk_nTracksTRT(0),
    tb_sADC_S1(0),
    tb_sADC_muHalo(0),
    tb_sADC_muTag(0),
    tb_sADC_C2(0),
    tb_sADC_TRTSci(0),
    tileRec_XchN1(0),
    tileRec_XchN2(0),
    tileRec_Xcha0(0),
    tileRec_Xcha1(0)
{
}


TBTrackInfo::~TBTrackInfo(){}
