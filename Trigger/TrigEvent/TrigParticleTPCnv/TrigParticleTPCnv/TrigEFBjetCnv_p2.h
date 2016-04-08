/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBjetCnv_p2
 *
 * @brief transient-persistent converter for TrigEFBjet
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGEFBJETCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBJETCNV_P2_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
#include "TrigParticleTPCnv/TrigEFBjet_p2.h"
#include "TrigParticle/TrigEFBjet.h"

class MsgStream;


class TrigEFBjetCnv_p2 : public T_AthenaPoolTPCnvBase<TrigEFBjet, TrigEFBjet_p2> {

 public:

  TrigEFBjetCnv_p2() : m_EFBjetCnv(0), m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigEFBjet_p2* persObj, TrigEFBjet* transObj, MsgStream &log);
  virtual void transToPers(const TrigEFBjet* transObj, TrigEFBjet_p2* persObj, MsgStream &log);
  
 protected:

  TrigEFBjetCnv_p2*   m_EFBjetCnv;
  P4PtEtaPhiMCnv_p1*  m_p4PtEtaPhiMCnv;

};

#endif
