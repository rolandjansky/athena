/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FATRASEVENTTPCNV_ISF_FATRASEVENTTPCNVDICT_H
#define ISF_FATRASEVENTTPCNV_ISF_FATRASEVENTTPCNVDICT_H

//-----------------------------------------------------------------------------
//
// file ISF_FatrasEventTPCnvDict.h
//
//-----------------------------------------------------------------------------

#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p1.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p1.h"

#include "ISF_FatrasEventTPCnv/ISF_FatrasEvent/PlanarCluster_p2.h"
#include "ISF_FatrasEventTPCnv/PlanarClusterContainer_p2.h"

#include <vector>

namespace ISF_FatrasEventTPCnvDict{
  struct tmp {
    std::vector < iFatras::PlanarCluster_p1 > m_v1;
    std::vector < iFatras::PlanarCluster_p2 > m_v2;
  };
}

#endif // ISF_FATRASEVENTTPCNV_ISF_FATRASEVENTTPCNVDICT_H
