/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonCnv_p2
 *
 * @brief transient-persistent converter for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonCnv_p2.h,v 1.2 2009-04-01 22:13:31 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCNV_P2_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

#include "TrigParticleTPCnv/TrigPhoton_p2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"
#include "TrigParticle/TrigPhoton.h"

class MsgStream;


class TrigPhotonCnv_p2 : public T_AthenaPoolTPCnvBase<TrigPhoton, TrigPhoton_p2> 
{

 public:

  TrigPhotonCnv_p2() : m_p4PtEtaPhiMCnv(0) {}

  virtual void persToTrans(const TrigPhoton_p2* persObj, 
			   TrigPhoton* transObj, 
			   MsgStream &log);

  virtual void transToPers(const TrigPhoton* transObj, 
			   TrigPhoton_p2* persObj, 
			   MsgStream &log);
  
 protected:

  P4PtEtaPhiMCnv_p1      *m_p4PtEtaPhiMCnv ;
};


#endif
