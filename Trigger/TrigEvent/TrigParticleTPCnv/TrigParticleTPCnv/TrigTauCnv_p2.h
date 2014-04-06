/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauCnv_p2
 *
 * @brief transient persistent converter for TrigTau
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauCnv_p2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGTAUCNV_P2_H

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p2.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTauCnv_p2 : public T_AthenaPoolTPCnvBase<TrigTau, TrigTau_p2>
{
 public:

  TrigTauCnv_p2() : m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigTau_p2* persObj, 
			   TrigTau* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigTau* transObj, 
			   TrigTau_p2* persObj, 
			   MsgStream &log);

 protected:

  P4PtEtaPhiMCnv_p1*  m_p4PtEtaPhiMCnv;

};


#endif
