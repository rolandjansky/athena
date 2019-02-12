/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARDAC2UASC_H
#define LARCOOLCONDITIONS_LARDAC2UASC_H

#include "LArElecCalib/ILArDAC2uA.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArDAC2uASC: public ILArDAC2uA,
                   public LArCondSuperCellBase,
		   public LArSingleFloatBlob {

public:
  LArDAC2uASC(); 
  LArDAC2uASC(const CondAttrListCollection* attrList);

  virtual ~LArDAC2uASC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArDAC2uA using online ID  
  virtual const float& DAC2UA(const HWIdentifier& chid) const;

private:
  //  static const float errorcode;
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArDAC2uASC ,74944220 , 1 )
CONDCONT_DEF( LArDAC2uASC, 122040978, ILArDAC2uA );

#endif 
