/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigT2JetContainerCnv
 *
 * @brief transient persistent converter for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetContainerCnv.h,v 1.2 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGT2JETCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGT2JETCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p3.h"

typedef TrigT2JetContainer_p3   TrigT2JetContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigT2JetContainer, TrigT2JetContainer_PERS> TrigT2JetContainerCnvBase;
 
class TrigT2JetContainerCnv : public TrigT2JetContainerCnvBase {
  
  friend class CnvFactory<TrigT2JetContainerCnv>;
 
  protected:
   
  TrigT2JetContainerCnv( ISvcLocator *svcloc ): TrigT2JetContainerCnvBase(svcloc){}
 
   
   virtual TrigT2JetContainer_PERS *createPersistent( TrigT2JetContainer *transObj);
   virtual TrigT2JetContainer      *createTransient();
 
};//end of class definitions
 
#endif //TRIGT2JETCONTAINER_CNV_H
