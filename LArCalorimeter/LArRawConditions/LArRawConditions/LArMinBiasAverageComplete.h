/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMINBIASAVERAGECOMPLETE_H
#define LARRAWCONDITIONS_LARMINBIASAVERAGECOMPLETE_H

#include "LArElecCalib/ILArMinBiasAverage.h" 
#include "LArRawConditions/LArMinBiasAverageP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

class LArCablingService;

/** This class implements the ILArMinBiasAverage interface
 * History:
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 */

class LArMinBiasAverageComplete: public ILArMinBiasAverage ,
       public LArConditionsContainer<LArMinBiasAverageP>
{
  
 public: 
  
  typedef LArConditionsContainer<LArMinBiasAverageP> CONTAINER ;

  LArMinBiasAverageComplete();
  
  virtual ~LArMinBiasAverageComplete( );
  
  // retrieving RMS of E in Minimum Bias events, using online ID
  
  virtual const float& minBiasAverage(const HWIdentifier&  CellID) const ;
  
  virtual const float& minBiasAverage(const Identifier&  CellID) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, float vMinBiasAverage);

 protected: 
  

};


CLASS_DEF( LArMinBiasAverageComplete, 242511813 ,1)
#endif 
