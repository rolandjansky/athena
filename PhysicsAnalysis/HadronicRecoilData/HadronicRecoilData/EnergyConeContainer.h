/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ENERGYCONECONTAINER_H
#define ENERGYCONECONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include <vector>

#include "HadronicRecoilData/EnergyCone.h"
#include "CaloEvent/CaloCluster.h"
#include "Particle/TrackParticle.h"

template <class T> class EnergyConeContainer : public DataVector<EnergyCone<T> >
{
 public:
  EnergyConeContainer(): DataVector<EnergyCone<T> >(){};
  ~EnergyConeContainer() {};
  
};

CLASS_DEF( EnergyConeContainer<CaloCluster>   , 1103949464 , 1 )
CLASS_DEF( EnergyConeContainer<Rec::TrackParticle> , 1103949465 , 1 )

#endif 
