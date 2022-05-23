/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPHOTONCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPHOTONCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigPhotonContainerCnv_p3.h"

typedef TrigPhotonContainer_p3   TrigPhotonContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigPhotonContainer, TrigPhotonContainer_PERS> TrigPhotonContainerCnvBase;
 
class TrigPhotonContainerCnv : public TrigPhotonContainerCnvBase {
  
  friend class CnvFactory<TrigPhotonContainerCnv>;
 
  protected:
   
public:
  TrigPhotonContainerCnv( ISvcLocator *svcloc ) : TrigPhotonContainerCnvBase(svcloc){}
protected:
   
   virtual TrigPhotonContainer_PERS *createPersistent( TrigPhotonContainer *transObj);
   virtual TrigPhotonContainer      *createTransient();

private:
   TrigPhotonContainerCnv_tlp1   m_converter_tlp1;
   TrigPhotonContainerCnv_tlp2   m_converter_tlp2;
   TrigPhotonContainerCnv_p3     m_converter;
 
};//end of class definitions
  
 
 #endif //TRIGPHOTONCONTAINER_CNV_H
