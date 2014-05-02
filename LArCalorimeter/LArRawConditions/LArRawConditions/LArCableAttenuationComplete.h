/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCABLEATTENUATIONCOMPLETE_H
#define LARRAWCONDITIONS_LARCABLEATTENUATIONCOMPLETE_H

#include "LArElecCalib/ILArCableAttenuation.h" 
#include "LArRawConditions/LArCableAttenuationP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArCableAttenuation interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArCableAttenuationComplete: public ILArCableAttenuation,
	public LArConditionsContainer<LArCableAttenuationP>{
  
 public: 
  
  typedef LArConditionsContainer<LArCableAttenuationP> CONTAINER ;
  LArCableAttenuationComplete();
  
  virtual ~LArCableAttenuationComplete();
  
  // retrieving CableAttenuation using online ID
  
  virtual const float& CableAttenuation(const HWIdentifier&  CellID) const ;
  
  virtual const float& CableAttenuation(const Identifier&  CellID) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vCableAttenuation);
  
 protected: 
  

};

CLASS_DEF( LArCableAttenuationComplete,227617954,1)
#endif 
