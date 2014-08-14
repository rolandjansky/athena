/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigT2MbtsBitsCnv
 *
 * @brief transient persistent converter for TrigT2MbtsBits
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGT2MBTSBITSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGT2MBTSBITSCNV_H

#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

class MsgStream;

class TrigT2MbtsBits_p2;
// typedef to the latest persistent version

typedef TrigT2MbtsBits_p2 TrigT2MbtsBits_PERS;
typedef T_AthenaPoolCustomCnv<TrigT2MbtsBits, TrigT2MbtsBits_PERS > TrigT2MbtsBitsCnvBase; 

class TrigT2MbtsBitsCnv : public TrigT2MbtsBitsCnvBase {
  friend class CnvFactory<TrigT2MbtsBitsCnv>;
  
 protected:
  TrigT2MbtsBitsCnv(ISvcLocator* svcloc);
  ~TrigT2MbtsBitsCnv();
  
  TrigT2MbtsBits_PERS* createPersistent(TrigT2MbtsBits* transObj);
  TrigT2MbtsBits* createTransient();
};


#endif
