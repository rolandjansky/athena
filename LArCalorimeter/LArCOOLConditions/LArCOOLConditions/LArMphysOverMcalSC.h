/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H
#define LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H

#include "LArElecCalib/ILArMphysOverMcal.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArMphysOverMcalSC: public ILArMphysOverMcal,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArMphysOverMcalSC(); 
  LArMphysOverMcalSC(const CondAttrListCollection* attrList);

  virtual ~LArMphysOverMcalSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArMphysOverMcal using online ID  
  virtual const float& MphysOverMcal(const HWIdentifier& chid, int gain) const;

private:
  //  static const float s_errorcode;

};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArMphysOverMcalSC ,125003343 , 1 )
CONDCONT_DEF( LArMphysOverMcalSC, 3164993, ILArMphysOverMcal );

#endif 
