/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARWFPARAMSCOMPLETE_H
#define LARRAWCONDITIONS_LARWFPARAMSCOMPLETE_H

#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArWFParams.h"

#include <vector>
#include <iostream>

/*
 * Two ways of filling this data class:
 *   - intra ATHENA
 *   - from NOVA DB
 *
 * @author M. Fanti
 * @version  0-0-1 , 22/04/05
 *
 * History:
 */

class LArWFParamsComplete :public LArConditionsContainer<LArWFParams>
{
  
 public: 
  
  typedef LArConditionsContainer<LArWFParams> CONTAINER; 
  LArWFParamsComplete();
  
  virtual ~LArWFParamsComplete( );
  
  // retrieving coefficients using offline/online ID

  virtual const LArWFParams& getParams(const HWIdentifier&  CellID, int gain) const ;
 
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain, LArWFParams theParams);

 private: 

  static LArWFParams s_dummyParams ;

};

CLASS_DEF( LArWFParamsComplete, 243713630, 1) 


#endif 
