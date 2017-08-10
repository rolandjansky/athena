/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_CLUSTER_ON_TRACK_CNV_p2_H
#define MM_CLUSTER_ON_TRACK_CNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   MM_ClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
class MsgStream;


class MM_ClusterOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<     Trk::MeasurementBase,
                                            Muon::MMClusterOnTrack,
                                            Muon::MM_ClusterOnTrack_p1 >
{
public:
    MM_ClusterOnTrackCnv_p1() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Muon :: MM_ClusterOnTrack_p1 *persObj,
        Muon :: MMClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: MMClusterOnTrack    *transObj,
        Muon :: MM_ClusterOnTrack_p1 *persObj,
        MsgStream                       &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_MM_Container > m_elCnv;   
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool ; 
    LocalParametersCnv_p1     *               m_localParCnv;
    ErrorMatrixCnv_p1         *               m_errorMxCnv;  
};

#endif // MM__CLUSTER_ON_TRACK_CNV_p2_H
