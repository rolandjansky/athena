/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_LOLUMRAWDATACNV_P1_H
#define TRT_LOLUMRAWDATACNV_P1_H

/*
Transient/Persistent converter for TRT_LoLumRawData class
Author: Davide Costanzo
*/

#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TRT_LoLumRawDataCnv_p1  : public T_AthenaPoolTPCnvBase<TRT_LoLumRawData, InDetRawData_p1>
{
public:

  TRT_LoLumRawDataCnv_p1() {}
  virtual void          persToTrans(const InDetRawData_p1* persObj, TRT_LoLumRawData* transObj, MsgStream &log);
  virtual void          transToPers(const TRT_LoLumRawData* transObj, InDetRawData_p1* persObj, MsgStream &log);
};


#endif


