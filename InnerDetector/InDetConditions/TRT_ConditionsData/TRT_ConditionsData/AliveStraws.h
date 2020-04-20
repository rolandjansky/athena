/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALIVESTRAWS_H
#define ALIVESTRAWS_H
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include <cassert>
#include <array>

namespace TRTCond{

  class AliveStraws{
  public:
    static constexpr unsigned int NTOTAL = 7;
    static constexpr unsigned int NLOCAL = 6;
    static constexpr unsigned int NWHEEL = 34;
    static constexpr unsigned int NLOCALPHI = 32;
    AliveStraws() {}

    virtual ~AliveStraws() {}
    const std::array<int,NTOTAL>                       &getStwTotal() const {return m_stw_total; }
    const std::array<std::array<int,NLOCALPHI>,NLOCAL> &getStwLocal() const {return m_stw_local; }
    const std::array<std::array<int,NLOCALPHI>,NWHEEL> &getStwWheel() const {return m_stw_wheel; }
    void update(const unsigned int i, const unsigned int j, const unsigned int phi) {
      assert(i>0 && i<m_stw_total.size() && i-1<m_stw_local.size() && j<m_stw_wheel.size() && phi<NLOCALPHI);
      m_stw_total[0]                        +=1;
      m_stw_total[i]                        +=1;
      m_stw_local[i-1][phi]                 +=1;
      m_stw_wheel[j][phi]                   +=1;
    }

  private:
    std::array<int,NTOTAL>                   m_stw_total {};
    std::array<std::array<int,NLOCALPHI>,NLOCAL> m_stw_local {};
    std::array<std::array<int,NLOCALPHI>,NWHEEL> m_stw_wheel {};
  };

}
CLASS_DEF(TRTCond::AliveStraws,234870469,1)
CONDCONT_DEF(TRTCond::AliveStraws,110897079);
#endif
