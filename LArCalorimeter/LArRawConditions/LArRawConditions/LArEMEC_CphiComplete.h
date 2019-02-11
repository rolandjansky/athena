/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAREMEC_CPHICOMPLETE_H
#define LARRAWCONDITIONS_LAREMEC_CPHICOMPLETE_H

#include "LArElecCalib/ILArEMEC_Cphi.h" 
#include "LArRawConditions/LArEMEC_CphiP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArEMEC_Cphi interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/12/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArEMEC_CphiComplete: public ILArEMEC_Cphi,
	public LArConditionsContainer<LArEMEC_CphiP>{
  
 public: 
  
  typedef LArConditionsContainer<LArEMEC_CphiP> CONTAINER ;
  LArEMEC_CphiComplete();
  
  virtual ~LArEMEC_CphiComplete();
  
  // retrieving EMEC_Cphi using online ID
  
  virtual const float& EMEC_Cphi(const HWIdentifier&  CellID) const ;
  
  static float dummy;

  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vEMEC_Cphi);
  
 protected: 
  

};

CLASS_DEF( LArEMEC_CphiComplete,194446375,1)
#endif 
