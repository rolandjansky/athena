/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EMBPresamplerHVPayload_h_
#define _EMBPresamplerHVPayload_h_
#include <iostream>
struct EMBPresamplerHVPayload {
  double         voltage[2];
  double         current[2];
  unsigned int   status[2];
  int            hvLineNo[2];
};

inline std::ostream & operator << (std::ostream & o, const EMBPresamplerHVPayload & payload) {
  o << "Half Gap #0: HV Line No " << payload.hvLineNo[0] << ' ' << payload.voltage[0] << " volts; " << payload.current[0] << " amps; status = " << payload.status[0] << std::endl;
  o << "Half Gap #1: HV Line No " << payload.hvLineNo[1] << ' ' << payload.voltage[1] << " volts; " << payload.current[1] << " amps; status = " << payload.status[1] << std::endl;
  return o;
}
#endif
