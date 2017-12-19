/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPMC_H
#define LARRAWCONDITIONS_LARRAMPMC_H

#include "LArRawConditions/LArRampComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

/** Implementation of the interface ILArRamp for MC
 *  Derives from LArRampComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 03/02/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */

class LArRampMC: public LArRampComplete {
  
 public: 

  typedef LArRampComplete::CONTAINER  CONTAINER; 
  
  LArRampMC();
  
  virtual ~LArRampMC( );
  
  virtual StatusCode initialize () ; 

  // retrieving Ramp using online ID
  
  virtual  RampRef_t ADC2DAC(const HWIdentifier&  CellID, int gain) const ;
  
  virtual  RampRef_t ADC2DAC(const Identifier&  CellID, int gain) const;
  
 private: 

  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;

};

//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRampMC, 42062668,1)
CONDCONT_DEF(  LArRampMC, 104173262 );

#endif 
