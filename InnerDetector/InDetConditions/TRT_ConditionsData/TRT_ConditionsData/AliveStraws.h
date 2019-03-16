/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALIVESTRAWS_H
#define ALIVESTRAWS_H
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
namespace TRTCond{
  class AliveStraws{
  public:
    AliveStraws() {
       // create arrays for alive straws
      m_stw_total = new int[7]();
       m_stw_local = new int*[6];
       m_stw_wheel = new int*[34];
       for (int i=0; i<6 ; ++i) m_stw_local[i] = new int[32]();
       for (int i=0; i<34; ++i) m_stw_wheel[i] = new int[32]();
    }

    virtual ~AliveStraws() {}
    int  *getStwTotal() const {return m_stw_total; } 
    int **getStwLocal() const {return m_stw_local; } 
    int **getStwWheel() const {return m_stw_wheel; } 
    void update(const int& i, const int& j, const int& phi) {

      m_stw_total[0]                        +=1;
      m_stw_total[i]                        +=1;
      m_stw_local[i-1][phi]                 +=1;
      m_stw_wheel[j][phi]                   +=1;
    }

  private:
    int *m_stw_total;
    int **m_stw_local;
    int **m_stw_wheel;
  };
}
CLASS_DEF(TRTCond::AliveStraws,234870469,1)
CONDCONT_DEF(TRTCond::AliveStraws,110897079);
#endif
