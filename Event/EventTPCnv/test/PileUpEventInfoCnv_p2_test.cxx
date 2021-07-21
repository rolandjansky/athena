/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/PileUpEventInfoCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/PileUpEventInfoCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>
#include <sstream>


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


void compare (const EventInfo& p1,
              const EventInfo& p2)
{
  compare (*p1.event_ID(), *p2.event_ID());
  compare (*p1.event_type(), *p2.event_type());
  compare (*p1.trigger_info(), *p2.trigger_info());
  compare (*p1.trigger_info(), *p2.trigger_info());

  for (unsigned int i = 0; i < EventInfo::nDets; i++) {
    EventInfo::EventFlagSubDet ii = static_cast<EventInfo::EventFlagSubDet>(i);
    assert (p1.eventFlags(ii) == p2.eventFlags(ii));
    assert (p1.errorState(ii) == p2.errorState(ii));
  }

  assert (p1.actualInteractionsPerCrossing() == p2.actualInteractionsPerCrossing());
  assert (p1.averageInteractionsPerCrossing() == p2.averageInteractionsPerCrossing());
}


void compare (const PileUpEventInfo::SubEvent& p1,
              const PileUpEventInfo::SubEvent& p2)
{
  assert (p1.time() == p2.time());
  assert (p1.index() == p2.index());
  assert (p2.type() == PileUpTimeEventIndex::Unknown);
  assert (p1.BCID() == p2.BCID());
  compare (*p1.pSubEvt, *p2.pSubEvt);
}


void compare (const PileUpEventInfo& p1,
              const PileUpEventInfo& p2)
{
  compare (static_cast<const EventInfo&>(p1),
           static_cast<const EventInfo&>(p2));

  PileUpEventInfo::SubEvent::const_iterator beg1 = p1.beginSubEvt();
  PileUpEventInfo::SubEvent::const_iterator end1 = p1.endSubEvt();
  PileUpEventInfo::SubEvent::const_iterator beg2 = p2.beginSubEvt();
  PileUpEventInfo::SubEvent::const_iterator end2 = p2.endSubEvt();
  assert (std::distance (beg1, end1) ==
          std::distance (beg2, end2));
  for (; beg1 != end1; ++beg1, ++beg2)
    compare (*beg1, *beg2);

  compare (*p1.event_ID(), *p2.event_ID());
}


void testit (const PileUpEventInfo& trans1)
{
  MsgStream log (0, "test");
  PileUpEventInfoCnv_p2 cnv;
  PileUpEventInfo_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  PileUpEventInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


std::string stroffs (const char* s, int offs)
{
  std::ostringstream os;
  os << s << offs;
  return os.str();
}


std::unique_ptr<EventID> make_id (int offs)
{
  return std::make_unique<EventID> (1234+offs, 2345+offs, 3456+offs,
                                    4567+offs, 5678+offs, 6789+offs,
                                    4321+offs, 5432+offs,
                                    6543+offs, 7654+offs);
}


std::unique_ptr<EventType> make_type (int offs)
{
  auto etype = std::make_unique<EventType>();
  etype->add_type (3+offs);
  etype->add_type (13+offs);
  etype->add_type (25+offs);
  etype->add_type (101+offs);
  etype->set_user_type (stroffs ("utype", offs));
  etype->set_mc_event_weight (1.5+offs, 0);
  etype->set_mc_event_weight (2.5+offs, 1);
  etype->set_mc_event_weight (3.5+offs, 2);
  etype->set_mc_channel_number (123+offs);
  etype->set_mc_event_number (345+offs);

  return etype;
}


std::unique_ptr<TriggerInfo> make_tinfo (TriggerInfo::number_type offs)
{
  std::vector<TriggerInfo::StreamTag> streamTags;
  streamTags.emplace_back (stroffs("asd", offs),
                           stroffs("zzz", offs), true,
                           std::set<TriggerInfo::number_type> {3+offs, 4+offs},
                           std::set<TriggerInfo::number_type> {5+offs, 10+offs, 12+offs});
  streamTags.emplace_back (stroffs("fds", offs),
                           stroffs("qwe", offs), false,
                           std::set<TriggerInfo::number_type> {13+offs, 14+offs},
                           std::set<TriggerInfo::number_type> {15+offs, 110+offs, 112+offs});
  auto tinfo = std::make_unique<TriggerInfo>
    (123+offs,
     234+offs,
     345+offs,
     std::vector<TriggerInfo::number_type> {6+offs, 7+offs, 8+offs},
     std::vector<TriggerInfo::number_type> {9+offs, 10+offs},
     std::vector<TriggerInfo::number_type> {11+offs, 12+offs, 13+offs, 14+offs},
     streamTags);

  return tinfo;
}


std::unique_ptr<EventInfo> make_ei (int offs)
{
  std::unique_ptr<EventID> id = make_id (offs);
  std::unique_ptr<EventType> etype = make_type (offs);
  std::unique_ptr<TriggerInfo> tinfo = make_tinfo (offs);
  
  auto ei = std::make_unique<EventInfo> (id.release(), etype.release(), tinfo.release());
  for (unsigned int i = 0; i < EventInfo::nDets; i++) {
    EventInfo::EventFlagSubDet ii = static_cast<EventInfo::EventFlagSubDet>(i);
    ei->setEventFlags (ii, i*100+3);
    ei->setErrorState (ii, static_cast<EventInfo::EventFlagErrorState>(i%3));
  }
  ei->setActualInteractionsPerCrossing (123.5);
  ei->setAverageInteractionsPerCrossing (321.5);

  return ei;
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  

  PileUpEventInfo trans1 (make_id(0).release(),
                          make_type(0).release(),
                          make_tinfo(0).release());
  trans1.addSubEvt (123, 432, PileUpTimeEventIndex::Signal, *make_ei(100), nullptr);
  trans1.addSubEvt (223, 632, PileUpTimeEventIndex::Cavern, *make_ei(200), nullptr);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
