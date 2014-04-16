/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonDigitTest/MuonDigitTestAlg.h"
#include "MuonDigitTest/MDTDigitsTestAlg.h"
#include "MuonDigitTest/RPCDigitsTestAlg.h"
//#include "MuonDigitTest/CSCDigitsTestAlg.h"
#include "MuonDigitTest/TGCDigitsTestAlg.h"
 
DECLARE_FACTORY_ENTRIES(MuonDigitTest) 
{
    DECLARE_ALGORITHM( MuonDigitTestAlg )
    DECLARE_ALGORITHM( MDTDigitsTestAlg )
    DECLARE_ALGORITHM( RPCDigitsTestAlg )
//    DECLARE_ALGORITHM( CSCDigitsTestAlg )
    DECLARE_ALGORITHM( TGCDigitsTestAlg )
}

DECLARE_ALGORITHM_FACTORY( MuonDigitTestAlg )
  DECLARE_ALGORITHM_FACTORY( MDTDigitsTestAlg )
  DECLARE_ALGORITHM_FACTORY( RPCDigitsTestAlg )
//  DECLARE_ALGORITHM_FACTORY( CSCDigitsTestAlg )
  DECLARE_ALGORITHM_FACTORY( TGCDigitsTestAlg )
 
  
