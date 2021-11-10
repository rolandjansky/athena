/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/HGTD_ClusterCnv_p1.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Transient/Persistent converter for the HGTD_Cluster class.
 *
 * TODO: For simplification and to have a first running version, the Identifiers
 * are written to file directly. This can be improved in the future to have
 * decrease the size needed for storage.
 */

#ifndef HGTD_EVENTTPCNV_HGTD_CLUSTER_CNV_P1_H
#define HGTD_EVENTTPCNV_HGTD_CLUSTER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HGTD_EventTPCnv/HGTD_Cluster_p1.h"
#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidthCnv_p2.h"

class MsgStream;

class HGTD_ClusterCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<Trk::PrepRawData, HGTD_Cluster,
                                       HGTD_Cluster_p1> {
public:
  HGTD_ClusterCnv_p1() = default;

  void persToTrans(const HGTD_Cluster_p1*, HGTD_Cluster*,
                   MsgStream&);

  void transToPers(const HGTD_Cluster*, HGTD_Cluster_p1*,
                   MsgStream&);

  // needs to be public since it is called in the container converter
  HGTD_Cluster
  createHGTDCluster(const HGTD_Cluster_p1* pers_obj,
                    const InDetDD::SolidStateDetectorElementBase* delEl,
                    MsgStream& log);

protected:
  InDet::SiWidthCnv_p2 m_si_width_cnv;
};

#endif // HGTD_EVENTTPCNV_HGTD_CLUSTER_CNV_P1_H
