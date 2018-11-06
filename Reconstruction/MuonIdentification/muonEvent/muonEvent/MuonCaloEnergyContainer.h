/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_MUONCALOENERGYCONTAINER_H
#define MUONEVENT_MUONCALOENERGYCONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "muonEvent/CaloEnergy.h"

/** @class MuonCaloEnergyContainer
    @brief Data object for a StoreGate container holding a vector of CaloEnergy,
           the detailed energy loss description associated to combined muons
    @author K. A. Assamagan
*/

class MuonCaloEnergyContainer : public DataVector<CaloEnergy> 
{
 public:
  MuonCaloEnergyContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS)
	  : DataVector<CaloEnergy>(own) {}
  virtual ~MuonCaloEnergyContainer() {};

};

CLASS_DEF(MuonCaloEnergyContainer, 1473853273, 1)

#endif





