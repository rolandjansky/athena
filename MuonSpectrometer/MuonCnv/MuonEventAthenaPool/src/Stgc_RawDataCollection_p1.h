/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCNV_P3_H
#define MUON_TGCRDOCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TgcRawDataCnv_p3.h"
#include "MuonEventAthenaPool/TgcRdo_p3.h"
#include "MuonRDO/TgcRdo.h"

class MsgStream;

typedef  T_AthenaPoolTPCnvVector< TgcRdo, TgcRdo_p3, TgcRawDataCnv_p3 >	TgcRdoCnv_p3_basetype;

class TgcRdoCnv_p3
   : public TgcRdoCnv_p3_basetype
{
public:
  TgcRdoCnv_p3() {}

  virtual void	persToTrans(const TgcRdo_p3* persColl, TgcRdo* transColl, MsgStream &log) ;
  virtual void	transToPers(const TgcRdo* transColl, TgcRdo_p3* persColl, MsgStream &log) ;
};

#endif // MUON_TGCRDOCNV_P3_H 

