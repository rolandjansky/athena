/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
using namespace std;
#include "../src/AtlasMcWeight.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
using namespace AtlasMcWeight;

#define AMC_DEBUG 1

double deltaMax(double weight) {
  const double DELTAMAX(numeric_limits<float>::epsilon());
  double rounding(fabs(weight)/1e8);
  return DELTAMAX<rounding ? rounding : DELTAMAX ;
}
int main() {
  cout << "*** AtlasMcWeight_test starts ***" << endl;

  const double MAXENCODE(999999999.999);
  const unsigned short NUMNOTP_EXPNOTP(3);

#ifdef AMC_DEBUG
  cout << "deltaMax(0.0) = " << deltaMax(0.0) << endl;
  cout << "deltaMax(999999999) = " << deltaMax(999999999) << endl;
  cout << "encode(0.0) = " << encode(0.0) << endl;
  cout << "decode(0) = " << decode(0) << endl;
  cout << "encode(MAXENCODE) = " << encode(MAXENCODE) << endl;
#endif
  assert( NUMNOTP_EXPNOTP == encode(0.0) );
  assert( deltaMax(999999999) >= fabs(double(999999999 - encode(MAXENCODE))) );
  assert( deltaMax(0) >= fabs( decode(0)) );

  try {
    encode(MAXENCODE + 1);
    assert(0);
  } catch (...) {}

  cout << "*** encode/decode OK ***" << endl;


  EventInfo evt;
  EventInfo* pUpdEvt(updatedEventInfo(evt, 22.0));
  assert( pUpdEvt );
#ifdef AMC_DEBUG
  cout << getEventWeight(*pUpdEvt) << endl;
#endif  
  assert( deltaMax(22.0) >= abs(22.0 - getEventWeight(*pUpdEvt)) );
  delete pUpdEvt;

  EventInfo* pNUpdEvt(updatedEventInfo(evt, -22.373));
  assert( pNUpdEvt );
#ifdef AMC_DEBUG
  cout << getEventWeight(*pNUpdEvt) << endl;
#endif  
  assert( deltaMax(-22.373) >= abs(-22.373 - getEventWeight(*pNUpdEvt)) );
  delete pNUpdEvt;

  EventInfo* pNNUpdEvt(updatedEventInfo(evt, -.0000373));
  assert( pNNUpdEvt );
#ifdef AMC_DEBUG
  cout << getEventWeight(*pNNUpdEvt) << endl;
#endif  
  assert( deltaMax(-.0000373) >= abs(-.0000373 - getEventWeight(*pNNUpdEvt)) );
  delete pNNUpdEvt;

  EventInfo* pNPUpdEvt(updatedEventInfo(evt, -99999999.9));
  assert( pNPUpdEvt );
#ifdef AMC_DEBUG
  cout << getEventWeight(*pNPUpdEvt) << endl;
#endif  
  assert( deltaMax(-99999999.9) >= abs(-99999999.9 - getEventWeight(*pNPUpdEvt)) );
  delete pNPUpdEvt;


  cout << "*** get/set event weight OK ***" << endl;

  cout << "*** AtlasMcWeight_test OK ***" << endl;
  return 0;
}
