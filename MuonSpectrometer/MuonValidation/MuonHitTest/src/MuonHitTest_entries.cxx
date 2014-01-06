/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonHitTest/MuonHitValAlg.h"
#include "MuonHitTest/TruthValAlg.h"
#include "MuonHitTest/MDTHitsValAlg.h"
#include "MuonHitTest/RPCHitsValAlg.h"
#include "MuonHitTest/CSCHitsValAlg.h"
#include "MuonHitTest/TGCHitsValAlg.h"
#include "MuonHitTest/MuonTrackRecordValAlg.h"

DECLARE_FACTORY_ENTRIES(MuonHitTest) 
{
  DECLARE_ALGORITHM( MuonHitValAlg )
    DECLARE_ALGORITHM( TruthValAlg )
    DECLARE_ALGORITHM( MDTHitsValAlg )
    DECLARE_ALGORITHM( RPCHitsValAlg )
    DECLARE_ALGORITHM( CSCHitsValAlg )
    DECLARE_ALGORITHM( TGCHitsValAlg )
    DECLARE_ALGORITHM( MuonTrackRecordValAlg ) 
}

DECLARE_ALGORITHM_FACTORY( MuonHitValAlg )
  DECLARE_ALGORITHM_FACTORY( TruthValAlg )
  DECLARE_ALGORITHM_FACTORY( MDTHitsValAlg )
  DECLARE_ALGORITHM_FACTORY( RPCHitsValAlg )
  DECLARE_ALGORITHM_FACTORY( CSCHitsValAlg )
  DECLARE_ALGORITHM_FACTORY( TGCHitsValAlg )
  DECLARE_ALGORITHM_FACTORY( MuonTrackRecordValAlg )
 
  
