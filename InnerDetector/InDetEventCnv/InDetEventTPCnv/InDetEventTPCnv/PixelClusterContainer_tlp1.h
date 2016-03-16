/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCLUSTERCONTAINER_TLP1_H
#define PIXELCLUSTERCONTAINER_TLP1_H




//-----------------------------------------------------------------------------
// InDetPrepRawData
//-----------------------------------------------------------------------------
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace InDet
{
   class PixelClusterContainer_tlp1
   {
  public:
     PixelClusterContainer_tlp1() {}
     TPCnvTokenList_p1		             m_tokenList;

// This is the basic info
     std::vector< InDet::InDetPRD_Container_p1>  m_pixDCCont;
     std::vector< InDet::PixelCluster_p1 >       m_pixClusters;
// This are the base classes
     std::vector< InDet::SiCluster_p1 >          m_siClusters;
     std::vector< InDet::SiWidth_p1 >          m_siWidths;
     std::vector< Trk::PrepRawData_p1>              m_prepData;
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>            m_locPos;
     std::vector< Trk::HepSymMatrix_p1>             m_errorMat;

   };
}


#endif
