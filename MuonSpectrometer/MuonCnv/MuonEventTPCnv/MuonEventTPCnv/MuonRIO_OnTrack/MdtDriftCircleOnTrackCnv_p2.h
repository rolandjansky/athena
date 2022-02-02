/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DRIFT_CIRCLE_ON_TRACK_CNV_P2_H
#define MDT_DRIFT_CIRCLE_ON_TRACK_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrackCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrack_p2.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

class MsgStream;

class MdtDriftCircleOnTrackCnv_p2
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                                        Muon::MdtDriftCircleOnTrack,
                                        Muon::MdtDriftCircleOnTrack_p2 >
{
public:
    MdtDriftCircleOnTrackCnv_p2() = default;
    void persToTrans( const Muon :: MdtDriftCircleOnTrack_p2 *persObj,
        Muon :: MdtDriftCircleOnTrack    *transObj,
        MsgStream                          &log );
    void transToPers( const Muon :: MdtDriftCircleOnTrack    *transObj,
        Muon :: MdtDriftCircleOnTrack_p2 *persObj,
        MsgStream                          &log );

protected:
    ElementLinkCnv_p3< ElementLinkToIDC_MDT_Container >     m_elCnv; 
    ToolHandle  < Trk::IEventCnvSuperTool >   m_eventCnvTool{"Trk::EventCnvSuperTool/EventCnvSuperTool"}; 
    LocalParametersCnv_p1     *               m_localParCnv{nullptr};
    ErrorMatrixCnv_p1         *               m_errorMxCnv{nullptr};  
};

#endif // DRIFT_CIRCLE_ON_TRACK_CNV_P2_H
