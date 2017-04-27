/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_CLUSTER_ON_TRACK_CNV_P2_H
#define CSC_CLUSTER_ON_TRACK_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrackCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrack_p2.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

class MsgStream;


class CscClusterOnTrackCnv_p2
    : 
    public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase,
                                        Muon::CscClusterOnTrack,
                                        Muon::CscClusterOnTrack_p2 >
{
public:
    CscClusterOnTrackCnv_p2() : m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans(   const Muon :: CscClusterOnTrack_p2  *persObj,
                        Muon :: CscClusterOnTrack           *transObj,
                        MsgStream                           &log );
    void transToPers(   const Muon :: CscClusterOnTrack     *transObj,
                        Muon :: CscClusterOnTrack_p2        *persObj,
                        MsgStream                           &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_CSC_Container >     m_elCnv;
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool ; 
    LocalParametersCnv_p1     *               m_localParCnv;
    ErrorMatrixCnv_p1         *               m_errorMxCnv;     
};

#endif // CSC_CLUSTER_ON_TRACK_CNV_P1_H
