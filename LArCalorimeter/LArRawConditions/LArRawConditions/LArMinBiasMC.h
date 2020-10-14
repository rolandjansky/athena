/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASMC_H
#define LARRAWCONDITIONS_LARMINBIASMC_H

#include "LArRawConditions/LArMinBiasComplete.h"

#include <vector>

/** Implementation of the interface ILArMinBias for MC
 *  Derives from LArMinBiasComplete, and implements the phi-z symmetry
 * History:
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 *
*/

class LArMinBiasMC: public LArMinBiasComplete {
  
 public: 
  
  LArMinBiasMC();
  
  virtual ~LArMinBiasMC( );
  virtual StatusCode initialize(); 
  
  // retrieving RMS of E in Minimum Bias events , using online ID
  
  virtual const float& minBiasRMS(const HWIdentifier&  CellID) const ;
};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasMC, 85803560 ,1)
CONDCONT_DEF( LArMinBiasMC, 133518152, ILArMinBias );
#endif 
