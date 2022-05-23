/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TrigT2JetContainerCnv
 *
 * @brief transient persistent converter for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGT2JETCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGT2JETCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_p3.h"


typedef TrigT2JetContainer_p3   TrigT2JetContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigT2JetContainer, TrigT2JetContainer_PERS> TrigT2JetContainerCnvBase;
 
class TrigT2JetContainerCnv : public TrigT2JetContainerCnvBase {
  
  friend class CnvFactory<TrigT2JetContainerCnv>;
 
  protected:
   
public:
  TrigT2JetContainerCnv( ISvcLocator *svcloc ): TrigT2JetContainerCnvBase(svcloc){}
protected:
 
   
   virtual TrigT2JetContainer_PERS *createPersistent( TrigT2JetContainer *transObj);
   virtual TrigT2JetContainer      *createTransient();

private:
   TrigT2JetContainerCnv_tlp1   m_converter_tlp1;
   TrigT2JetContainerCnv_p3     m_converter;

};//end of class definitions
 
#endif //TRIGT2JETCONTAINER_CNV_H
