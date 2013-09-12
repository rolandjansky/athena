/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETCnv_p3
 *
 * @brief transient-persistent converter for TrigMissingET
 *
 * @author Diego Casadei  <Diego.Casadei@cern.ch>  - NYU
 *
 * File and Version Information:
 * $Id: TrigMissingETCnv_p3.h 363695 2011-05-04 14:46:32Z salvator $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_p3_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETCNV_p3_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//TrigMissingET persistent class
#include "TrigMissingEtEventTPCnv/TrigMissingET_p3.h"


class MsgStream;

class TrigMissingETCnv_p3 : public T_AthenaPoolTPCnvBase<TrigMissingET, TrigMissingET_p3> 
{

 public:

  TrigMissingETCnv_p3() {}

  virtual void persToTrans(const TrigMissingET_p3* persObj, TrigMissingET* transObj, MsgStream &log);
  virtual void transToPers(const TrigMissingET* transObj, TrigMissingET_p3* persObj, MsgStream &log);
 
};

#endif
