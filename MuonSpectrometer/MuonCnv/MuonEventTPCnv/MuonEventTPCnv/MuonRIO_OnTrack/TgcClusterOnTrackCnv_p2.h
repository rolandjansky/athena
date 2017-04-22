/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_CLUSTER_ON_TRACK_CNV_P2_H
#define TGC_CLUSTER_ON_TRACK_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrackCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrack_p2.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

class MsgStream;


class TgcClusterOnTrackCnv_p2
    : 
    public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase,
                                        Muon::TgcClusterOnTrack,
                                        Muon::TgcClusterOnTrack_p2 >
{
public:
    TgcClusterOnTrackCnv_p2() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Muon :: TgcClusterOnTrack_p2 *persObj,
        Muon :: TgcClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: TgcClusterOnTrack    *transObj,
        Muon :: TgcClusterOnTrack_p2 *persObj,
        MsgStream                       &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_TGC_Container >     m_elCnv;     
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool ; 
    LocalParametersCnv_p1     *               m_localParCnv;
    ErrorMatrixCnv_p1         *               m_errorMxCnv;
};

#endif // TGC_CLUSTER_ON_TRACK_CNV_P1_H
