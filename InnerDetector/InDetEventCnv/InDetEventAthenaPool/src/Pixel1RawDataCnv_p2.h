/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL1RAWDATACNV_P2_H
#define PIXEL1RAWDATACNV_P2_H

/*
Transient/Persistent converter for Pixel1RawData class
Author: Davide Costanzo
*/

#include "InDetRawData/Pixel1RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class Pixel1RawDataCnv_p2  : public T_AthenaPoolTPCnvBase<Pixel1RawData, InDetRawData_p2>
{
public:

  Pixel1RawDataCnv_p2() {}
  virtual void          persToTrans(const InDetRawData_p2* persObj, Pixel1RawData* transObj, MsgStream &log);
  virtual void          transToPers(const Pixel1RawData* transObj, InDetRawData_p2* persObj, MsgStream &log);
};


#endif


