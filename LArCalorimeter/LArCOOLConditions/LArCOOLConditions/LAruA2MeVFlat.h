/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARUA2MEVFLAT_H
#define LARCOOLCONDITIONS_LARUA2MEVFLAT_H

#include "LArElecCalib/ILAruA2MeV.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondFlatBase.h"

class CondAttrListCollection;

class LAruA2MeVFlat: public ILAruA2MeV,
		     public LArCondFlatBase,
		     public LArSingleFloatBlob {

public:
  LAruA2MeVFlat(); 
  LAruA2MeVFlat(const CondAttrListCollection* attrList);

  virtual ~LAruA2MeVFlat();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LAruA2MeV using online ID  
  virtual const float& UA2MEV(const HWIdentifier& chid) const;

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LAruA2MeVFlat , 92824273 , 1 )
CONDCONT_DEF( LAruA2MeVFlat, 84245593, ILAruA2MeV );
#endif 
