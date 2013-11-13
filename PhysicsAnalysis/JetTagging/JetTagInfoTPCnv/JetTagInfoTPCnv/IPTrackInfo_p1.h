/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPTRACKINFO_P1
#define JETTAGINFOTPCNV_IPTRACKINFO_P1

#include "DataModelAthenaPool/ElementLink_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Analysis {

  class IPTrackInfoCnv_p1;

  class IPTrackInfo_p1 {
    friend class IPTrackInfoCnv_p1;
    private:
      ElementLinkInt_p1 m_track;
      int m_trackGrade_int;
      std::string m_trackGrade_string;
      bool m_isFromV0;
      float m_valD0wrtPV;
      float m_sigD0wrtPV;
      float m_valZ0wrtPV;
      float m_sigZ0wrtPV;
      float m_trackWeight2D;
      float m_trackWeight3D;
      float m_trackProbJP;
      float m_trackProbJPneg;
  };

}
#endif // JETTAGINFOTPCNV_IPTRACKINFO_P1
