/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALMC_H
#define LARRAWCONDITIONS_LARMPHYSOVERMCALMC_H

#include "LArRawConditions/LArMphysOverMcalComplete.h"

#include <vector>

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

  constexpr static float dummy=LArElecCalib::ERRORCODE;
};
//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMphysOverMcalMC, 135003373,1)
CONDCONT_DEF(LArMphysOverMcalMC,46372179,ILArMphysOverMcal);
#endif 
