/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/MergedEventInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/MergedEventInfoCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const EventID& p1,
              const EventID& p2)
{
  assert (p1.run_number() == p2.run_number());
  assert (p1.event_number() == p2.event_number());
  assert (p1.time_stamp() == p2.time_stamp());
  assert (p1.time_stamp_ns_offset() == p2.time_stamp_ns_offset());
  assert (p1.lumi_block() == p2.lumi_block());
  assert (p1.bunch_crossing_id() == p2.bunch_crossing_id());
  assert (p1.detector_mask0() == p2.detector_mask0());
  assert (p1.detector_mask1() == p2.detector_mask1());
  //assert (p1.detector_mask2() == p2.detector_mask2());
  //assert (p1.detector_mask3() == p2.detector_mask3());
}


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


void compare (const TriggerInfo& p1,
              const TriggerInfo& p2)
{
  //assert (p1.statusElement() == p2.statusElement());
  assert (p1.extendedLevel1ID() == p2.extendedLevel1ID());
  assert (p1.level1TriggerType() == p2.level1TriggerType());
  //assert (p1.level1TriggerInfo() == p2.level1TriggerInfo());
  assert (p2.level2TriggerInfo().size() == 1);
  assert (p1.level2TriggerInfo()[0] == p2.level2TriggerInfo()[0]);
  assert (p1.eventFilterInfo() == p2.eventFilterInfo());

  //assert (p1.streamTags().size() == p2.streamTags().size());
  //for (size_t i = 0; i < p1.streamTags().size(); i++)
  //  compare (p1.streamTags()[i], p2.streamTags()[i]);
}


void compare (const EventInfo& p1,
              const EventInfo& p2)
{
  compare (*p1.event_ID(), *p2.event_ID());
  compare (*p1.event_type(), *p2.event_type());
  compare (*p1.trigger_info(), *p2.trigger_info());
  compare (*p1.trigger_info(), *p2.trigger_info());

#if 0
  for (unsigned int i = 0; i < EventInfo::nDets; i++) {
    EventInfo::EventFlagSubDet ii = static_cast<EventInfo::EventFlagSubDet>(i);
    assert (p1.eventFlags(ii) == p2.eventFlags(ii));
    assert (p1.errorState(ii) == p2.errorState(ii));
  }

  assert (p1.actualInteractionsPerCrossing() == p2.actualInteractionsPerCrossing());
  assert (p1.averageInteractionsPerCrossing() == p2.averageInteractionsPerCrossing());
#endif
}


void compare (const MergedEventInfo& p1,
              const MergedEventInfo& p2)
{
  compare (static_cast<const EventInfo&>(p1),
           static_cast<const EventInfo&>(p2));
  compare (*p1.event_ID(), *p2.event_ID());
}


void testit (const MergedEventInfo& trans1)
{
  MsgStream log (0, "test");
  MergedEventInfoCnv_p1 cnv;
  MergedEventInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MergedEventInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  

  auto id = std::make_unique<EventID> (1234, 2345, 3456, 4567, 5678, 6789,
                                       4321, 5432, 6543, 7654);

  auto etype = std::make_unique<EventType>();
  etype->add_type (3);
  etype->add_type (13);
  etype->add_type (25);
  etype->add_type (101);
  etype->set_user_type ("utype");
  etype->set_mc_event_weight (1.5, 0);
  etype->set_mc_event_weight (2.5, 1);
  etype->set_mc_event_weight (3.5, 2);
  etype->set_mc_channel_number (123);
  etype->set_mc_event_number (345);

  std::vector<TriggerInfo::StreamTag> streamTags;
  streamTags.emplace_back ("asd", "zzz", true,
                           std::set<TriggerInfo::number_type> {3, 4},
                           std::set<TriggerInfo::number_type> {5, 10, 12});
  streamTags.emplace_back ("fds", "qwe", false,
                           std::set<TriggerInfo::number_type> {13, 14},
                           std::set<TriggerInfo::number_type> {15, 110, 112});
  auto tinfo = std::make_unique<TriggerInfo>
    (123,
     234,
     345,
     std::vector<TriggerInfo::number_type> {6, 7, 8},
     std::vector<TriggerInfo::number_type> {9, 10},
     std::vector<TriggerInfo::number_type> {11, 12, 13, 14},
     streamTags);
                                                   
  
  EventInfo info1 (id.release(), etype.release(), tinfo.release());
  for (unsigned int i = 0; i < EventInfo::nDets; i++) {
    EventInfo::EventFlagSubDet ii = static_cast<EventInfo::EventFlagSubDet>(i);
    info1.setEventFlags (ii, i*100+3);
    info1.setErrorState (ii, static_cast<EventInfo::EventFlagErrorState>(i%3));
  }
  info1.setActualInteractionsPerCrossing (123.5);
  info1.setAverageInteractionsPerCrossing (321.5);

  MergedEventInfo trans1 (info1, 333, 444, 555);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
