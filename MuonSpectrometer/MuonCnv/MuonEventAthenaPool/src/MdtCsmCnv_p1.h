/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MDTCSMCNV_P1_H
#define MUONEVENTATHENAPOOL_MDTCSMCNV_P1_H

/*
Transient/Persistent converter for the Collection of MdtAmtHits
 @author edward.moyse@cern.ch
*/
#include "MuonRDO/MdtCsm.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonEventAthenaPool/MdtCsm_p1.h"
#include "MdtAmtHitCnv_p1.h"


typedef  T_AthenaPoolTPCnvVector< MdtCsm, MdtCsm_p1, MdtAmtHitCnv_p1 >	MdtCsm_Cnvp1_base_t;

class MdtCsmCnv_p1
   : public MdtCsm_Cnvp1_base_t
{
public:
  MdtCsmCnv_p1() {}

  virtual void	persToTrans(const MdtCsm_p1* persColl, MdtCsm* transColl, MsgStream &log) ;
  virtual void	transToPers(const MdtCsm* transColl, MdtCsm_p1* persColl, MsgStream &log) ;
};


#endif




