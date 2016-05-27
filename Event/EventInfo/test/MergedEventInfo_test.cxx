/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
using namespace std;

int main() {
  cout << "*** MergedEventInfo_test starts ***" << endl;
  MergedEventInfo empty;
  assert( static_cast<EventID::number_type>(-1) == empty.event_ID()->run_number() || // Gaudi 27
          0 == empty.event_ID()->run_number()); // Gaudi 26
  EventInfo anInfo(new EventID(1, 23), new EventType(), new TriggerInfo());

  MergedEventInfo merged(anInfo, 3, 33);
  assert(3 == merged.event_ID()->run_number());
  assert(33 == merged.event_ID()->event_number());
  assert(1 == merged.origEvent_ID()->run_number());
  assert(23 == merged.origEvent_ID()->event_number());

  MergedEventInfo copy(merged);
  assert(3 == copy.event_ID()->run_number());
  assert(33 == copy.event_ID()->event_number());
  assert(1 == copy.origEvent_ID()->run_number());
  assert(23 == copy.origEvent_ID()->event_number());

  cout << "*** MergedEventInfo_test OK ***" << endl;
  return 0;
}
