/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMPACTINCALOCOLLECTION_H
#define IMPACTINCALOCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TBRec/ImpactInCalo.h"

//class ImpactInCalo;
class ImpactInCaloCollection : public DataVector<ImpactInCalo> {
public:
  virtual ~ImpactInCaloCollection() {};
  
      
  
  void print() const;	
};



CLASS_DEF(ImpactInCaloCollection,1237752222,1)


#endif // IMPACTINCALOCOLLECTION_H
