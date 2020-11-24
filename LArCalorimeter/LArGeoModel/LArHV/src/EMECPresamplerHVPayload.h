/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECPRESAMPLERHVPAYLOAD_H
#define LARHV_EMECPRESAMPLERHVPAYLOAD_H
#include <iostream>
struct EMECPresamplerHVPayload {
  double         voltage;
  double         current;
  int            hvLineNo;
};

inline std::ostream & operator << (std::ostream & o, const EMECPresamplerHVPayload & payload) {
  o << "HV Line No " << payload.hvLineNo << ' ' << payload.voltage << " volts; " << payload.current << " amps" << std::endl;
  return o;
}
#endif
