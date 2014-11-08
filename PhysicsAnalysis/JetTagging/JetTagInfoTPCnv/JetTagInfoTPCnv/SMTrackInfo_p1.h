/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SMTrackInfo_P1_H
#define JETTAGINFOTPCNV_SMTrackInfo_P1_H

#include "DataModelAthenaPool/ElementLink_p1.h"

///
/// Persitent class for the truth info
///

#include <string>

namespace Analysis {
  class SMTrackInfoCnv_p1;

  class SMTrackInfo_p1 {
    friend class SMTrackInfoCnv_p1;

  public:
    SMTrackInfo_p1()
      : m_valD0wrtPV(0),
        m_pTrel(0)
    {}

  private:
    ElementLinkInt_p1 m_muon;
    float m_valD0wrtPV;
    float m_pTrel;
    std::vector<float> m_tagLikelihood;
  };
}

#endif
