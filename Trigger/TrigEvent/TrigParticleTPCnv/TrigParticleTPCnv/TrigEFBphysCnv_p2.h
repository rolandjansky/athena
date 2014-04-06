/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysCnv_p1
 *
 * @brief transient-persistent converter for TrigEFBphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysCnv_p2.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P2_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P2_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

#include "TrigParticleTPCnv/TrigEFBphys_p2.h"

#include "Particle/TrackParticleContainer.h"
#include "TrigParticle/TrigEFBphysContainer.h"


class MsgStream;


class TrigEFBphysCnv_p2 : public T_AthenaPoolTPCnvBase<TrigEFBphys, TrigEFBphys_p2> 
{

 public:

  TrigEFBphysCnv_p2()   {}

  virtual void persToTrans(const TrigEFBphys_p2* persObj, TrigEFBphys* transObj, MsgStream &log);
  virtual void transToPers(const TrigEFBphys* transObj, TrigEFBphys_p2* persObj, MsgStream &log);
  
 protected:
  ElementLinkCnv_p1<ElementLink<TrigEFBphysContainer> > m_elementLinkEFBphysCnv   ;

};


#endif
