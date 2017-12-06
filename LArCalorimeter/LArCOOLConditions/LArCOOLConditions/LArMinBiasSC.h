/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMINBIASSC_H
#define LARCOOLCONDITIONS_LARMINBIASSC_H

#include "LArElecCalib/ILArMinBias.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArMinBiasSC: public ILArMinBias,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArMinBiasSC(); 
  LArMinBiasSC(const CondAttrListCollection* attrList);

  virtual ~LArMinBiasSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArMinBias using online ID  
  virtual const float& minBiasRMS(const HWIdentifier& chid) const;

  // retrieving LArMinBias using offline ID  
  virtual const float& minBiasRMS(const Identifier& id) const; 
  
private:
  //static const float errorcode;

};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMinBiasSC ,156198230 , 1 )
CONDCONT_DEF( LArMinBiasSC, 98956242, ILArMinBias );

#endif 
