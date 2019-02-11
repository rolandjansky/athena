/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_HVBETACOMPLETE_H
#define LARRAWCONDITIONS_LAREMEC_HVBETACOMPLETE_H

#include "LArElecCalib/ILArEMEC_HVbeta.h" 
#include "LArRawConditions/LArEMEC_HVbetaP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArEMEC_HVbeta interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArEMEC_HVbetaComplete: public ILArEMEC_HVbeta,
	public LArConditionsContainer<LArEMEC_HVbetaP>{
  
 public: 
  
  typedef LArConditionsContainer<LArEMEC_HVbetaP> CONTAINER ;
  LArEMEC_HVbetaComplete();
  
  virtual ~LArEMEC_HVbetaComplete();
  
  // retrieving EMEC_HVbeta using online ID
  
  virtual const float& EMEC_HVbeta(const HWIdentifier&  CellID) const ;
  
  static float dummy;

  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vEMEC_HVbeta);
  
 protected: 
  

};

CLASS_DEF( LArEMEC_HVbetaComplete,231293145,1)
#endif 
