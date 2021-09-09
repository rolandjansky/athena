/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/TriggerInfoCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/TriggerInfoCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const TriggerInfo::StreamTag& p1,
              const TriggerInfo::StreamTag& p2)
{
  assert (p1.name() == p2.name());
  assert (p1.type() == p2.type());
  assert (p1.obeysLumiblock() == p2.obeysLumiblock());
  //assert (p1.robs() == p2.robs());
  //assert (p1.dets() == p2.dets());
}


void compare (const TriggerInfo& p1,
              const TriggerInfo& p2)
{
  assert (p1.statusElement() == p2.statusElement());
  assert (p1.extendedLevel1ID() == p2.extendedLevel1ID());
  assert (p1.level1TriggerType() == p2.level1TriggerType());
  assert (p1.level1TriggerInfo() == p2.level1TriggerInfo());
  assert (p1.level2TriggerInfo() == p2.level2TriggerInfo());
  assert (p1.eventFilterInfo() == p2.eventFilterInfo());

  assert (p1.streamTags().size() == p2.streamTags().size());
  for (size_t i = 0; i < p1.streamTags().size(); i++)
    compare (p1.streamTags()[i], p2.streamTags()[i]);
}


void testit (const TriggerInfo& trans1)
{
  MsgStream log (0, "test");
  TriggerInfoCnv_p2 cnv;
  TriggerInfo_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TriggerInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  

  std::vector<TriggerInfo::StreamTag> streamTags;
  streamTags.emplace_back ("asd", "zzz", true,
                           std::set<TriggerInfo::number_type> {3, 4},
                           std::set<TriggerInfo::number_type> {5, 10, 12});
  streamTags.emplace_back ("fds", "qwe", false,
                           std::set<TriggerInfo::number_type> {13, 14},
                           std::set<TriggerInfo::number_type> {15, 110, 112});
  TriggerInfo trans1
    (123,
     234,
     345,
     std::vector<TriggerInfo::number_type> {6, 7, 8},
     std::vector<TriggerInfo::number_type> {9, 10},
     std::vector<TriggerInfo::number_type> {11, 12, 13, 14},
     streamTags);
                                                   
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
