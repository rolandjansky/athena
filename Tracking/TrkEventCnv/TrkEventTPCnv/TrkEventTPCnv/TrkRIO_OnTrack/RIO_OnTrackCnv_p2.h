/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RIO_ON_TRACK_CNV_p2_H
#define RIO_ON_TRACK_CNV_p2_H


#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;

class RIO_OnTrackCnv_p2
   : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase, Trk::RIO_OnTrack, Trk::RIO_OnTrack_p2 >
{
public:
  RIO_OnTrackCnv_p2() : m_localParCnv(0), m_errorMxCnv(0), m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool")  {}
  virtual void persToTrans( const Trk::RIO_OnTrack_p2 *persObj, Trk::RIO_OnTrack *transObj, MsgStream &log );
  virtual void transToPers( const Trk::RIO_OnTrack *transObj, Trk::RIO_OnTrack_p2 *persObj, MsgStream &log );
  
  //void initialize();
  
private:
  LocalParametersCnv_p1     *               m_localParCnv;
  ErrorMatrixCnv_p1         *               m_errorMxCnv;
  ToolHandle<Trk::IEventCnvSuperTool>       m_eventCnvTool;
};

#endif 
