/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_ON_TRACK_CNV_p2_H
#define TRT_DRIFT_CIRCLE_ON_TRACK_CNV_p2_H

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrack_p2.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h" 
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;

class TRT_DriftCircleOnTrackCnv_p2 :  public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase, InDet::TRT_DriftCircleOnTrack, InDet::TRT_DriftCircleOnTrack_p2 > {

public:
    TRT_DriftCircleOnTrackCnv_p2(): m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                                    m_localParCnv(0), m_errorMxCnv(0) {} 

    void persToTrans(   const InDet :: TRT_DriftCircleOnTrack_p2 *persObj, InDet :: TRT_DriftCircleOnTrack    *transObj, MsgStream &log );
    void transToPers(   const InDet :: TRT_DriftCircleOnTrack    *transObj, InDet :: TRT_DriftCircleOnTrack_p2 *persObj, MsgStream &log );

protected:
    ToolHandle<Trk::IEventCnvSuperTool>                            m_eventCnvTool;
    ElementLinkCnv_p1< ElementLinkToIDCTRT_DriftCircleContainer >  m_elCnv;     
    LocalParametersCnv_p1*                                         m_localParCnv;
    ErrorMatrixCnv_p1*                                             m_errorMxCnv;

};

#endif 
