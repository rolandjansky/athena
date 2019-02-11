/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARTSHAPERCOMPLETE_H
#define LARRAWCONDITIONS_LARTSHAPERCOMPLETE_H

#include "LArElecCalib/ILArTshaper.h" 
#include "LArRawConditions/LArTshaperP.h"
#include "LArRawConditions/LArConditionsContainer.h"

/** This class implements the ILArTshaper interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArTshaperComplete: public ILArTshaper,
	public LArConditionsContainer<LArTshaperP>{
  
 public: 
  
  typedef LArConditionsContainer<LArTshaperP> CONTAINER ;
  LArTshaperComplete();
  
  virtual ~LArTshaperComplete();
  
  // retrieving Tshaper using online ID
  
  virtual const float& Tshaper(const HWIdentifier&  CellID) const ;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vTshaper);
  
 protected: 
  

};

CLASS_DEF( LArTshaperComplete,175811031,1)
#endif 
