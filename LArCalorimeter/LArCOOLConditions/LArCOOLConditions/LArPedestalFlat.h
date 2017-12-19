/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARPEDESTALFLAT_H
#define LARCOOLCONDITIONS_LARPEDESTALFLAT_H

#include "LArElecCalib/ILArPedestal.h" 
#include "LArCOOLConditions/LArCondFlatBase.h"
#include "LArCOOLConditions/LArPedestalBlob.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"


class CondAttrListCollection;

class LArPedestalFlat: public ILArPedestal, 
		       public LArCondFlatBase,
                       public LArPedestalBlob {
  
private: 
  LArPedestalFlat(); //private default constructor
  
public:
  LArPedestalFlat(const CondAttrListCollection* attrList);

  //~LArPedestalFlat();

  bool good() const { return m_isInitialized && m_nChannels>0; }

  
  // retrieving Pedestal using online ID
  
  virtual float pedestal(const HWIdentifier&  CellID, int gain) const;
  
  virtual float pedestalRMS(const HWIdentifier&  CellID, int gain) const;

  virtual float pedestal(const Identifier&  CellID, int gain) const;
  
  virtual float pedestalRMS(const Identifier&  CellID, int gain) const;
  
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArPedestalFlat,69859981,1  )
CONDCONT_DEF( LArPedestalFlat, 151238349, ILArPedestal );
#endif 
