/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCNV_P4_H
#define MUON_TGCRDOCNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TgcRawDataCnv_p4.h"
#include "MuonEventAthenaPool/TgcRdo_p4.h"
#include "MuonRDO/TgcRdo.h"

class MsgStream;

typedef  T_AthenaPoolTPCnvVector< TgcRdo, TgcRdo_p4, TgcRawDataCnv_p4 >	TgcRdoCnv_p4_basetype;

class TgcRdoCnv_p4
   : public TgcRdoCnv_p4_basetype
{
public:
  TgcRdoCnv_p4() {}

  virtual void	persToTrans(const TgcRdo_p4* persColl, TgcRdo* transColl, MsgStream &log) ;
  virtual void	transToPers(const TgcRdo* transColl, TgcRdo_p4* persColl, MsgStream &log) ;
};

#endif // MUON_TGCRDOCNV_P4_H 

