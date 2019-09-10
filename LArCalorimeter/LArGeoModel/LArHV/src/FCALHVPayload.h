/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVPAYLOAD_H
#define LARHV_FCALHVPAYLOAD_H
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
