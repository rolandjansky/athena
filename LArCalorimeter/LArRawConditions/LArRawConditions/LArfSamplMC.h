/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLMC_H
#define LARRAWCONDITIONS_LARFSAMPLMC_H

#include "LArRawConditions/LArfSamplComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

/** Implementation of the interface ILArfSampl for MC
 *  Derives from LArfSamplComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */

class LArfSamplMC: public LArfSamplComplete {
  
 public: 
  
  LArfSamplMC();
  
  virtual ~LArfSamplMC();
  virtual StatusCode initialize();
  
  // retrieving fSampl using online ID
  
  virtual const float& FSAMPL(const HWIdentifier&  CellID) const ;
  
  virtual const float& FSAMPL(const Identifier&  CellID) const;

  constexpr static float dummy=LArElecCalib::ERRORCODE; 
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArfSamplMC, 164984905,1)
CONDCONT_DEF( LArfSamplMC, 24657825, ILArfSampl );
#endif 
