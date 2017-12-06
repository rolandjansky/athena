/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARRAMPCOMPLETE_H
#define LARRAWCONDITIONS_LARRAMPCOMPLETE_H

#include "LArElecCalib/ILArRamp.h" 
#include "LArRawConditions/LArRampP1.h"
#include "LArRawConditions/LArConditionsContainer.h"

#include <vector>

/** This class implements the ILArRamp interface
 *
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 10/12/03
 *
 * Modifications:
 * S. Laplace, 16/01/2004 
 *       - Changed ADC2GEV to ADC2DAC
 * S. Laplace, 08/02/2004
 *       - new online ID
 */

class LArRampCompleteBase: public ILArRamp, 
  public LArConditionsContainer<LArRampP1> 
{  
 public: 
  
  LArRampCompleteBase();
  
  virtual ~LArRampCompleteBase( );
  
  typedef LArConditionsContainer<LArRampP1> CONTAINER ; 

  // retrieving Ramp using online ID
  
  virtual  RampRef_t ADC2DAC(const HWIdentifier&  CellID, int gain) const ;
  
  virtual  RampRef_t ADC2DAC(const Identifier&  CellID, int gain) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  virtual void set(const HWIdentifier& CellID, int gain,
		   const std::vector<float>& vRamp);
  
 protected: 
  
    const std::vector<float>&     empty() const;

};


class LArRampComplete: public LArRampCompleteBase {
  
 public: 
  
  LArRampComplete()  ;
  
  virtual ~LArRampComplete( ) ;
  
  virtual void set(const HWIdentifier& CellID, int gain,
		   const std::vector<float>& vRamp)
  {
     LArRampCompleteBase::set(CellID,gain,vRamp); 
  }

};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArRampCompleteBase, 157708224,1)
CLASS_DEF( LArRampComplete, 236941963,1)
CONDCONT_DEF( LArRampCompleteBase, 209705886, ILArRamp );
CONDCONT_DEF( LArRampComplete, 46870495, LArRampCompleteBase );

#endif 
