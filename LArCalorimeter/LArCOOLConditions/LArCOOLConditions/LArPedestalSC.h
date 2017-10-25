/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARPEDESTALSC_H
#define LARCOOLCONDITIONS_LARPEDESTALSC_H

#include "LArElecCalib/ILArPedestal.h" 
#include "LArCOOLConditions/LArCondSuperCellBase.h"
#include "LArCOOLConditions/LArPedestalBlob.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"


class CondAttrListCollection;

class LArPedestalSC: public ILArPedestal, 
		     public LArCondSuperCellBase,
                     public LArPedestalBlob {
  
private: 
  LArPedestalSC(); //private default constructor
  
public:
  LArPedestalSC(const CondAttrListCollection* attrList);

  //~LArPedestalSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }

  
  // retrieving Pedestal using online ID
  
  virtual float pedestal(const HWIdentifier&  CellID, int gain) const;
  
  virtual float pedestalRMS(const HWIdentifier&  CellID, int gain) const;

  virtual float pedestal(const Identifier&  CellID, int gain) const;
  
  virtual float pedestalRMS(const Identifier&  CellID, int gain) const;
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArPedestalSC,17770864,1  )
CONDCONT_DEF( LArPedestalSC, 219458938, ILArPedestal );
#endif 
