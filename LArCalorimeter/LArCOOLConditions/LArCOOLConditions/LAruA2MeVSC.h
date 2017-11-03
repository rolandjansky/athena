/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARUA2MEVSC_H
#define LARCOOLCONDITIONS_LARUA2MEVSC_H

#include "LArElecCalib/ILAruA2MeV.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"

class CondAttrListCollection;

class LAruA2MeVSC: public ILAruA2MeV,
		   public LArCondSuperCellBase,
		   public LArSingleFloatBlob {

public:
  LAruA2MeVSC(); 
  LAruA2MeVSC(const CondAttrListCollection* attrList);

  virtual ~LAruA2MeVSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LAruA2MeV using online ID  
  virtual const float& UA2MEV(const HWIdentifier& chid) const;

  // retrieving LAruA2MeV using offline ID  
  virtual const float& UA2MEV(const Identifier& id) const; 

private:
  //static const float errorcode;

  
};
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LAruA2MeVSC ,142345128  , 1 )
CONDCONT_DEF( LAruA2MeVSC, 155224118, ILAruA2MeV );
#endif 
