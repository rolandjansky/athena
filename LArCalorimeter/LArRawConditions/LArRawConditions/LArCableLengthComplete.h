/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCABLELENGTHCOMPLETE_H
#define LARRAWCONDITIONS_LARCABLELENGTHCOMPLETE_H

#include "LArElecCalib/ILArCableLength.h" 
#include "LArRawConditions/LArCableLengthP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

class LArCablingService ;

/** This class implements the ILArCableLength interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArCableLengthComplete: public ILArCableLength,
	public LArConditionsContainer<LArCableLengthP>{
  
 public: 
  
  typedef LArConditionsContainer<LArCableLengthP> CONTAINER ;
  LArCableLengthComplete();
  
  virtual ~LArCableLengthComplete();
  
  // retrieving CableLength using online ID
  
  virtual const float& CableLength(const HWIdentifier&  CellID) const ;
  
  virtual const float& CableLength(const Identifier&  CellID) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vCableLength);
  
 protected: 
  

};

CLASS_DEF( LArCableLengthComplete,205539859,1)
#endif 
