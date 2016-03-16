/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CLUSTER_ON_TRACK_CNV_p2_H
#define SCT_CLUSTER_ON_TRACK_CNV_p2_H

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrack_p2.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;

class SCT_ClusterOnTrackCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase, InDet::SCT_ClusterOnTrack, InDet::SCT_ClusterOnTrack_p2 >{
public:
 SCT_ClusterOnTrackCnv_p2(): m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                             m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const InDet::SCT_ClusterOnTrack_p2 *persObj, InDet::SCT_ClusterOnTrack *transObj, MsgStream &log );
    void transToPers( const InDet::SCT_ClusterOnTrack *transObj, InDet::SCT_ClusterOnTrack_p2 *persObj, MsgStream &log );

protected:
    ToolHandle<Trk::IEventCnvSuperTool>                       m_eventCnvTool;
    ElementLinkCnv_p1< ElementLinkToIDCSCT_ClusterContainer > m_elCnv;     
    LocalParametersCnv_p1*                                    m_localParCnv;
    ErrorMatrixCnv_p1*                                        m_errorMxCnv;
};

#endif 
