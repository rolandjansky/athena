/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetStatus.cxx - class to hold status of one detector
// Richard Hawkings, started 19/12/06
#include "DetDescrConditions/DetStatus.h"

DetStatus::DetStatus() {}

DetStatus::DetStatus(const int fullcode, const float deadfrac,
 			    const float deadthrust) :
  m_code(fullcode), m_deadfrac(deadfrac), m_deadthrust(deadthrust) {}

std::string DetStatus::colour() const {
  // endcoding of colours here 0=unknown, 1=RED, 2=YELLOW, 3=GREEN
  // only lowest two bits are used - rest is free for subdetector use
  // unless code is -1, indicating BLACK (absent)
  if (m_code<0) return "BLACK"; 
  int code=m_code & 3;
  if (code==1) return "RED";
  if (code==2) return "YELLOW";
  if (code==3) return "GREEN";
  return "UNKNOWN";
}
