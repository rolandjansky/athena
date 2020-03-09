/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALSCNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALSCNV_P1_H

#include "TrigCaloEvent/TrigT2ZdcSignals.h"
#include "TrigCaloEventTPCnv/TrigT2ZdcSignals_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigT2ZdcSignalsCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigT2ZdcSignals, TrigT2ZdcSignals_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigT2ZdcSignalsCnv_p1() {}
  
  virtual void persToTrans( const TrigT2ZdcSignals_p1 *persObj,
			    TrigT2ZdcSignals    *transObj,
			    MsgStream            &log ) const override;
  virtual void transToPers( const TrigT2ZdcSignals    *transObj,
			    TrigT2ZdcSignals_p1 *persObj,
			    MsgStream            &log ) const override;
  
};


#endif
