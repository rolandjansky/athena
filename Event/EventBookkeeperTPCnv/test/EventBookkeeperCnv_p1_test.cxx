/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventBookkeeperTPCnv/test/EventBookkeeperCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for EventBookkeeperCnv_p1.
 */


#undef NDEBUG
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p1.h"
#include "CxxUtils/StrFormat.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const EventBookkeeper& p1,
              const EventBookkeeper& p2)
{
  assert (p1.getName() == p2.getName());
  assert (p1.getDescription() == p2.getDescription());
  //assert (p1.getInputStream() == p2.getInputStream());
  assert (p1.getOutputStream() == p2.getOutputStream());
  assert (p1.getLogic() == p2.getLogic());
  assert (p1.getNAcceptedEvents() == p2.getNAcceptedEvents());
  assert (p1.getNWeightedAcceptedEvents() == p2.getNWeightedAcceptedEvents());
  assert (p1.getCycle() == p2.getCycle());

#if 0
  if (p1.getChildrenEventBookkeepers() || p2.getChildrenEventBookkeepers()) {
    const std::vector<EventBookkeeper*>& v1 =*p1.getChildrenEventBookkeepers();
    const std::vector<EventBookkeeper*>& v2 =*p2.getChildrenEventBookkeepers();
    assert (v1.size() == v2.size());
    for (size_t i = 0; i < v1.size(); i++) {
      compare (*v1[i], *v2[i]);
    }
  }
#endif
  assert (p2.getChildrenEventBookkeepers()->empty());
}


void testit (const EventBookkeeper& trans1)
{
  MsgStream log (0, "test");
  EventBookkeeperCnv_p1 cnv;
  EventBookkeeper_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  EventBookkeeper trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


std::unique_ptr<EventBookkeeper> make (int o)
{
  std::string ostr = CxxUtils::strformat ("%d", o);
  auto p = std::make_unique<EventBookkeeper> ("name" + ostr,
                                              "desc" + ostr,
                                              "logic" + ostr);
  p->setInputStream ("inp");
  p->setOutputStream ("out");
  p->setNAcceptedEvents (o*10);
  p->setNWeightedAcceptedEvents (o*12.5);
  p->setCycle (12345);
  return p;
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena_test::Leakcheck check;

  std::unique_ptr<EventBookkeeper> trans1 = make(1);
  trans1->AddChild (make(2).release());
  std::unique_ptr<EventBookkeeper> c3 = make(3);
  c3->AddChild (make(4).release());
  c3->AddChild (make(5).release());
  trans1->AddChild (c3.release());
    
  testit (*trans1);
}


int main()
{
  test1();
  return 0;
}
