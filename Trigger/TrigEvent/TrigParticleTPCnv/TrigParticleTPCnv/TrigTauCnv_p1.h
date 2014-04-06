/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauCnv_p1
 *
 * @brief transient persistent converter for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCnv_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGTAUCNV_P1_H

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTauCnv_p1 : public T_AthenaPoolTPCnvBase<TrigTau, TrigTau_p1>
{
 public:

  TrigTauCnv_p1() : m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigTau_p1* persObj, 
			   TrigTau* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigTau* transObj, 
			   TrigTau_p1* persObj, 
			   MsgStream &log);

 protected:

  P4PtEtaPhiMCnv_p1*  m_p4PtEtaPhiMCnv;

};


#endif
