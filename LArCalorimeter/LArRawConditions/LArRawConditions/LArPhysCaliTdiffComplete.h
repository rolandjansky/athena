/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARPHYSCALITDIFFCOMPLETE_H
#define LARRAWCONDITIONS_LARPHYSCALITDIFFCOMPLETE_H

#include "LArElecCalib/ILArPhysCaliTdiff.h" 
#include "LArRawConditions/LArPhysCaliTdiffP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

class LArCablingService ;

/** This class implements the ILArPhysCaliTdiff interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 21/07/05
 *
 * History:
 *  - 
 */

class LArPhysCaliTdiffComplete: public ILArPhysCaliTdiff,
	public LArConditionsContainer<LArPhysCaliTdiffP>{

 public: 
  
  typedef LArConditionsContainer<LArPhysCaliTdiffP> CONTAINER ;
  
  LArPhysCaliTdiffComplete();
  
  virtual ~LArPhysCaliTdiffComplete( );
  
  // retrieving PhysCaliTdiff using online ID
  virtual  const float& Tdiff(const HWIdentifier&  CellID, int gain) const ;
  
  // retrieving PhysCaliTdiff using offline ID
  virtual  const float& Tdiff(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually
  // (if one wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain,
	   float tdiff);
  
//  static const float empty; //returned in case of non-existing data 
};

CLASS_DEF( LArPhysCaliTdiffComplete,265448020,1)
#endif 
