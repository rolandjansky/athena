/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_CLUSTER_ON_TRACK_CNV_p2_H
#define STGC_CLUSTER_ON_TRACK_CNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   STGC_ClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/STGC_ClusterOnTrack_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
class MsgStream;


class STGC_ClusterOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<     Trk::MeasurementBase,
                                            Muon::sTgcClusterOnTrack,
                                            Muon::STGC_ClusterOnTrack_p1 >
{
public:
    STGC_ClusterOnTrackCnv_p1() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Muon :: STGC_ClusterOnTrack_p1 *persObj,
        Muon :: sTgcClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: sTgcClusterOnTrack    *transObj,
        Muon :: STGC_ClusterOnTrack_p1 *persObj,
        MsgStream                       &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_STGC_Container > m_elCnv;   
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool ; 
    LocalParametersCnv_p1     *               m_localParCnv;
    ErrorMatrixCnv_p1         *               m_errorMxCnv;  
};

#endif // STGC__CLUSTER_ON_TRACK_CNV_p2_H
