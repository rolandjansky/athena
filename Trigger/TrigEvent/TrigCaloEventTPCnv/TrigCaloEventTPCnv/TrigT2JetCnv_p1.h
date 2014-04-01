/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetCnv_p1
 *
 * @brief transient-persistent converter for TrigT2Jet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"

class MsgStream;


class TrigT2JetCnv_p1 : public T_AthenaPoolTPCnvBase<TrigT2Jet, TrigT2Jet_p1> 
{

 public:

  TrigT2JetCnv_p1() {}                     

  virtual void persToTrans(const TrigT2Jet_p1* persObj, TrigT2Jet* transObj, MsgStream &log);
  virtual void transToPers(const TrigT2Jet* transObj, TrigT2Jet_p1* persObj, MsgStream &log);
  
};


#endif
