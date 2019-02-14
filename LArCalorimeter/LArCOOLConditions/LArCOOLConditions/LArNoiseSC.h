/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARNOISESSC_H
#define LARCOOLCONDITIONS_LARNOISESSC_H

#include "LArElecCalib/ILArNoise.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArNoiseSC: public ILArNoise,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArNoiseSC(); 
  LArNoiseSC(const CondAttrListCollection* attrList);

  virtual ~LArNoiseSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArNoise using online ID  
  virtual const float& noise(const HWIdentifier& chid, int gain) const;

private:
  //static const float errorcode;

};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArNoiseSC ,234451881 , 1 )
CONDCONT_DEF( LArNoiseSC, 89903463, ILArNoise );

#endif 
