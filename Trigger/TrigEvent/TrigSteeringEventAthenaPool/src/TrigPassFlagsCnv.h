/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigPassFlagsCnv
 *
 * @brief transient persistent converter for TrigPassFlags
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPassFlagsCnv.h 719102 2016-01-21 16:04:25Z will $
 **********************************************************************************/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCNV_H

#include "TrigSteeringEvent/TrigPassFlags.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class TrigPassFlags_p1;

class MsgStream;

// typedef to the latest top level persistent version
// typedef TrigPassFlags_p1  TrigPassFlags_PERS;
typedef TrigPassFlags_p1  TrigPassFlags_PERS;

typedef T_AthenaPoolCustomCnv<TrigPassFlags, TrigPassFlags_PERS > TrigPassFlagsCnvBase; 

class TrigPassFlagsCnv : public TrigPassFlagsCnvBase {

  friend class CnvFactory<TrigPassFlagsCnv>;

 protected:
  TrigPassFlagsCnv(ISvcLocator *svcloc);
  ~TrigPassFlagsCnv();

  virtual TrigPassFlags_PERS  *createPersistent(TrigPassFlags *transObj);
  virtual TrigPassFlags       *createTransient();
};


#endif
