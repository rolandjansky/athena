/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYLISTCONTAINER_H
#define ENERGYLISTCONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include <vector>

#include "HadronicRecoilData/EnergyList.h"
#include "CaloEvent/CaloCluster.h"

template <class T>class EnergyListContainer : public DataVector< EnergyList<T> >
{
 public:
  EnergyListContainer(): DataVector< EnergyList<T> >() {};
  ~EnergyListContainer() {};
  
};

CLASS_DEF( EnergyListContainer<CaloCluster> ,   1249518825 , 1 )

#endif 
