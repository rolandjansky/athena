/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONECONTAINER_H
#define CONECONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include <vector>

#include "HadronicRecoilData/Cone.h"

class ConeContainer : public DataVector<Cone>
{
 public:
  ConeContainer(): DataVector<Cone>() {};
  ~ConeContainer() {};
  
};

CLASS_DEF( ConeContainer , 1171029047 , 1 )

#endif 
