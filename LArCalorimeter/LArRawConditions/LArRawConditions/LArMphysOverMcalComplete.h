/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARRAWCONDITIONS_LARMPHYSOVERMCALCOMPLETE_H
#define LARRAWCONDITIONS_LARMPHYSOVERMCALCOMPLETE_H

#include "LArElecCalib/ILArMphysOverMcal.h" 
#include "LArRawConditions/LArSingleFloatP.h"
#include "LArRawConditions/LArConditionsContainer.h"
#include <vector>


/** This class implements the ILArMphysOverMcal interface
 *
 * @author S. Laplace
 * @version  0-0-1 , 01/07/05
 *
 *
 */

/**  use LArConditionsContainer. 
   Hong Ma , Aug 5, 2005
*/ 


class LArMphysOverMcalComplete: public ILArMphysOverMcal,
				public LArConditionsContainer<LArSingleFloatP> {
 public: 
  typedef  LArConditionsContainer<LArSingleFloatP> CONTAINER;
  LArMphysOverMcalComplete(); 
  
  virtual ~LArMphysOverMcalComplete();
  
  // retrieving MphysOverMcal using online ID  
  virtual const float& MphysOverMcal(const HWIdentifier& chid, int gain) const
    {return (this->get(chid,gain).m_data);};

  // retrieving MphysOverMcal using offline ID  
  virtual const float& MphysOverMcal(const Identifier& id, int gain) const;
  
  // wants to fill this class not using the DB)
  void set(const HWIdentifier& CellID, int gain, float vMphysOverMcal);
  
    
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMphysOverMcalComplete,84212362,1)
CONDCONT_DEF( LArMphysOverMcalComplete, 216720142, ILArMphysOverMcal );
#endif 
