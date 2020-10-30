/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LArMinBiasAverageMC_H
#define LARRAWCONDITIONS_LArMinBiasAverageMC_H

#include "LArRawConditions/LArMinBiasAverageComplete.h"

#include <vector>

/** Implementation of the interface ILArMinBiasAverage for MC
 *  Derives from LArMinBiasAverageComplete, and implements the phi-z symmetry
 * History:
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 *
*/

class LArMinBiasAverageMC: public LArMinBiasAverageComplete {
  
 public: 
  
  LArMinBiasAverageMC();
  
  virtual ~LArMinBiasAverageMC( );
  virtual StatusCode initialize(); 
  
  // retrieving Average of E in Minimum Bias events , using online ID
  
  virtual const float& minBiasAverage(const HWIdentifier&  CellID) const ;
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasAverageMC, 255615918, 1)
CONDCONT_DEF( LArMinBiasAverageMC,157938128 , ILArMinBiasAverage );

#endif 
