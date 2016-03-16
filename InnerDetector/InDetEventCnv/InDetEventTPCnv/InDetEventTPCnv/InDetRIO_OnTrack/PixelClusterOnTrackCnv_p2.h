/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_ON_TRACK_CNV_p2_H
#define PIXEL_CLUSTER_ON_TRACK_CNV_p2_H


#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrack_p2.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;


class PixelClusterOnTrackCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase, InDet::PixelClusterOnTrack, InDet::PixelClusterOnTrack_p2 >
{
public:
    PixelClusterOnTrackCnv_p2() :  m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), m_localParCnv(0), m_errorMxCnv(0), m_isInitialized(0) {}

    void persToTrans( const InDet :: PixelClusterOnTrack_p2 *persObj, InDet :: PixelClusterOnTrack    *transObj, MsgStream &log );
    void transToPers( const InDet :: PixelClusterOnTrack    *transObj,InDet :: PixelClusterOnTrack_p2 *persObj, MsgStream  &log );

private:
    ToolHandle<Trk::IEventCnvSuperTool>                            m_eventCnvTool;
    ElementLinkCnv_p1< ElementLinkToIDCPixelClusterContainer >     m_elCnv;  
    LocalParametersCnv_p1*                                         m_localParCnv;
    ErrorMatrixCnv_p1*                                             m_errorMxCnv;
    const PixelID*                                                 m_pixId;
    bool                                                           m_isInitialized;
    StatusCode initialize(MsgStream &log);


   
};

#endif
