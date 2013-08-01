/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECATHENAPOOL_MUONCALOENERGYCONTAINERCNV_H 
#define RECATHENAPOOL_MUONCALOENERGYCONTAINERCNV_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
// #include "RecTPCnv/MuonCaloEnergyContainer_tlp1.h"
#include "RecTPCnv/MuonCaloEnergyContainer_p1.h"

// muonEvent includes
#include "muonEvent/MuonCaloEnergyContainer.h"

// the latest persistent representation type of DataCollection:
typedef MuonCaloEnergyContainer_p1  MuonCaloEnergyContainer_PERS;
//typedef MuonCaloEnergyContainer_tlp1  MuonCaloEnergyContainer_PERS;

class MuonCaloEnergyContainerCnv: public T_AthenaPoolCustomCnv<  MuonCaloEnergyContainer,  MuonCaloEnergyContainer_PERS > {
  friend class CnvFactory<MuonCaloEnergyContainerCnv>;

 protected:

  MuonCaloEnergyContainerCnv(ISvcLocator* svcloc);

  virtual MuonCaloEnergyContainer_PERS* createPersistent( MuonCaloEnergyContainer* transCont );

  virtual MuonCaloEnergyContainer* createTransient();

};


#endif 
