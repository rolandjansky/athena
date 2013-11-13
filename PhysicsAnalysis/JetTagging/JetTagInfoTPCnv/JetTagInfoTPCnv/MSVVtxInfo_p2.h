/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_MSVVtxInfo_P2_H
#define JETTAGINFOTPCNV_MSVVtxInfo_P2_H


#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <string>

namespace Analysis {
  class MSVVtxInfoCnv_p2;

  class MSVVtxInfo_p2 {
    friend class MSVVtxInfoCnv_p2;

  private:

    /// All of this data will be written out.
    // Points to a Trk::RecVertex
    TPObjRef m_recsvx;

    float          m_masssvx;
    float          m_ptsvx;
    float          m_etasvx;
    float          m_phisvx;
    float          m_efracsvx;
    float          m_normdist;

    // Points to SVTrackInfo list - which is just some element pointers.
    ElementLinkIntVector_p1 m_trackinfo;
  };
}

#endif
