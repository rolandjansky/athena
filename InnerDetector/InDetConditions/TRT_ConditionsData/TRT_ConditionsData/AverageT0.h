/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AVERAGET0_H
#define AVERAGET0_H
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
namespace TRTCond{
  class AverageT0{
  public:
    AverageT0() {
       m_avgT0 = 0.;
    }

    virtual ~AverageT0() {}
    // Store average T0
    void update(const float& at0 ) { m_avgT0=at0; }
    double get() const {return m_avgT0;}

  private:
    double m_avgT0;
  };
}
CLASS_DEF(TRTCond::AverageT0,221681781,1)
CONDCONT_DEF(TRTCond::AverageT0,71803019);
#endif
