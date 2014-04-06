/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigEFBphysCnv_p3
 *
 * @brief transient-persistent converter for TrigEFBphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysCnv_p3.h,v 1.1 2010-08-12 demelian Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGEFBPHYSCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "TrigParticleTPCnv/TrigEFBphys_p3.h"
#include "TrigParticle/TrigEFBphysContainer.h"


class MsgStream;


class TrigEFBphysCnv_p3 : public T_AthenaPoolTPCnvBase<TrigEFBphys, TrigEFBphys_p3> 
{

 public:

  TrigEFBphysCnv_p3()   {}

  virtual void persToTrans(const TrigEFBphys_p3* persObj, TrigEFBphys* transObj, MsgStream &log);
  virtual void transToPers(const TrigEFBphys* transObj, TrigEFBphys_p3* persObj, MsgStream &log);
  
 protected:
  ElementLinkCnv_p3<ElementLink<TrigEFBphysContainer> > m_elementLinkEFBphysCnv   ;

};


#endif
