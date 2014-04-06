/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigTauCnv_p3
 *
 * @brief transient persistent converter for TrigTau
 *
 *
 * File and Version Information:
 * $Id: TrigTauCnv_p3.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGTAUCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGTAUCNV_P3_H

#include "TrigParticle/TrigTau.h"
#include "TrigParticleTPCnv/TrigTau_p3.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TrigTauCnv_p3 : public T_AthenaPoolTPCnvBase<TrigTau, TrigTau_p3>
{
 public:

  TrigTauCnv_p3(): m_p4PtEtaPhiMCnv() {}

  virtual void persToTrans(const TrigTau_p3* persObj, 
			   TrigTau* transObj, 
			   MsgStream &log);
  virtual void transToPers(const TrigTau* transObj, 
			   TrigTau_p3* persObj, 
			   MsgStream &log);

 protected:

  P4PtEtaPhiMCnv_p1  m_p4PtEtaPhiMCnv;

};


#endif
