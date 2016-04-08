/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BjetCnv_p3
 *
 * @brief transient-persistent converter for TrigL2Bjet
 *
 * @author Andrea Coccaro  <Andrea.Coccaro@ge.infn.it>
 *
 **********************************************************************************/

#ifndef TRIGPARTICLETPCNV_TRIGL2BJETCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGL2BJETCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
#include "TrigParticleTPCnv/TrigL2Bjet_p3.h"
#include "TrigParticle/TrigL2Bjet.h"

class MsgStream;


class TrigL2BjetCnv_p3 : public T_AthenaPoolTPCnvBase<TrigL2Bjet, TrigL2Bjet_p3> {

 public:
  TrigL2BjetCnv_p3() : m_L2BjetCnv(0) {}

  virtual void persToTrans(const TrigL2Bjet_p3* persObj, TrigL2Bjet* transObj, MsgStream &log);
  virtual void transToPers(const TrigL2Bjet* transObj, TrigL2Bjet_p3* persObj, MsgStream &log);
  
 protected:

  TrigL2BjetCnv_p3*   m_L2BjetCnv;
  P4PtEtaPhiMCnv_p1  m_p4PtEtaPhiMCnv;


};

#endif
