/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFEBENERGYCOLLECTION_H
#define LARFEBENERGYCOLLECTION_H

/**
   @class LArFebEnergyCollection
   @brief Container class for LArFebEnergy

   @author Ignacio Aracena
   
*/


#include "LArRecEvent/LArFebEnergy.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

class LArFebEnergyCollection : public DataVector<LArFebEnergy> {

 public:
  
  /** @brief constructor*/
  LArFebEnergyCollection () 
    {
    };
  
  /** @brief destructor */
  virtual ~LArFebEnergyCollection()
    {
    };
  
};

CLASS_DEF( LArFebEnergyCollection , 1315019678 , 1 )
#endif
