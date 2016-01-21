/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigPassBitsCnv
 *
 * @brief transient persistent converter for TrigPassBits
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPassBitsCnv.h 719102 2016-01-21 16:04:25Z will $
 **********************************************************************************/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSBITSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSBITSCNV_H

#include "TrigSteeringEvent/TrigPassBits.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class TrigPassBits_p1;

class MsgStream;

// typedef to the latest top level persistent version
// typedef TrigPassBits_p1  TrigPassBits_PERS;
typedef TrigPassBits_p1  TrigPassBits_PERS;

typedef T_AthenaPoolCustomCnv<TrigPassBits, TrigPassBits_PERS > TrigPassBitsCnvBase; 

class TrigPassBitsCnv : public TrigPassBitsCnvBase {

  friend class CnvFactory<TrigPassBitsCnv>;

 protected:
  TrigPassBitsCnv(ISvcLocator *svcloc);
  ~TrigPassBitsCnv();

  virtual TrigPassBits_PERS  *createPersistent(TrigPassBits *transObj);
  virtual TrigPassBits       *createTransient();
};


#endif
