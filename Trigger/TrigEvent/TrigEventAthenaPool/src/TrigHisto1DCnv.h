/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigHisto1DCnv
 *
 * @brief transient persistent converter for TrigHisto1D
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGHISTO1DCNV_H
#define TRIGEVENTATHENAPOOL_TRIGHISTO1DCNV_H

#include "TrigInDetEvent/TrigHisto1D.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigHisto1D_p1;


class MsgStream;

// typedef to the latest persistent version
typedef TrigHisto1D_p1  TrigHisto1D_PERS;

typedef T_AthenaPoolCustomCnv<TrigHisto1D, TrigHisto1D_PERS > TrigHisto1DCnvBase; 

class TrigHisto1DCnv : public TrigHisto1DCnvBase {
  friend class CnvFactory<TrigHisto1DCnv>;
  
 protected:
  TrigHisto1DCnv(ISvcLocator* svcloc);
  ~TrigHisto1DCnv();
  
  TrigHisto1D_PERS* createPersistent(TrigHisto1D* transObj);
  TrigHisto1D* createTransient();
};

#endif
