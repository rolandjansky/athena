/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARNOISECOMPLETE_H
#define LARRAWCONDITIONS_LARNOISECOMPLETE_H

#include "LArElecCalib/ILArNoise.h" 
#include "LArRawConditions/LArNoiseP.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArNoise interface
 *
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 08/01/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 */

class LArNoiseComplete: public ILArNoise,
  public  LArConditionsContainer<LArNoiseP> 
{
public: 
  typedef LArConditionsContainer<LArNoiseP> CONTAINER ;
  LArNoiseComplete();
  virtual ~LArNoiseComplete( );
  
  // retrieving Noise using online ID
  virtual  const float& noise(const HWIdentifier&  CellID,int gain) const override;

  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain, float vNoise);  
};

CLASS_DEF( LArNoiseComplete, 12532144, 1)
#endif 
