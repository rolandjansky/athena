/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DRIFT_CIRCLE_ON_TRACK_CNV_P1_H
#define MDT_DRIFT_CIRCLE_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MdtDriftCircleOnTrack_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/BoundSurfaceCnv_p1.h"                 
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"

class MsgStream;

class MdtDriftCircleOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                                        Muon::MdtDriftCircleOnTrack,
                                        Muon::MdtDriftCircleOnTrack_p1 >
{
public:
    MdtDriftCircleOnTrackCnv_p1() : m_RIOCnv(0), m_surfCnv(0) {}

    void persToTrans( const Muon :: MdtDriftCircleOnTrack_p1 *persObj,
        Muon :: MdtDriftCircleOnTrack    *transObj,
        MsgStream                          &log );
    void transToPers( const Muon :: MdtDriftCircleOnTrack    *transObj,
        Muon :: MdtDriftCircleOnTrack_p1 *persObj,
        MsgStream                          &log );

protected:
    RIO_OnTrackCnv_p1*              m_RIOCnv;
    StraightLineSurfaceCnv_p1*      m_surfCnv;
    ElementLinkCnv_p1< ElementLinkToIDC_MDT_Container >     m_elCnv;     
};

#endif // DRIFT_CIRCLE_ON_TRACK_CNV_P1_H
