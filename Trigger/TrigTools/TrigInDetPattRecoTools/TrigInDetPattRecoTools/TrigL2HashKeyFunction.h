/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_HASH_KEY_FUNCTION_H__
#define __TRIG_L2_HASH_KEY_FUNCTION_H__

class TrigL2HashKeyFunction {
 public:
  TrigL2HashKeyFunction(double xbs, double ybs, long c = 1000) :
    m_invXbinSize(xbs), m_invYbinSize(ybs), m_hashKeyCoeff(c){
  }
    ~TrigL2HashKeyFunction(){};

    inline long operator()(double y, double x) {
      long Xbin = (long)(x*m_invXbinSize);
      long Ybin = (long)(y*m_invYbinSize); 
      return (m_hashKeyCoeff*Xbin + Ybin);
    }

    inline double invXbinSize() {
      return m_invXbinSize;
    }
    inline double invYbinSize() {
      return m_invYbinSize;
    }

 protected: 
    double m_invXbinSize, m_invYbinSize;
    long m_hashKeyCoeff;
};

#endif
