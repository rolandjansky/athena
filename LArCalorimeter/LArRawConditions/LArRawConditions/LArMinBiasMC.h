/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASMC_H
#define LARRAWCONDITIONS_LARMINBIASMC_H

#include "LArRawConditions/LArMinBiasComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

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
  
  virtual const float& minBiasRMS(const Identifier&  CellID) const;

  static float dummy;
  
 private: 
  
  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;

};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasMC, 85803560 ,1)
CONDCONT_DEF( LArMinBiasMC, 133518152, ILArMinBias );
#endif 
