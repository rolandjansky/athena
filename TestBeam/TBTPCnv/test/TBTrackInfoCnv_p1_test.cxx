/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBTrackInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBTrackInfoCnv_p1.h"
#include "TBEvent/TBTrackInfo.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBTrackInfo& p1,
              const TBTrackInfo& p2)
{
  assert (p1.tb_Trigger == p2.tb_Trigger);
  assert (p1.tb_trk_nTracks == p2.tb_trk_nTracks);
  assert (p1.tb_trk_nTracksTRT == p2.tb_trk_nTracksTRT);
  assert (p1.tb_sADC_S1 == p2.tb_sADC_S1);
  assert (p1.tb_sADC_muHalo == p2.tb_sADC_muHalo);
  assert (p1.tb_sADC_muTag == p2.tb_sADC_muTag);
  assert (p1.tb_sADC_C2 == p2.tb_sADC_C2);
  assert (p1.tb_sADC_TRTSci == p2.tb_sADC_TRTSci);
  assert (p1.tb_trk_nPixelHits == p2.tb_trk_nPixelHits);
  assert (p1.tb_trk_nSctHits == p2.tb_trk_nSctHits);
  assert (p1.tb_trk_nTrtHitsTRT == p2.tb_trk_nTrtHitsTRT);
  assert (p1.tb_trk_Trt_HLTRT == p2.tb_trk_Trt_HLTRT);
  assert (p1.tileRec_XchN1 == p2.tileRec_XchN1);
  assert (p1.tileRec_XchN2 == p2.tileRec_XchN2);
  assert (p1.tileRec_Xcha0 == p2.tileRec_Xcha0);
  assert (p1.tileRec_Xcha1 == p2.tileRec_Xcha1);
  assert (p1.tileRec_btdc1 == p2.tileRec_btdc1);
  assert (p1.tileRec_GainC0 == p2.tileRec_GainC0);
  assert (p1.tileRec_GainC1 == p2.tileRec_GainC1);
  assert (p1.tileRec_GainC2 == p2.tileRec_GainC2);
  assert (p1.tileRec_MuBack == p2.tileRec_MuBack);
  assert (p1.tileRec_TfitC0 == p2.tileRec_TfitC0);
  assert (p1.tileRec_TfitC1 == p2.tileRec_TfitC1);
  assert (p1.tileRec_TfitC2 == p2.tileRec_TfitC2);
  assert (p1.tileRec_EfitC0 == p2.tileRec_EfitC0);
  assert (p1.tileRec_EfitC1 == p2.tileRec_EfitC1);
  assert (p1.tileRec_EfitC2 == p2.tileRec_EfitC2);
  assert (p1.tileRec_Chi2C0 == p2.tileRec_Chi2C0);
  assert (p1.tileRec_Chi2C1 == p2.tileRec_Chi2C1);
  assert (p1.tileRec_Chi2C2 == p2.tileRec_Chi2C2);
  assert (p1.tileRec_SampleC0 == p2.tileRec_SampleC0);
  assert (p1.tileRec_SampleC1 == p2.tileRec_SampleC1);
  assert (p1.tileRec_SampleC2 == p2.tileRec_SampleC2);
}


void testit (const TBTrackInfo& trans1)
{
  MsgStream log (0, "test");
  TBTrackInfoCnv_p1 cnv;
  TBTrackInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBTrackInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBTrackInfo trans1;
  trans1.tb_Trigger = 1;
  trans1.tb_trk_nTracks = 2;
  trans1.tb_trk_nTracksTRT = 3;

  trans1.tb_sADC_S1 = 4.5;
  trans1.tb_sADC_muHalo = 5.5;
  trans1.tb_sADC_muTag = 6.5;
  trans1.tb_sADC_C2 = 7.5;
  trans1.tb_sADC_TRTSci = 8.5;

  trans1.tb_trk_nPixelHits.assign ({1, 2, 3});
  trans1.tb_trk_nSctHits.assign ({4, 3});
  trans1.tb_trk_nTrtHitsTRT.assign ({7, 8, 9, 10});

  trans1.tb_trk_Trt_HLTRT.assign ({{1, 2}, {6, 5}});

  trans1.tileRec_XchN1 = 9.5;
  trans1.tileRec_XchN2 = 10.5;
  trans1.tileRec_Xcha0 = 11.5;
  trans1.tileRec_Xcha1 = 12.5;

  trans1.tileRec_btdc1.assign ({20, 21, 22});
  trans1.tileRec_GainC0.assign ({25, 24});
  trans1.tileRec_GainC1.assign ({29, 28});
  trans1.tileRec_GainC2.assign ({30});

  trans1.tileRec_MuBack.assign ({40.5});
  trans1.tileRec_TfitC0.assign ({41.5, 42.5});
  trans1.tileRec_TfitC1.assign ({43.5, 44.5, 45.5});
  trans1.tileRec_TfitC2.assign ({46.5});
  trans1.tileRec_EfitC0.assign ({47.5, 48.5});
  trans1.tileRec_EfitC1.assign ({49.5, 50.5, 51.5});
  trans1.tileRec_EfitC2.assign ({52.5});
  trans1.tileRec_Chi2C0.assign ({53.5, 54.5});
  trans1.tileRec_Chi2C1.assign ({55.5, 56.5, 57.5});
  trans1.tileRec_Chi2C2.assign ({58.5});

  trans1.tileRec_SampleC0.assign ({{90, 91}, {92, 93}});
  trans1.tileRec_SampleC1.assign ({{94, 95, 96}});
  trans1.tileRec_SampleC2.assign ({{97}, {98}, {99}});

  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBTrackInfoCnv_p1_test\n";
  test1();
  return 0;
}
