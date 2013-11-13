/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_AtlfInfo_P1_H
#define JETTAGINFOTPCNV_AtlfInfo_P1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

///
/// Persistent class for the ATL Fast tag info.
///

namespace Analysis {

  class AtlfInfo_p1 {
    friend class AtlfInfoCnv_p1;

  public:
    inline AtlfInfo_p1(void);

  private:
    TPObjRef m_BaseTagInfo;

    bool m_isBTagged; 
    bool m_isTauTagged; 
    bool m_isTau1PTagged; 
    bool m_isTau3PTagged; 
    float m_calFactLight;
    float m_calFactTau;
    float m_calFactTau1P3P;
    float m_calFactB;
    float m_distanceToBQuark;
    float m_distanceToCQuark;
    float m_distanceToTLepton;
  };

  /// Zero out the container when we do the init!
  AtlfInfo_p1::AtlfInfo_p1()
    : m_isBTagged(false),
    m_isTauTagged (false),
    m_isTau1PTagged (false),
    m_isTau3PTagged(false),
    m_calFactLight(0.0),
    m_calFactTau(0.0),
    m_calFactTau1P3P(0.0),
    m_calFactB(0.0),
    m_distanceToBQuark(0.0),
    m_distanceToCQuark(0.0),
    m_distanceToTLepton(0.0)
    {
    }
}

#endif
