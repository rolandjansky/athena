/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL1RAWDATACNV_P1_H
#define PIXEL1RAWDATACNV_P1_H

/*
Transient/Persistent converter for Pixel1RawData class
Author: Davide Costanzo
*/

#include "InDetRawData/Pixel1RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

class MsgStream;


class Pixel1RawDataCnv_p1  : public T_AthenaPoolTPCnvBase<Pixel1RawData, InDetRawData_p1>
{
public:

  Pixel1RawDataCnv_p1() : m_isInitialized(0) {}
  virtual void          persToTrans(const InDetRawData_p1* persObj, Pixel1RawData* transObj, MsgStream &log);
  virtual void          transToPers(const Pixel1RawData* transObj, InDetRawData_p1* persObj, MsgStream &log);

private:
  const PixelID *m_pixId = nullptr;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);
};


#endif


