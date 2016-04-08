/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysCnv_p1
 *
 * @brief transient-persistent converter for TrigEFBphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticleTPCnv/TrigEFBphys_p1.h"
#include "TrigParticle/TrigEFBphys.h"

class MsgStream;


class TrigEFBphysCnv_p1 : public T_AthenaPoolTPCnvBase<TrigEFBphys, TrigEFBphys_p1> 
{

 public:

  TrigEFBphysCnv_p1() : m_EFBphysCnv(0) {}

  virtual void persToTrans(const TrigEFBphys_p1* persObj, TrigEFBphys* transObj, MsgStream &log);
  virtual void transToPers(const TrigEFBphys* transObj, TrigEFBphys_p1* persObj, MsgStream &log);
  virtual TPObjRef virt_toPersistent( const TrigEFBphys *trans, MsgStream &log );
  
 protected:
  TrigEFBphysCnv_p1   *m_EFBphysCnv   ;
};


#endif
