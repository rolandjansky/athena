/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVPAYLOAD_H
#define LARHV_EMECHVPAYLOAD_H
#include <iostream>
struct EMECHVPayload {
  double         voltage[2];
  double         current[2];
  int            hvLineNo[2];
};


inline std::ostream & operator << (std::ostream & o, const EMECHVPayload & payload) {
  o << "Half Gap #0: HV Line No " << payload.hvLineNo[0] << ' ' << payload.voltage[0] << " volts; " << payload.current[0] << " amps" << std::endl;
  o << "Half Gap #1: HV Line No " << payload.hvLineNo[1] << ' ' << payload.voltage[1] << " volts; " << payload.current[1] << " amps" << std::endl;
  return o;
}
#endif
