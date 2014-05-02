/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARTDRIFTCOMPLETE_H
#define LARRAWCONDITIONS_LARTDRIFTCOMPLETE_H

#include "LArElecCalib/ILArTdrift.h" 
#include "LArRawConditions/LArTdriftP.h"
#include "LArRawConditions/LArConditionsContainer.h"


#include <vector>

class LArCablingService ;

/** This class implements the ILArTdrift interface
 *
 * @author M. Fanti
 * @version  0-0-1 , 21/07/05
 *
 * History:
 *  - 
 */

class LArTdriftComplete: public ILArTdrift,
  public LArConditionsContainer<LArTdriftP>{

 public: 
  
  typedef LArConditionsContainer<LArTdriftP> CONTAINER ;
  LArTdriftComplete();
  
  virtual ~LArTdriftComplete( );
  
  // retrieving Tdrift using online ID
  virtual  const float& Tdrift(const HWIdentifier&  CellID) const ;
  
  // retrieving Tdrift using offline ID
  virtual  const float& Tdrift(const Identifier&  CellID) const;
  
  // set method filling the data members individually
  // (if one wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float tdrift);
  

};

CLASS_DEF( LArTdriftComplete,89410564,1)
#endif 
