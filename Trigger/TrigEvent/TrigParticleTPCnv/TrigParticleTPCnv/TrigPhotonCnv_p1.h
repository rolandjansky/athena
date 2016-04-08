/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonCnv_p1
 *
 * @brief transient-persistent converter for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonCnv_p1.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCNV_P1_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigParticleTPCnv/TrigPhoton_p1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
#include "TrigParticle/TrigPhoton.h"

class MsgStream;


class TrigPhotonCnv_p1 : public T_AthenaPoolTPCnvBase<TrigPhoton, TrigPhoton_p1> 
{

 public:

  TrigPhotonCnv_p1() : m_EMclusterCnv(0),
                       m_P4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigPhoton_p1* persObj, TrigPhoton* transObj, MsgStream &log);
  virtual void transToPers(const TrigPhoton* transObj, TrigPhoton_p1* persObj, MsgStream &log);
  
 protected:
  TrigEMClusterCnv_p1    *m_EMclusterCnv   ;
  P4PtEtaPhiMCnv_p1      *m_P4PtEtaPhiMCnv ;
};


#endif
