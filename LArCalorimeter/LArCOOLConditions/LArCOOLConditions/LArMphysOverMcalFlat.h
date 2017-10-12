/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMPHYSOVERMCALFLAT_H
#define LARCOOLCONDITIONS_LARMPHYSOVERMCALFLAT_H

#include "LArElecCalib/ILArMphysOverMcal.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondFlatBase.h"

class CondAttrListCollection;

class LArMphysOverMcalFlat: public ILArMphysOverMcal,
			    public LArCondFlatBase,
			    public LArSingleFloatBlob {
			    
private:
  LArMphysOverMcalFlat(); 

public:
  LArMphysOverMcalFlat(const CondAttrListCollection* attrList);
  virtual ~LArMphysOverMcalFlat();
  
  bool good() const { return m_isInitialized && m_nChannels>0; }

  // retrieving MphysOverMcal using online ID  
  virtual const float& MphysOverMcal(const HWIdentifier& chid, int gain) const;

  // retrieving MphysOverMcal using offline ID  
  virtual const float& MphysOverMcal(const Identifier& id, int gain) const; 
  
};


CLASS_DEF( LArMphysOverMcalFlat , 219931062 , 1 )
//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArMphysOverMcalFlat> , 261355674 , 1 )
#endif 
