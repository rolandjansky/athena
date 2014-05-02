/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALMC_H
#define LARRAWCONDITIONS_LARMPHYSOVERMCALMC_H

#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

/** Implementation of the interface ILArMphysOverMcal for MC
 *  Derives from LArMphysOverMcalComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 01/07/2005
 *
 */


class LArMphysOverMcalMC: public LArMphysOverMcalComplete {
  
 public: 
  
  LArMphysOverMcalMC();

  virtual StatusCode initialize(); 
  
  virtual ~LArMphysOverMcalMC();
  
  // retrieving MphysOverMcal using online ID
  
  virtual const float& MphysOverMcal(const HWIdentifier&  CellID, int gain=0) const ;
  
  virtual const float& MphysOverMcal(const Identifier&  CellID, int gain=0) const;
  
  
 private: 
  
  // helper for MC z-phi symmetry 
//  LArMCSym m_larmc;
  ToolHandle<ILArMCSymTool> m_larmcsym;

};

CLASS_DEF( LArMphysOverMcalMC, 135003373,1)
#endif 
