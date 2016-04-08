/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetCnv_p1
 *
 * @brief transient-persistent converter for TrigEFBjet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjetCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticleTPCnv/TrigEFBjet_p1.h"
#include "TrigParticle/TrigEFBjet.h"

class MsgStream;


class TrigEFBjetCnv_p1 : public T_AthenaPoolTPCnvBase<TrigEFBjet, TrigEFBjet_p1> 
{

 public:

  TrigEFBjetCnv_p1() : m_EFBjetCnv(0) {}

  virtual void persToTrans(const TrigEFBjet_p1* persObj, TrigEFBjet* transObj, MsgStream &log);
  virtual void transToPers(const TrigEFBjet* transObj, TrigEFBjet_p1* persObj, MsgStream &log);
  
 protected:
  TrigEFBjetCnv_p1   *m_EFBjetCnv   ;
};


#endif
