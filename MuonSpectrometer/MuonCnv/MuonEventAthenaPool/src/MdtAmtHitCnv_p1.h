/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTATHENAPOOL_MDTAMTHITCNV_P1_H
#define MUONEVENTATHENAPOOL_MDTAMTHITCNV_P1_H

/*
Transient/Persistent converter for MdtAmtHit class
@author edward.moyse@cern.ch
*/

#include "MuonEventAthenaPool/MdtAmtHit_p1.h"
#include "MuonRDO/MdtAmtHit.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  

class MsgStream;

class MdtAmtHitCnv_p1  : public T_AthenaPoolTPCnvBase<MdtAmtHit, MdtAmtHit_p1>
{
public:
  MdtAmtHitCnv_p1() {}
  
  virtual void		persToTrans(const MdtAmtHit_p1* persObj, MdtAmtHit* transObj, MsgStream &log);
  virtual void		transToPers(const MdtAmtHit* transObj, MdtAmtHit_p1* persObj, MsgStream &log);
};


#endif

