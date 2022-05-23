/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGRNNOUTPUTCONTAINERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGRNNOUTPUTCONTAINERCNV_H

#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainer_p2.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_p2.h"

typedef TrigRNNOutputContainer_p2   TrigRNNOutputContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigRNNOutputContainer, TrigRNNOutputContainer_PERS> TrigRNNOutputContainerCnvBase;

class TrigRNNOutputContainerCnv : public TrigRNNOutputContainerCnvBase {
  
  friend class CnvFactory<TrigRNNOutputContainerCnv>;
 
  protected:
   
public:
  TrigRNNOutputContainerCnv( ISvcLocator *svcloc ) : TrigRNNOutputContainerCnvBase(svcloc){}
protected:
   
   virtual TrigRNNOutputContainer_PERS *createPersistent( TrigRNNOutputContainer *transObj);
   virtual TrigRNNOutputContainer      *createTransient();

private:
   TrigRNNOutputContainerCnv_tlp1   m_converter_tlp1;
   TrigRNNOutputContainerCnv_p2     m_converter;
 
 };//end of class definitions
  
 
#endif //TRIGRNNOUTPUTCONTAINERCNV_H
