/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_ON_TRACK_CNV_P1_H
#define PIXEL_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PixelClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrack_p1.h"

#include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrackCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

class MsgStream;


class PixelClusterOnTrackCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
				       InDet::PixelClusterOnTrack,
				       InDet::PixelClusterOnTrack_p1 >
{
public:
 PixelClusterOnTrackCnv_p1() : m_scCnv(0), m_isInitialized(0) {}

  void persToTrans( const InDet :: PixelClusterOnTrack_p1 *persObj,
		    InDet :: PixelClusterOnTrack    *transObj,
		    MsgStream                       &log );
  void transToPers( const InDet :: PixelClusterOnTrack    *transObj,
		    InDet :: PixelClusterOnTrack_p1 *persObj,
		    MsgStream                       &log );
        
protected:
  SiClusterOnTrackCnv_p1* m_scCnv;
  ElementLinkCnv_p1< ElementLinkToIDCPixelClusterContainer >     m_elCnv;     

private:
  const PixelID *m_pixId;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);

};

#endif // PIXEL_CLUSTER_ON_TRACK_CNV_P1_H
