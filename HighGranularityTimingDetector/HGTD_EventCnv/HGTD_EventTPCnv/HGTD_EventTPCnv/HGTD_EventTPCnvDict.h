/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_EventTPCnvDict.h
 * @author Noemi Calace <noemi.calace@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief
 */

#ifndef HGTD_EVENTTPCNV_HGTD_EVENTTPCNVDICT_H
#define HGTD_EVENTTPCNV_HGTD_EVENTTPCNVDICT_H

#include "HGTD_EventTPCnv/HGTD_RDO_p1.h"
#include "HGTD_EventTPCnv/HGTD_RDOCollection_p1.h"
#include "HGTD_EventTPCnv/HGTD_RDOContainer_p1.h"
#include "HGTD_EventTPCnv/HGTD_Cluster_p1.h"
#include "HGTD_EventTPCnv/HGTD_PRD_Collection_p1.h"
#include "HGTD_EventTPCnv/HGTD_ClusterContainer_p1.h"

namespace HGTD_EventTPCnvDict {
struct tmp {
  std::vector<HGTD_Cluster_p1> m_v1;
};
} // namespace HGTD_EventTPCnvDict

#endif // HGTD_EVENTTPCNV_HGTD_EVENTTPCNVDICT_H
