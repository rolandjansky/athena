/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_ON_TRACK_CNV_P1_H
#define SCT_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrack_p1.h"

#include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrackCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/SCT_ClusterContainer.h"


class MsgStream;

class SCT_ClusterOnTrackCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
				       InDet::SCT_ClusterOnTrack,
				       InDet::SCT_ClusterOnTrack_p1 >
{
public:
  SCT_ClusterOnTrackCnv_p1() : m_scCnv(0) {}
  
  void persToTrans( const InDet::SCT_ClusterOnTrack_p1 *persObj,
		    InDet::SCT_ClusterOnTrack    *transObj,
		    MsgStream                      &log );
  void transToPers( const InDet::SCT_ClusterOnTrack    *transObj,
		    InDet::SCT_ClusterOnTrack_p1 *persObj,
		    MsgStream                      &log );

protected:
  SiClusterOnTrackCnv_p1*	m_scCnv;
  ElementLinkCnv_p1< ElementLinkToIDCSCT_ClusterContainer >     m_elCnv;     

};

#endif // SCT_CLUSTER_ON_TRACK_CNV_P1_H
