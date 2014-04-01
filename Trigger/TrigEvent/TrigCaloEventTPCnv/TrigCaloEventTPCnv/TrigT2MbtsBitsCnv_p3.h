/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsCnv_p3
 *
 * @brief transient persistent converter for TrigT2MbtsBits
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCNV_P3_H

#include "TrigCaloEventTPCnv/TrigT2MbtsBits_p3.h"
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigT2MbtsBitsCnv_p3 : public T_AthenaPoolTPCnvBase<TrigT2MbtsBits, TrigT2MbtsBits_p3> {
 public:
  
  TrigT2MbtsBitsCnv_p3() {}
  
  virtual void persToTrans(const TrigT2MbtsBits_p3 *persObj, TrigT2MbtsBits *transObj, MsgStream &log);
  virtual void transToPers(const TrigT2MbtsBits *transObj, TrigT2MbtsBits_p3 *persObj, MsgStream &log);  

};

#endif
