/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLMC_H
#define LARRAWCONDITIONS_LARFSAMPLMC_H

#include "LArRawConditions/LArfSamplComplete.h"

class LArfSamplMC: public LArfSamplComplete {
  
 public: 
  
  LArfSamplMC();  
  virtual ~LArfSamplMC();
    
 private: 

};

#include "AthenaKernel/CondCont.h"
CONDCONT_BASE(LArfSamplMC,ILArfSampl);
CLASS_DEF( LArfSamplMC, 164984905,1)
CLASS_DEF( CondCont<LArfSamplMC> , 24657825 , 1 )
#endif 
