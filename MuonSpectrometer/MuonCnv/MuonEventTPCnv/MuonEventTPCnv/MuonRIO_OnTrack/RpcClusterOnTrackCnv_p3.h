/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CLUSTER_ON_TRACK_CNV_p2_H
#define RPC_CLUSTER_ON_TRACK_CNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p3.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p3.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
class MsgStream;


class RpcClusterOnTrackCnv_p3
    : public T_AthenaPoolTPPolyCnvBase<     Trk::MeasurementBase,
                                            Muon::RpcClusterOnTrack,
                                            Muon::RpcClusterOnTrack_p3 >
{
public:
    RpcClusterOnTrackCnv_p3() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Muon :: RpcClusterOnTrack_p3 *persObj,
        Muon :: RpcClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: RpcClusterOnTrack    *transObj,
        Muon :: RpcClusterOnTrack_p3 *persObj,
        MsgStream                       &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_RPC_Container > m_elCnv;   
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool ; 
    LocalParametersCnv_p1     *               m_localParCnv;
    ErrorMatrixCnv_p1         *               m_errorMxCnv;  
};

#endif // RPC_CLUSTER_ON_TRACK_CNV_p2_H
