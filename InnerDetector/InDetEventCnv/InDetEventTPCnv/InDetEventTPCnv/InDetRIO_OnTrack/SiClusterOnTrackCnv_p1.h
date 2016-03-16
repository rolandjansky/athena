/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_CLUSTER_ON_TRACK_CNV_P1_H
#define SI_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   SiClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrack_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"

class MsgStream;

class SiClusterOnTrackCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase,
					       InDet::SiClusterOnTrack,
					       InDet::SiClusterOnTrack_p1 >
{
public:
  SiClusterOnTrackCnv_p1() : m_RIOCnv(0) {}
  
  void persToTrans( const InDet::SiClusterOnTrack_p1 *persObj,
		    InDet::SiClusterOnTrack    *transObj,
		    MsgStream                  &log );
  void transToPers( const InDet::SiClusterOnTrack    *transObj,
		    InDet::SiClusterOnTrack_p1 *persObj,
		    MsgStream                  &log );

  
protected:
  RIO_OnTrackCnv_p1	*m_RIOCnv;
};

#endif // SI_CLUSTER_ON_TRACK_CNV_P1_H
