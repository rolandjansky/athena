/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_ELECTRONMUONTOPOINFO_CNV_H
#define TRIGEVENTATHENAPOOL_ELECTRONMUONTOPOINFO_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoContainerCnv_p1.h"
 
typedef ElectronMuonTopoInfoContainer_p1   ElectronMuonTopoInfoContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<ElectronMuonTopoInfoContainer, ElectronMuonTopoInfoContainer_PERS> ElectronMuonTopoInfoContainerCnvBase;
 
class ElectronMuonTopoInfoContainerCnv : public ElectronMuonTopoInfoContainerCnvBase
 {
  
  friend class CnvFactory<ElectronMuonTopoInfoContainerCnv>;
 
  protected:
   
  ElectronMuonTopoInfoContainerCnv( ISvcLocator *svcloc ): ElectronMuonTopoInfoContainerCnvBase(svcloc){}
 
   
   virtual ElectronMuonTopoInfoContainer_PERS *createPersistent( ElectronMuonTopoInfoContainer *transObj);
   virtual ElectronMuonTopoInfoContainer      *createTransient();
 
  };//end of class definitions
  
 
 #endif //TRIGEMCLUSTERCONTAINER_CNV_H
