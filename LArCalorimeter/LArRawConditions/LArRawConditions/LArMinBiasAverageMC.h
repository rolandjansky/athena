/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LArMinBiasAverageMC_H
#define LARRAWCONDITIONS_LArMinBiasAverageMC_H

#include "LArRawConditions/LArMinBiasAverageComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

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
  
  virtual const float& minBiasAverage(const Identifier&  CellID) const;

  static float dummy;
  
 private: 
  
  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;

};


CLASS_DEF( LArMinBiasAverageMC, 255615918, 1)
#endif 
