/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PREP_RAW_DATA_CNV_P1_H
#define PREP_RAW_DATA_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   PrepRawDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

class MsgStream;

class PrepRawDataCnv_p1
   : public T_AthenaPoolTPAbstractPolyCnvBase<Trk::PrepRawData, Trk::PrepRawData, Trk::PrepRawData_p1>
{
public:
  PrepRawDataCnv_p1() : m_localPosCnv(0), m_errorMxCnv(0) {}
  
  void persToTrans( const Trk::PrepRawData_p1 *, Trk::PrepRawData *, MsgStream & );
  void transToPers( const Trk::PrepRawData *, Trk::PrepRawData_p1 *, MsgStream & );

private:
  LocalPositionCnv_p1  *m_localPosCnv;
  ErrorMatrixCnv_p1    *m_errorMxCnv;
};

#endif // PREP_RAW_DATA_CNV_P1_H
