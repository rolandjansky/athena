/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigT2JetCnv_p2
 *
 * @brief transient-persistent converter for TrigT2Jet including jet quality variables
 *
 * @author Tobias Kruker    <kruker@cern.ch>					 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetCnv_p2.h,v 1.0 2010-08-17 $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P2_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P2_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCaloEventTPCnv/TrigT2Jet_p2.h"

class MsgStream;


class TrigT2JetCnv_p2 : public T_AthenaPoolTPCnvConstBase<TrigT2Jet, TrigT2Jet_p2> 
{

 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigT2JetCnv_p2() {}                     

  virtual void persToTrans(const TrigT2Jet_p2* persObj, TrigT2Jet* transObj, MsgStream &log) const override;
  virtual void transToPers(const TrigT2Jet* transObj, TrigT2Jet_p2* persObj, MsgStream &log) const override;
  
};


#endif
