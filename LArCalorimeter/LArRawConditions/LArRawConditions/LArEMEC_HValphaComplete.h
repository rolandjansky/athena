/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_HVALPHACOMPLETE_H
#define LARRAWCONDITIONS_LAREMEC_HVALPHACOMPLETE_H

#include "LArElecCalib/ILArEMEC_HValpha.h" 
#include "LArRawConditions/LArEMEC_HValphaP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArEMEC_HValpha interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArEMEC_HValphaComplete: public ILArEMEC_HValpha,
	public LArConditionsContainer<LArEMEC_HValphaP>{
  
 public: 
  
  typedef LArConditionsContainer<LArEMEC_HValphaP> CONTAINER ;
  LArEMEC_HValphaComplete();
  
  virtual ~LArEMEC_HValphaComplete();
  
  // retrieving EMEC_HValpha using online ID
  
  virtual const float& EMEC_HValpha(const HWIdentifier&  CellID) const ;
  
  static float dummy;

  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vEMEC_HValpha);
  
 protected: 
  

};

CLASS_DEF( LArEMEC_HValphaComplete,89432706,1)
#endif 
