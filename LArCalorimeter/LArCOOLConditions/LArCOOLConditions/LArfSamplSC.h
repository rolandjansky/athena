/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_FSAMPLSC_H
#define LARCOOLCONDITIONS_FSAMPLSC_H

#include "LArElecCalib/ILArfSampl.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArfSamplSC: public ILArfSampl,
		   public LArCondSuperCellBase,
		   public LArSingleFloatBlob {

public:
  LArfSamplSC(); 
  LArfSamplSC(const CondAttrListCollection* attrList);

  virtual ~LArfSamplSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArfSampl using online ID  
  virtual const float& FSAMPL(const HWIdentifier& chid) const;

  // retrieving LArfSampl using offline ID  
  virtual const float& FSAMPL(const Identifier& id) const; 
  
private:
  //static const float errorcode;

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArfSamplSC , 174984919, 1 )
CONDCONT_DEF( LArfSamplSC, 188510651, ILArfSampl );

#endif 
