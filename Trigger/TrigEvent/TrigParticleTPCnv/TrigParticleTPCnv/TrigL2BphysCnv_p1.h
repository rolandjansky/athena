/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysCnv_p1
 *
 * @brief transient-persistent converter for TrigL2Bphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYSCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYSCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticleTPCnv/TrigL2Bphys_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCnv_p1.h"
#include "TrigParticle/TrigL2Bphys.h"

class MsgStream;


class TrigL2BphysCnv_p1 : public T_AthenaPoolTPCnvBase<TrigL2Bphys, TrigL2Bphys_p1> 
{

 public:

  TrigL2BphysCnv_p1() : m_VertexCnv(0),
                        m_L2BphysCnv(0) {}

  virtual void persToTrans(const TrigL2Bphys_p1* persObj, TrigL2Bphys* transObj, MsgStream &log);
  virtual void transToPers(const TrigL2Bphys* transObj, TrigL2Bphys_p1* persObj, MsgStream &log);
  virtual TPObjRef virt_toPersistent( const TrigL2Bphys *trans, MsgStream &log );

 protected:
  TrigVertexCnv_p1    *m_VertexCnv    ;
  TrigL2BphysCnv_p1   *m_L2BphysCnv   ;
};


#endif
