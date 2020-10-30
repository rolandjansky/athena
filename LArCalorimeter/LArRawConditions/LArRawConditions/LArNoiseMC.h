/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARNOISEMC_H
#define LARRAWCONDITIONS_LARNOISEMC_H

#include "LArRawConditions/LArNoiseComplete.h"

#include <vector>


/** Implementation of the interface ILArNoise for MC
 *  Derives from LArNoiseComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 */

class LArNoiseMC: public LArNoiseComplete {
  
 public: 
  
  LArNoiseMC();
  
  virtual ~LArNoiseMC( );

  virtual StatusCode initialize ( );
  
  // retrieving Noise using online ID  
  virtual  const float& noise(const HWIdentifier&  CellID, int gain) const ;
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArNoiseMC, 264452083, 1)
CONDCONT_DEF( LArNoiseMC, 181236653, ILArNoise );
#endif 
