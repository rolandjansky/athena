/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _FCALHVPayload_h_
#define _FCALHVPayload_h_
#include <iostream>
struct FCALHVPayload {
  double         voltage;
  double         current;
  unsigned int   status;
  int            hvLineNo;
};

inline std::ostream & operator << (std::ostream & o, const FCALHVPayload & payload) {
  o << "FCAL Line:  HV Line No " << payload.hvLineNo << ' '  << payload.voltage << " volts; " << payload.current << " amps; status = " << payload.status << std::endl;
  return o;
}
#endif
