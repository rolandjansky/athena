/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/EventStreamInfoCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/EventStreamInfo.h"
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

  assert (p2.n_mc_event_weights() == 1);
  assert (static_cast<unsigned int>(p1.mc_event_weight(0)) == p2.mc_event_weight(0));
  //assert (p1.mc_channel_number() == p2.mc_channel_number());
  //assert (p1.mc_event_number() == p2.mc_event_number());
}


void compare (const EventStreamInfo& p1,
              const EventStreamInfo& p2)
{
  assert (p1.getNumberOfEvents() == p2.getNumberOfEvents());
  assert (p1.getRunNumbers() == p2.getRunNumbers());
  assert (p1.getLumiBlockNumbers() == p2.getLumiBlockNumbers());
  assert (p1.getProcessingTags() == p2.getProcessingTags());
  assert (p1.getItemList() == p2.getItemList());
  const std::set<EventType>& s1 = p1.getEventTypes();
  const std::set<EventType>& s2 = p2.getEventTypes();
  assert (s1.size() == s2.size());
  std::set<EventType>::const_iterator it1 = s1.begin();
  std::set<EventType>::const_iterator it2 = s2.begin();
  for (; it1 != s1.end(); ++it1, ++it2)
    compare (*it1, *it2);
}


void testit (const EventStreamInfo& trans1)
{
  MsgStream log (0, "test");
  EventStreamInfoCnv_p2 cnv;
  EventStreamInfo_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  EventStreamInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  

  EventStreamInfo trans1;
  trans1.addEvent (1234);
  trans1.insertRunNumber(234);
  trans1.insertRunNumber(345);
  trans1.insertLumiBlockNumber(456);
  trans1.insertLumiBlockNumber(567);
  trans1.insertLumiBlockNumber(678);
  trans1.insertProcessingTag("asd");
  trans1.insertProcessingTag("sdf");
  trans1.insertItemList(321, "gfd");
  trans1.insertItemList(432, "lkj");
                                                   
  EventType etype1;
  etype1.add_type (3);
  etype1.add_type (13);
  etype1.add_type (25);
  etype1.add_type (101);
  etype1.set_user_type ("utype");
  etype1.set_mc_event_weight (1.5, 0);
  etype1.set_mc_event_weight (2.5, 1);
  etype1.set_mc_event_weight (3.5, 2);
  etype1.set_mc_channel_number (123);
  etype1.set_mc_event_number (345);

  EventType etype2;
  etype2.add_type (13);
  etype2.add_type (113);
  etype2.add_type (125);
  etype2.add_type (1101);
  etype2.set_user_type ("xtype");
  etype2.set_mc_event_weight (11.5, 0);
  etype2.set_mc_event_weight (12.5, 1);
  etype2.set_mc_event_weight (13.5, 2);
  etype2.set_mc_channel_number (1123);
  etype2.set_mc_event_number (1345);

  trans1.insertEventType (etype1);
  trans1.insertEventType (etype2);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
