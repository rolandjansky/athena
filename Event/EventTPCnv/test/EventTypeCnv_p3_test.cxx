/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/EventTypeCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/EventTypeCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/EventType.h"
#include "GaudiKernel/MsgStream.h"
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

  assert (p1.n_mc_event_weights() == p2.n_mc_event_weights());
  for (unsigned int i = 0; i < p1.n_mc_event_weights(); i++)
    assert (p1.mc_event_weight(i) == p2.mc_event_weight(i));
  assert (p1.mc_channel_number() == p2.mc_channel_number());
  assert (p1.mc_event_number() == p2.mc_event_number());
}


void testit (const EventType& trans1)
{
  MsgStream log (0, "test");
  EventTypeCnv_p3 cnv;
  EventType_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  EventType trans2;
  cnv.persToTrans (&pers, &trans2, log);
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
