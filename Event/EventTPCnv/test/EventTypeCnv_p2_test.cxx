/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/EventTypeCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/EventTypeCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/EventType.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const EventType& p1,
              const EventType& p2)
{
  EventType::BitMaskIterator beg1 = p1.bit_mask_begin();
  EventType::BitMaskIterator end1 = p1.bit_mask_end();
  EventType::BitMaskIterator beg2 = p2.bit_mask_begin();
  EventType::BitMaskIterator end2 = p2.bit_mask_end();
  assert (end1-beg1 == end2-beg2);
  assert (std::equal (beg1, end1, beg2));

  assert (p1.user_type() == p2.user_type());
  assert (p1.get_detdescr_tags() == p2.get_detdescr_tags());

  assert (p2.n_mc_event_weights() == 1);
  assert (p1.mc_event_weight(0) == p2.mc_event_weight(0));
  assert (p1.mc_channel_number() == p2.mc_channel_number());
  assert (p1.mc_event_number() == p2.mc_event_number());
}


void testit (const EventType& trans1)
{
  EventTypeCnv_p2 cnv;
  std::vector<unsigned int> pers;
  cnv.transToPers (&trans1, pers);
  EventType trans2;
  std::vector<unsigned int>::const_iterator it = pers.begin();
  int versionToWrite = 2 | (3<<6) | (3<<12) | (1<<18) | (1<<24);
  cnv.persToTrans (it, &trans2, versionToWrite, false);
  assert (it == pers.end());
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  

  EventType trans1;
  trans1.add_type (3);
  trans1.add_type (13);
  trans1.add_type (25);
  trans1.add_type (101);
  trans1.set_user_type ("utype");
  trans1.set_mc_event_weight (1.5, 0);
  trans1.set_mc_event_weight (2.5, 1);
  trans1.set_mc_event_weight (3.5, 2);
  trans1.set_mc_channel_number (123);
  trans1.set_mc_event_number (345);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
