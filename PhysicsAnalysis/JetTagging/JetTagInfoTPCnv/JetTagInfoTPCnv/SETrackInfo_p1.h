/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SETrackInfo_P1_H
#define JETTAGINFOTPCNV_SETrackInfo_P1_H

#include "DataModelAthenaPool/ElementLink_p1.h"

///
/// Persitent class for the truth info
///

#include <string>

namespace Analysis {
  class SETrackInfoCnv_p1;

  class SETrackInfo_p1 {
    friend class SETrackInfoCnv_p1;

  public:
    SETrackInfo_p1()
      : m_valD0wrtPV(0),
        m_pTrel(0)
    {}

  private:
    ElementLinkInt_p1 m_electron;
    ElementLinkInt_p1 m_photon;
    float m_valD0wrtPV;
    float m_pTrel;
    std::vector<float> m_tagLikelihood;
  };
}

#endif
