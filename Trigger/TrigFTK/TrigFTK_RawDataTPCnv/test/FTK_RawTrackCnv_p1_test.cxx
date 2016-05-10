/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigFTK_RawDataTPCnv/test/FTK_RawTrackCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for FTK_RawTrackCnv_p1.
 */


#undef NDEBUG
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const FTK_RawPixelCluster& p1,
              const FTK_RawPixelCluster& p2)
{
  assert (p1.getWordA() == p2.getWordA());
  assert (p1.getWordB() == p2.getWordB());
  assert (p1.getBarcode() == p2.getBarcode());
}


void compare (const FTK_RawSCT_Cluster& p1,
              const FTK_RawSCT_Cluster& p2)
{
  assert (p1.getWord() == p2.getWord());
  assert (p1.getBarcode() == p2.getBarcode());
}


void compare (const std::vector<FTK_RawPixelCluster>& p1,
              const std::vector<FTK_RawPixelCluster>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void compare (const std::vector<FTK_RawSCT_Cluster>& p1,
              const std::vector<FTK_RawSCT_Cluster>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void compare (const FTK_RawTrack& p1,
              const FTK_RawTrack& p2)
{
  assert (p1.getTH1() == p2.getTH1());
  assert (p1.getTH2() == p2.getTH2());
  assert (p1.getTH3() == p2.getTH3());
  assert (p1.getTH4() == p2.getTH4());
  assert (p1.getTH5() == p2.getTH5());
  assert (p1.getTH6() == p2.getTH6());
  assert (p1.getBarcode() == p2.getBarcode());
  compare (p1.getPixelClusters(), p2.getPixelClusters());
  compare (p1.getSCTClusters(), p2.getSCTClusters());
}


void testit (const FTK_RawTrack& trans1)
{
  MsgStream log (0, "test");
  FTK_RawTrackCnv_p1 cnv;
  FTK_RawTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  FTK_RawTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  FTK_RawTrack trans1 (123, 234, 345, 456, 567, 678);
  trans1.setBarcode (789);

  for (size_t i=0; i < trans1.getPixelClusters().size(); i++) {
    trans1.setPixelCluster (i,FTK_RawPixelCluster (11 + i*100, 22 + i*100));
    trans1.getPixelClusters()[i].setBarcode (33 + i*100);
  }
    
  for (size_t i=0; i < trans1.getSCTClusters().size(); i++) {
    trans1.setSCTCluster (i,FTK_RawSCT_Cluster (44 + i*100, 55 + i*100));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
