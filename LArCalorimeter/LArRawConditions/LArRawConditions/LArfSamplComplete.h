/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLCOMPLETE_H
#define LARRAWCONDITIONS_LARFSAMPLCOMPLETE_H

#include "LArElecCalib/ILArfSampl.h" 
#include "LArRawConditions/LArfSamplP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArfSampl interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 21/01/04
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID *
 */

 /*   - July 16, 2005  Hong Ma
 *      use LArfSamplP. 

 */



class LArfSamplComplete: public ILArfSampl ,
	public LArConditionsContainer<LArfSamplP>
{
  
 public: 
  
  typedef LArConditionsContainer<LArfSamplP> CONTAINER ; 
  LArfSamplComplete();
  
  virtual ~LArfSamplComplete();
  
  // retrieving fSampl using online ID
  
  virtual const float& FSAMPL(const HWIdentifier&  CellID) const ;
  
  virtual const float& FSAMPL(const Identifier&  CellID) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vfSampl);
  
 protected: 
  

};

CLASS_DEF( LArfSamplComplete, 250517982,1)
#endif 
