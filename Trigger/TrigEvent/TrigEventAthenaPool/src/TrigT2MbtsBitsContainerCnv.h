/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGT2MBTSBITSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGT2MBTSBITSCONTAINER_CNV_H
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_p3.h"
 
typedef TrigT2MbtsBitsContainer_p3   TrigT2MbtsBitsContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigT2MbtsBitsContainer, TrigT2MbtsBitsContainer_PERS> TrigT2MbtsBitsContainerCnvBase;
 
class TrigT2MbtsBitsContainerCnv : public TrigT2MbtsBitsContainerCnvBase {
  
  friend class CnvFactory<TrigT2MbtsBitsContainerCnv>;
 
  protected:
   
  TrigT2MbtsBitsContainerCnv( ISvcLocator *svcloc ) : TrigT2MbtsBitsContainerCnvBase(svcloc){}
   
   virtual TrigT2MbtsBitsContainer_PERS *createPersistent( TrigT2MbtsBitsContainer *transObj);
   virtual TrigT2MbtsBitsContainer      *createTransient();
  
 };//end of class definitions
  
 
#endif //TRIGT2MBTSBITSCONTAINER_CNV_H
