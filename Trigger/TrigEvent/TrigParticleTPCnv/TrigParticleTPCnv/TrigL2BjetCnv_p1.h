/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetCnv_p1
 *
 * @brief transient-persistent converter for TrigL2Bjet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BjetCnv_p1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticleTPCnv/TrigL2Bjet_p1.h"
#include "TrigParticle/TrigL2Bjet.h"

class MsgStream;


class TrigL2BjetCnv_p1 : public T_AthenaPoolTPCnvBase<TrigL2Bjet, TrigL2Bjet_p1> 
{

 public:

  TrigL2BjetCnv_p1() : m_L2BjetCnv(0) {}

  virtual void persToTrans(const TrigL2Bjet_p1* persObj, TrigL2Bjet* transObj, MsgStream &log);
  virtual void transToPers(const TrigL2Bjet* transObj, TrigL2Bjet_p1* persObj, MsgStream &log);
  
 protected:
  TrigL2BjetCnv_p1   *m_L2BjetCnv   ;
};


#endif
