/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASCOMPLETE_H
#define LARRAWCONDITIONS_LARMINBIASCOMPLETE_H

#include "LArElecCalib/ILArMinBias.h" 
#include "LArRawConditions/LArMinBiasP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

class LArCablingService ;

/** This class implements the ILArMinBias interface
 * History:
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 */

class LArMinBiasComplete: public ILArMinBias ,
	public LArConditionsContainer<LArMinBiasP>

{
  
 public: 
  
  typedef LArConditionsContainer<LArMinBiasP> CONTAINER ;

  LArMinBiasComplete();
  
  virtual ~LArMinBiasComplete( );
  
  // retrieving RMS of E in Minimum Bias events, using online ID
  
  virtual const float& minBiasRMS(const HWIdentifier&  CellID) const ;
  
  virtual const float& minBiasRMS(const Identifier&  CellID) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vMinBiasRMS);

 protected: 
  

};


CLASS_DEF( LArMinBiasComplete, 13786367 ,1)
#endif 
