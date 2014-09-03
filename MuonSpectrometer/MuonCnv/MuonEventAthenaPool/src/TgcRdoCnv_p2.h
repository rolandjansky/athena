/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCNV_P2_H
#define MUON_TGCRDOCNV_P2_H

/*

Transient/Persistent converter for the Collection of TgcRDO,
Author: Zvi Tarem
        CERN, Augist, 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TgcRawDataCnv_p2.h"
#include "MuonEventAthenaPool/TgcRdo_p2.h"
#include "MuonRDO/TgcRdo.h"

class MsgStream;

typedef  T_AthenaPoolTPCnvVector< TgcRdo, TgcRdo_p2, TgcRawDataCnv_p2 >	TgcRdoCnv_p2_basetype;

class TgcRdoCnv_p2
   : public TgcRdoCnv_p2_basetype
{
public:
  TgcRdoCnv_p2() {}

  virtual void	persToTrans(const TgcRdo_p2* persColl, TgcRdo* transColl, MsgStream &log) ;
  virtual void	transToPers(const TgcRdo* transColl, TgcRdo_p2* persColl, MsgStream &log) ;
};

#endif // MUON_TGCRDOCNV_P2_H 

