/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRANSVERSEENERGYCONTAINER_H
#define TRANSVERSEENERGYCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include <vector>

#include "HadronicRecoilData/TransverseEnergy.h"

class TransverseEnergyContainer : public DataVector<TransverseEnergy >
{
 public:
  TransverseEnergyContainer(): DataVector<TransverseEnergy >(){};
  ~TransverseEnergyContainer() {};
  
};

CLASS_DEF( TransverseEnergyContainer , 1113949464 , 1 )

#endif 
