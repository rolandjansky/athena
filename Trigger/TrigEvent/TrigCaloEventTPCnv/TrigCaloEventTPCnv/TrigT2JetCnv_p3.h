/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @class  : TrigT2JetCnv_p3
 *
 * @brief transient-persistent converter for TrigT2Jet including jet quality variables
 *
 * @author Tobias Kruker    <kruker@cern.ch>					 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetCnv_p3.h 362053 2011-04-28 12:02:40Z salvator $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_p3.h"

class MsgStream;


class TrigT2JetCnv_p3 : public T_AthenaPoolTPCnvConstBase<TrigT2Jet, TrigT2Jet_p3> 
{

 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigT2JetCnv_p3() {}                     

  virtual void persToTrans(const TrigT2Jet_p3* persObj, TrigT2Jet* transObj, MsgStream &log) const override;
  virtual void transToPers(const TrigT2Jet* transObj, TrigT2Jet_p3* persObj, MsgStream &log) const override;
  
};


#endif
