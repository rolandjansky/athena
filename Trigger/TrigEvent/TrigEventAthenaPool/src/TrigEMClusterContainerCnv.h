/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGEMCLUSTERCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGEMCLUSTERCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p4.h"
 
typedef TrigEMClusterContainer_p4   TrigEMClusterContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigEMClusterContainer, TrigEMClusterContainer_PERS> TrigEMClusterContainerCnvBase;
 
class TrigEMClusterContainerCnv : public TrigEMClusterContainerCnvBase
 {
  
  friend class CnvFactory<TrigEMClusterContainerCnv>;
 
  protected:
   
  TrigEMClusterContainerCnv( ISvcLocator *svcloc ): TrigEMClusterContainerCnvBase(svcloc){}
 
   
   virtual TrigEMClusterContainer_PERS *createPersistent( TrigEMClusterContainer *transObj);
   virtual TrigEMClusterContainer      *createTransient();
 
  };//end of class definitions
  
 
 #endif //TRIGEMCLUSTERCONTAINER_CNV_H
