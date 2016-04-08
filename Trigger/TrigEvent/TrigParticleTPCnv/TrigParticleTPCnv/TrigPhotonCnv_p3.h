/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigPhotonCnv_p3
 *
 * @brief transient-persistent converter for TrigPhoton
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigPhotonCnv_p3.h 724397 2016-02-16 22:17:22Z ssnyder $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGPHOTONCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGPHOTONCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TrigParticleTPCnv/TrigPhoton_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"
#include "TrigParticle/TrigPhoton.h"

class MsgStream;


class TrigPhotonCnv_p3 : public T_AthenaPoolTPCnvBase<TrigPhoton, TrigPhoton_p3> 
{

 public:

  TrigPhotonCnv_p3() {}

  virtual void persToTrans(const TrigPhoton_p3* persObj, 
			   TrigPhoton* transObj, 
			   MsgStream &log);

  virtual void transToPers(const TrigPhoton* transObj, 
			   TrigPhoton_p3* persObj, 
			   MsgStream &log);
  
 protected:

  P4PtEtaPhiMCnv_p1 m_p4PtEtaPhiMCnv ;
};


#endif
