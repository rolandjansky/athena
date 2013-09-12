/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETCnv_p2
 *
 * @brief transient-persistent converter for TrigMissingET
 *
 * @author Diego Casadei  <Diego.Casadei@cern.ch>  - NYU
 *
 * File and Version Information:
 * $Id: TrigMissingETCnv_p2.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_p2_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_p2_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigMissingET persistent class
#include "TrigMissingEtEventTPCnv/TrigMissingET_p2.h"


class MsgStream;


class TrigMissingETCnv_p2 : public T_AthenaPoolTPCnvBase<TrigMissingET, TrigMissingET_p2> 
{

 public:

  TrigMissingETCnv_p2() {}

  virtual void persToTrans(const TrigMissingET_p2* persObj, TrigMissingET* transObj, MsgStream &log);
  virtual void transToPers(const TrigMissingET* transObj, TrigMissingET_p2* persObj, MsgStream &log);
 
};


#endif
