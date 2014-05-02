/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARRAWCONDITIONS_LARSINGLEFLOATP
#define LARRAWCONDITIONS_LARSINGLEFLOATP

#include "LArElecCalib/LArCalibErrorCode.h"

class  LArSingleFloatP{
 public:
  enum {ERRORCODE = LArElecCalib::ERRORCODE};
  LArSingleFloatP() : m_data(ERRORCODE) {} 
  LArSingleFloatP(const float& f) : m_data(f) {} 
  bool isEmpty() const {return (m_data<=1.0+ERRORCODE);};
  operator float() {return m_data;};
  operator float() const {return m_data;};
  float m_data;
};



#endif 

