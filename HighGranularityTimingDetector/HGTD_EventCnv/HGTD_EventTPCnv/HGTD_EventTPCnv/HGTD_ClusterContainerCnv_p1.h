/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_ClusterContainerCnv_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Converts a HGTD_ClusterContainer into its persistent representation,
          where the individual collections of HGTD_Cluster are flattened in
          a single vector. An additional structure is used to store separator
          positions between the original collections as well as the collection
          identifiers.
 */

#ifndef HGTD_CLUSTERCONTAINERCNV_P1_H
#define HGTD_CLUSTERCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HGTD_EventTPCnv/HGTD_ClusterContainer_p1.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"

class HGTD_ID;
class StoreGateSvc;
class HGTD_DetectorManager;

class HGTD_ClusterContainerCnv_p1
    : public T_AthenaPoolTPCnvBase<HGTD_ClusterContainer,
                                   HGTD_ClusterContainer_p1> {
public:
  HGTD_ClusterContainerCnv_p1() : m_is_initialized(false){};

  typedef HGTD_ClusterContainer_p1 Pers_t;
  typedef HGTD_ClusterContainer Trans_t;

  virtual void persToTrans(const Pers_t* persistent_container,
                           Trans_t* transient_container, MsgStream& log);
  virtual void transToPers(const Trans_t* transient_container,
                           Pers_t* persistent_container, MsgStream& log);

  virtual Trans_t* createTransient(const Pers_t* persistent_container,
                                   MsgStream& log);

private:
  StatusCode initialize(MsgStream& log);

  const HGTD_ID* m_hgtd_idhelper;

  bool m_is_initialized;
};

#endif
