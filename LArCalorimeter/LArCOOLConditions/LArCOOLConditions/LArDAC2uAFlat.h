/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARDAC2UAFLAT_H
#define LARCOOLCONDITIONS_LARDAC2UAFLAT_H

#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondFlatBase.h"

class CondAttrListCollection;

class LArDAC2uAFlat: public ILArDAC2uA,
		     public LArCondFlatBase,
		     public LArSingleFloatBlob {

public:
  LArDAC2uAFlat(); 
  LArDAC2uAFlat(const CondAttrListCollection* attrList);

  virtual ~LArDAC2uAFlat();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArDAC2uA using online ID  
  virtual const float& DAC2UA(const HWIdentifier& chid) const;

  // retrieving LArDAC2uA using offline ID  
  virtual const float& DAC2UA(const Identifier& id) const; 
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArDAC2uAFlat , 54280245 , 1 )
CONDCONT_DEF( LArDAC2uAFlat, 251448397, ILArDAC2uA );
#endif 
