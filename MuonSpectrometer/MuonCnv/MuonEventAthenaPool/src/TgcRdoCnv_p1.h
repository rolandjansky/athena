/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCNV_P1_H
#define MUON_TGCRDOCNV_P1_H

/*

Transient/Persistent converter for the Collection of TgcRDO,
Author: Marcin Nowak
        CERN, February, 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TgcRawDataCnv_p1.h"
#include "MuonEventAthenaPool/TgcRdo_p1.h"
#include "MuonRDO/TgcRdo.h"

class MsgStream;


typedef  T_AthenaPoolTPCnvVector< TgcRdo, TgcRdo_p1, TgcRawDataCnv_p1 >	TgcRdoCnv_p1_basetype;

class TgcRdoCnv_p1
   : public TgcRdoCnv_p1_basetype
{
public:
  TgcRdoCnv_p1() {}

  virtual void	persToTrans(const TgcRdo_p1* persColl, TgcRdo* transColl, MsgStream &log) ;
  virtual void	transToPers(const TgcRdo* transColl, TgcRdo_p1* persColl, MsgStream &log) ;
};


#endif




