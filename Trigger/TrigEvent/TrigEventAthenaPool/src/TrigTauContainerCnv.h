/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_p3.h"
 #include "TrigParticleTPCnv/TrigTauContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_p3.h"

typedef TrigTauContainer_p3   TrigTauContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigTauContainer, TrigTauContainer_PERS> TrigTauContainerCnvBase;
 
class TrigTauContainerCnv : public TrigTauContainerCnvBase
 {
  
  friend class CnvFactory<TrigTauContainerCnv>;
 
  protected:
   
public:
  TrigTauContainerCnv( ISvcLocator *svcloc ): TrigTauContainerCnvBase(svcloc){}
protected:
 
   
   virtual TrigTauContainer_PERS *createPersistent( TrigTauContainer *transObj);
   virtual TrigTauContainer      *createTransient();

private:
   TrigTauContainerCnv_tlp1 m_converter_tlp1;
   TrigTauContainerCnv_p3   m_converter;

  };//end of class definitions
  
 
 #endif //TRIGTAUCONTAINER_CNV_H
