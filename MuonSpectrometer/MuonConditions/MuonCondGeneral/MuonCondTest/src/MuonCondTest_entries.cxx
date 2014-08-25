/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondTest/AlignCondAthTest.h"
#include "MuonCondTest/MuonDetectorStatusTest.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "MuonCondTest/RpcStatusTest.h"
#include "MuonCondTest/MuonConditionsTestAlg.h" 
#include "MuonCondTest/MDT_DCSStatusTest.h"
#include "MuonCondTest/MDTConditionsTestAlg.h"
#include "MuonCondTest/RPCStatusTestAlg.h"
#include "MuonCondTest/MuonConditionsHistoSummary.h"

DECLARE_ALGORITHM_FACTORY( AlignCondAthTest )
DECLARE_ALGORITHM_FACTORY( MuonDetectorStatusTest )
//DECLARE_ALGORITHM_FACTORY( RpcStatusTest )
DECLARE_ALGORITHM_FACTORY( MuonConditionsTestAlg )
DECLARE_ALGORITHM_FACTORY( MDT_DCSStatusTest )
DECLARE_ALGORITHM_FACTORY( MDTConditionsTestAlg )
DECLARE_ALGORITHM_FACTORY( RPCStatusTestAlg )
DECLARE_ALGORITHM_FACTORY( MuonConditionsHistoSummary )

DECLARE_FACTORY_ENTRIES( MuonCondTest ) {
	DECLARE_ALGORITHM( AlignCondAthTest );
	DECLARE_ALGORITHM( MuonDetectorStatusTest );
//        DECLARE_ALGORITHM( RpcStatusTest );
	DECLARE_ALGORITHM( MuonConditionsTestAlg );
        DECLARE_ALGORITHM( MDT_DCSStatusTest );
        DECLARE_ALGORITHM( MDTConditionsTestAlg );
        DECLARE_ALGORITHM( RPCStatusTestAlg );
        DECLARE_ALGORITHM( MuonConditionsHistoSummary );
}
 
