/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RDOCNV_P1_H
#define MUON_TGCL1RDOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TgcL1RawDataCnv_p1.h"
#include "MuonEventAthenaPool/TgcL1Rdo_p1.h"
#include "MuonRDO/TgcL1Rdo.h"

class MsgStream;

typedef  T_AthenaPoolTPCnvVector< TgcL1Rdo, TgcL1Rdo_p1, TgcL1RawDataCnv_p1 >	TgcL1RdoCnv_p1_basetype;

class TgcL1RdoCnv_p1
   : public TgcL1RdoCnv_p1_basetype
{
public:
  TgcL1RdoCnv_p1() {}

  virtual void	persToTrans(const TgcL1Rdo_p1* persColl, TgcL1Rdo* transColl, MsgStream &log) ;
  virtual void	transToPers(const TgcL1Rdo* transColl, TgcL1Rdo_p1* persColl, MsgStream &log) ;
};

#endif // MUON_TGCL1RDOCNV_P1_H 

