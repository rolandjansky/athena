/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARNOISEMC_H
#define LARRAWCONDITIONS_LARNOISEMC_H

#include "LArRawConditions/LArNoiseComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class ILArMCSymTool;

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
    
 private: 
  
  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArNoiseMC, 264452083, 1)
CONDCONT_DEF( LArNoiseMC, 181236653, ILArNoise );
#endif 
