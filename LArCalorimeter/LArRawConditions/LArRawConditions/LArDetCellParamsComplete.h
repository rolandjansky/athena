/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDETCELLPARAMSCOMPLETE_H
#define LARRAWCONDITIONS_LARDETCELLPARAMSCOMPLETE_H

#include "LArElecCalib/ILArDetCellParams.h" 
#include "LArRawConditions/LArDetCellParamsP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArDetCellParams interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 21/07/05
 *
 * History:
 *  - 
 */

class LArDetCellParamsComplete: public ILArDetCellParams ,
	public LArConditionsContainer<LArDetCellParamsP>{

  
 public: 
  
  typedef LArConditionsContainer<LArDetCellParamsP> CONTAINER ;
  LArDetCellParamsComplete();
  
  virtual ~LArDetCellParamsComplete( );
  
  // retrieving DetCellParams using online ID
  virtual  const float& Omega0(const HWIdentifier&  CellID, int gain) const ;
  virtual  const float& Taur(const HWIdentifier&  CellID, int gain) const ;
  
  // retrieving DetCellParams using offline ID
  virtual  const float& Omega0(const Identifier&  CellID, int gain) const;
  virtual  const float& Taur(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually
  // (if one wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	   float omega0, float taur);
  

};

CLASS_DEF( LArDetCellParamsComplete,191084371,1)
#endif 
