/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RIO_ON_TRACK_CNV_P1_H
#define RIO_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RIO_OnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrack_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

class MsgStream;

class RIO_OnTrackCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase, Trk::RIO_OnTrack, Trk::RIO_OnTrack_p1 >
{
public:
 RIO_OnTrackCnv_p1() : m_localParCnv(0), m_errorMxCnv(0), m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), m_pixId(0), m_storeGate(0), m_isInitialized(0) {}
  virtual void persToTrans( const Trk::RIO_OnTrack_p1 *persObj, Trk::RIO_OnTrack *transObj, MsgStream &log );
  virtual void transToPers( const Trk::RIO_OnTrack *transObj, Trk::RIO_OnTrack_p1 *persObj, MsgStream &log );
  
  //void initialize();
  
private:
  LocalParametersCnv_p1     *               m_localParCnv;
  ErrorMatrixCnv_p1         *               m_errorMxCnv;
  ToolHandle<Trk::IEventCnvSuperTool>       m_eventCnvTool;
  const PixelID *m_pixId;
  StoreGateSvc *m_storeGate;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);


};

#endif // RIO_ON_TRACK_CNV_P1_H
