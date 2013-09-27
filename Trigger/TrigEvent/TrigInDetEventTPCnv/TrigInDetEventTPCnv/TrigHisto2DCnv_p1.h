/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigHisto2DCnv_p1
 *
 * @brief transient persistent converter for the TrigHisto2D class
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO2DCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO2DCNV_P1_H

#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"
#include "TrigInDetEvent/TrigHisto2D.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigHisto2DCnv_p1 : public T_AthenaPoolTPCnvBase<TrigHisto2D, TrigHisto2D_p1> {
 public:
  
  TrigHisto2DCnv_p1() {}
  
  virtual void persToTrans( const TrigHisto2D_p1 *persObj,
			    TrigHisto2D *transObj,
			    MsgStream &log );
  virtual void transToPers( const TrigHisto2D *transObj,
			    TrigHisto2D_p1 *persObj,
			    MsgStream &log );  
};


#endif
