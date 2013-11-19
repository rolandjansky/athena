/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonEvtValidator/MuonEvtValidatorAlg.h"
#include "MuonEvtValidator/MVCombined.h"
 

DECLARE_FACTORY_ENTRIES(MuonEvtValidator) 
{
  DECLARE_ALGORITHM( MuonEvtValidatorAlg )
  DECLARE_ALGORITHM( MVCombined )
}

DECLARE_ALGORITHM_FACTORY( MuonEvtValidatorAlg )
DECLARE_ALGORITHM_FACTORY( MVCombined )
 
  
  
