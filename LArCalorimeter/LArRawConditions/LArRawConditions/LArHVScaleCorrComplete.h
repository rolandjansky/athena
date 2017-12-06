/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARRAWCONDITIONS_LARHVSCALECORRCOMPLETE_H
#define LARRAWCONDITIONS_LARHVSCALECORRCOMPLETE_H

#include "LArElecCalib/ILArHVScaleCorr.h" 
#include "LArRawConditions/LArSingleFloatP.h"
#include "LArRawConditions/LArConditionsContainer.h"

/** This class implements the ILArHVScaleCorr interface
 * @author W. Lampl
 */

class LArHVScaleCorrComplete: public ILArHVScaleCorr ,
	public LArConditionsContainer<LArSingleFloatP> {

 public: 
  
  typedef LArConditionsContainer<LArSingleFloatP> CONTAINER ;

    // online ID
  inline const float& HVScaleCorr(const HWIdentifier& id )  const 
    {return (this->get(id).m_data);};
  
  // offline ID 
  const float& HVScaleCorr(const Identifier& id ) const;
  
  // set method filling the data members individually (if one
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, const float vDAC2uA);

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArHVScaleCorrComplete, 220593802,1)
CONDCONT_DEF(LArHVScaleCorrComplete, 84955454, ILArHVScaleCorr);

#endif 
