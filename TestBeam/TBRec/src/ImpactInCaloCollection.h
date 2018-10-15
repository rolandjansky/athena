/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_IMPACTINCALOCOLLECTION_H
#define TBREC_IMPACTINCALOCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "ImpactInCalo.h"

//class ImpactInCalo;
class ImpactInCaloCollection : public DataVector<ImpactInCalo> {
public:
  virtual ~ImpactInCaloCollection() {};
  
      
  
  void print() const;	
};



CLASS_DEF(ImpactInCaloCollection,1237752222,1)


#endif // IMPACTINCALOCOLLECTION_H
