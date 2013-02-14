/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _HECHVPayload_h_
#define _HECHVPayload_h_
#include <iostream>
struct HECHVPayload {
  double         voltage;
  double         current;
  unsigned int   status;
  int            hvLineNo;
};

inline std::ostream & operator << (std::ostream & o, const HECHVPayload & payload) {
  o << "HEC Subgap: HV Line No "  << payload.hvLineNo    << ' ' << payload.voltage << " volts; " << payload.current << " amps; status = " << payload.status << std::endl;
  return o;
}
#endif
