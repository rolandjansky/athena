/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigL2BphysCnv_p3
 *
 * @brief transient-persistent converter for TrigL2Bphys
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysCnv_p3.h,v 1.2 2010-08-11  demelian Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGL2BPHYSCNV_P3_H
#define TRIGPARTICLETPCNV_TRIGL2BPHYSCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TrigParticleTPCnv/TrigL2Bphys_p3.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigParticle/TrigL2BphysContainer.h"

class MsgStream;


class TrigL2BphysCnv_p3 : public T_AthenaPoolTPCnvBase<TrigL2Bphys, TrigL2Bphys_p3> 
{

 public:

  TrigL2BphysCnv_p3()  {}

  virtual void persToTrans(const TrigL2Bphys_p3* persObj, TrigL2Bphys* transObj, MsgStream &log);
  virtual void transToPers(const TrigL2Bphys* transObj, TrigL2Bphys_p3* persObj, MsgStream &log);

  
  protected:

   ElementLinkCnv_p3<ElementLink<TrigL2BphysContainer> > m_elementLinkL2BphysCnv   ;
};


#endif
