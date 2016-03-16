/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFTCIRCLECONTAINER_TLP1_H
#define TRT_DRIFTCIRCLECONTAINER_TLP1_H




//-----------------------------------------------------------------------------
// InDetPrepRawData
//-----------------------------------------------------------------------------
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace InDet
{
   class TRT_DriftCircleContainer_tlp1
   {
  public:
     TRT_DriftCircleContainer_tlp1() {}
     TPCnvTokenList_p1		             m_tokenList;

// This is the basic info
     std::vector< InDet::InDetPRD_Container_p1> m_trtDCCont;
     std::vector< InDet::TRT_DriftCircle_p1 >       m_trtDriftCircles;
// This is the base class
     std::vector< Trk::PrepRawData_p1>              m_prepData;
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>            m_locPos;
     std::vector< Trk::HepSymMatrix_p1>             m_errorMat;

   };
}


#endif
