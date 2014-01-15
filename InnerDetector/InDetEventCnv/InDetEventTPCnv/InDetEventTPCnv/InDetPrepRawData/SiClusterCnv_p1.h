/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_CLUSTER_CNV_P1_H
#define SI_CLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SiClusterCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetPrepRawData/SiCluster.h"
#include "SiCluster_p1.h"

#include "SiWidthCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"


class MsgStream;

class SiClusterCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, InDet::SiCluster, InDet::SiCluster_p1>
{
public:
  SiClusterCnv_p1(): m_rawDataCnv(0), m_swCnv(0) {}

  void persToTrans( const InDet::SiCluster_p1 *, InDet::SiCluster *, MsgStream & );
  void transToPers( const InDet::SiCluster *, InDet::SiCluster_p1 *, MsgStream & );
        
protected:
//  ITPConverterFor<Trk::PrepRawData>	*m_rawDataCnv;
//  ITPConverterFor<InDet::SiWidth> 	*m_swCnv;
    PrepRawDataCnv_p1 *m_rawDataCnv;
    SiWidthCnv_p1  *m_swCnv;
   

};

#endif // SI_CLUSTER_CNV_P1_H
