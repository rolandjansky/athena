/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETCnv_p1
 *
 * @brief transient-persistent converter for TrigMissingET
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETCnv_p1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_P1_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigMissingET persistent class
#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"


class MsgStream;


class TrigMissingETCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMissingET, TrigMissingET_p1> 
{

 public:

  TrigMissingETCnv_p1() {}

  virtual void persToTrans(const TrigMissingET_p1* persObj, TrigMissingET* transObj, MsgStream &log);
  virtual void transToPers(const TrigMissingET* transObj, TrigMissingET_p1* persObj, MsgStream &log);
 
};

#endif
