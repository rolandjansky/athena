/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigHisto2DCnv
 *
 * @brief transient persistent converter for TrigHisto2D
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGHISTO2DCNV_H
#define TRIGEVENTATHENAPOOL_TRIGHISTO2DCNV_H

#include "TrigInDetEvent/TrigHisto2D.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigHisto2D_p1;

class MsgStream;

// typedef to the latest persistent version
typedef TrigHisto2D_p1  TrigHisto2D_PERS;

typedef T_AthenaPoolCustomCnv<TrigHisto2D, TrigHisto2D_PERS > TrigHisto2DCnvBase; 

class TrigHisto2DCnv : public TrigHisto2DCnvBase {
  friend class CnvFactory<TrigHisto2DCnv>;
  
 protected:
  TrigHisto2DCnv(ISvcLocator* svcloc);
  ~TrigHisto2DCnv();
  
  TrigHisto2D_PERS* createPersistent(TrigHisto2D* transObj);
  TrigHisto2D* createTransient();
};

#endif
