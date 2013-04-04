/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_LOLUMRAWDATACNV_P2_H
#define TRT_LOLUMRAWDATACNV_P2_H

/*
Transient/Persistent converter for TRT_LoLumRawData class
Author: Davide Costanzo
*/

#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetEventAthenaPool/InDetRawData_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TRT_LoLumRawDataCnv_p2  : public T_AthenaPoolTPCnvBase<TRT_LoLumRawData, InDetRawData_p2>
{
public:

  TRT_LoLumRawDataCnv_p2() {}
  virtual void          persToTrans(const InDetRawData_p2* persObj, TRT_LoLumRawData* transObj, MsgStream &log);
  virtual void          transToPers(const TRT_LoLumRawData* transObj, InDetRawData_p2* persObj, MsgStream &log);
};


#endif


