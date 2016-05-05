/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
using namespace std;

int main() {
  cout << "*** PileUpEventInfo_test starts ***" << endl;
  EventInfo *pEI = new EventInfo();
  PileUpEventInfo::SubEvent empty;
  assert(0 == empty.index());
  assert(0 == empty.time());

  PileUpEventInfo::SubEvent sette(7, pEI, 0);
  assert(sette.time() == 7);
  assert(0 == sette.index());

  PileUpEventInfo::SubEvent setteOtto(7, 8, pEI, 0);
  assert(setteOtto.time()==7);
  assert(8 == setteOtto.index());

  PileUpEventInfo::SubEvent menoSetteOtto(-7, 8, 0, 0);
  assert(menoSetteOtto.time()== - 7.0);
  assert(8 == menoSetteOtto.index());


  PileUpEventInfo test;
  test.addSubEvt(7, PileUpTimeEventIndex::Signal, 0, 0);
  test.addSubEvt(5, PileUpTimeEventIndex::Cavern, 0, 0);
  test.addSubEvt(6, PileUpTimeEventIndex::HaloGas, 0, 0);
  PileUpEventInfo::SubEvent& se(*test.beginSubEvt());
  assert(se.time() == 7);
  assert(0 == se.index());
  se=*(++test.beginSubEvt());
  assert(se.time() == 5);
  assert(1 == se.index());

  cout << "*** PileUpEventInfo_test OK ***" << endl;
  return 0;
}
