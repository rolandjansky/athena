/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto1DCnv_p1
 *
 * @brief transient persistent converter for the TrigHisto1D class
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO1DCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO1DCNV_P1_H

#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"
#include "TrigInDetEvent/TrigHisto1D.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigHisto1DCnv_p1 : public T_AthenaPoolTPCnvBase<TrigHisto1D, TrigHisto1D_p1> {
 public:
  
  TrigHisto1DCnv_p1() {}
  
  virtual void persToTrans( const TrigHisto1D_p1 *persObj,
			    TrigHisto1D *transObj,
			    MsgStream &log );
  virtual void transToPers( const TrigHisto1D *transObj,
			    TrigHisto1D_p1 *persObj,
			    MsgStream &log );  
};

#endif
