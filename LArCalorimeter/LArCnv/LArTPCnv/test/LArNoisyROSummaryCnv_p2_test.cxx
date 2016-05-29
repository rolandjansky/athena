/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArTPCnv/test/LArNoisyROSSummaryCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArNoisyROSSummaryCnv_p2.
 */


#undef NDEBUG
#include "LArTPCnv/LArNoisyROSummaryCnv_p2.h"
#include <cassert>
#include <iostream>


void compare (const LArNoisyROSummary& p1,
              const LArNoisyROSummary& p2)
{
  assert (p1.get_noisy_febs() == p2.get_noisy_febs());
  assert (p1.get_noisy_preamps() == p2.get_noisy_preamps());
  assert (p1.BadFEBFlaggedPartitions() == p2.BadFEBFlaggedPartitions());
  //assert (p1.BadFEB_WFlaggedPartitions() == p2.BadFEB_WFlaggedPartitions());
  assert (p1.SatMediumFlaggedPartitions() == p2.SatMediumFlaggedPartitions());
  assert (p1.SatTightFlaggedPartitions() == p2.SatTightFlaggedPartitions());
  //assert (p1.MNBLooseFlaggedPartitions() == p2.MNBLooseFlaggedPartitions());
  //assert (p1.MNBTightFlaggedPartitions() == p2.MNBTightFlaggedPartitions());
}


void testit (const LArNoisyROSummary& trans1)
{
  MsgStream log (0, "test");
  LArNoisyROSummaryCnv_p2 cnv;
  LArNoisyROSummary_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArNoisyROSummary trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LArNoisyROSummary trans1;
  trans1.add_noisy_feb (HWIdentifier (0x1234));
  trans1.add_noisy_feb (HWIdentifier (0x1235));
  trans1.add_noisy_feb (HWIdentifier (0x1236));
  trans1.add_noisy_preamp (HWIdentifier (0x2134), 10);
  trans1.add_noisy_preamp (HWIdentifier (0x2135), 11);
  trans1.SetBadFEBFlaggedPartitions (0x12);
  trans1.SetBadFEB_WFlaggedPartitions (0x23);
  trans1.SetSatMediumFlaggedPartitions (0x34);
  trans1.SetSatTightFlaggedPartitions (0x45);
  trans1.SetMNBTightFlaggedPartitions (0x56);
  trans1.SetMNBLooseFlaggedPartitions (0x67);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
