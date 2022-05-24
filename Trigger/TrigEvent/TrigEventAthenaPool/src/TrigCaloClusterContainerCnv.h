/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGCALOCLUSTERCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGCALOCLUSTERCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterContainerCnv_p3.h"
 
typedef TrigCaloClusterContainer_p3   TrigCaloClusterContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigCaloClusterContainer, TrigCaloClusterContainer_PERS> TrigCaloClusterContainerCnvBase;
 
class TrigCaloClusterContainerCnv : public TrigCaloClusterContainerCnvBase
 {
  
  friend class CnvFactory<TrigCaloClusterContainerCnv>;
 
  protected:
   
public:
  TrigCaloClusterContainerCnv( ISvcLocator *svcloc ): TrigCaloClusterContainerCnvBase(svcloc){}
protected:
 
   
   virtual TrigCaloClusterContainer_PERS *createPersistent( TrigCaloClusterContainer *transObj);
   virtual TrigCaloClusterContainer      *createTransient();

private:
   TrigCaloClusterContainerCnv_p3 m_converter;

  };//end of class definitions
  
 
 #endif //TRIGCALOCLUSTERCONTAINER_CNV_H
