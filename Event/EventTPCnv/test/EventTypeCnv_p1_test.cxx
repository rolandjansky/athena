/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventTPCnv/test/EventTypeCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/EventTypeCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/EventType.h"
#include "GaudiKernel/MsgStream.h"
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
  assert (static_cast<unsigned int>(p1.mc_event_weight(0)) == p2.mc_event_weight(0));
  //assert (p1.mc_channel_number() == p2.mc_channel_number());
  //assert (p1.mc_event_number() == p2.mc_event_number());
}


void testit (const EventType& trans1)
{
  MsgStream log (0, "test");
  EventTypeCnv_p1 cnv;
  EventType_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  EventType trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
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


int main()
{
  test1();
  return 0;
}
