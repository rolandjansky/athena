#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../SiHitAnalysis.h"
#include "../CaloHitAnalysis.h"
#include "../TRTHitAnalysis.h"
#include "../TGCHitAnalysis.h"
#include "../RPCHitAnalysis.h"
#include "../MDTHitAnalysis.h"
#include "../CSCHitAnalysis.h"
#include "../TruthHitAnalysis.h"
#include "../TrackRecordAnalysis.h"
#include "../ALFAHitAnalysis.h"
#include "../LucidHitAnalysis.h"
#include "../ZDCHitAnalysis.h"
#include "../AFPHitAnalysis.h"
#include "../MMHitAnalysis.h"
#include "../sTGCHitAnalysis.h"

DECLARE_ALGORITHM_FACTORY( CaloHitAnalysis )
DECLARE_ALGORITHM_FACTORY( SiHitAnalysis )
DECLARE_ALGORITHM_FACTORY( TRTHitAnalysis )
DECLARE_ALGORITHM_FACTORY( TGCHitAnalysis )
DECLARE_ALGORITHM_FACTORY( RPCHitAnalysis )
DECLARE_ALGORITHM_FACTORY( MDTHitAnalysis )
DECLARE_ALGORITHM_FACTORY( CSCHitAnalysis )
DECLARE_ALGORITHM_FACTORY( TruthHitAnalysis )
DECLARE_ALGORITHM_FACTORY( TrackRecordAnalysis )
DECLARE_ALGORITHM_FACTORY( ALFAHitAnalysis )
DECLARE_ALGORITHM_FACTORY( LucidHitAnalysis )
DECLARE_ALGORITHM_FACTORY( ZDCHitAnalysis )
DECLARE_ALGORITHM_FACTORY( AFPHitAnalysis )
DECLARE_ALGORITHM_FACTORY( MMHitAnalysis )
DECLARE_ALGORITHM_FACTORY( sTGCHitAnalysis )

DECLARE_FACTORY_ENTRIES( HitAnalysis ) {
  DECLARE_ALGORITHM( CaloHitAnalysis )
    DECLARE_ALGORITHM( SiHitAnalysis )
    DECLARE_ALGORITHM( TRTHitAnalysis )
    DECLARE_ALGORITHM( TGCHitAnalysis )
    DECLARE_ALGORITHM( RPCHitAnalysis )
    DECLARE_ALGORITHM( MDTHitAnalysis )
    DECLARE_ALGORITHM( CSCHitAnalysis )
    DECLARE_ALGORITHM( TruthHitAnalysis )
    DECLARE_ALGORITHM( TrackRecordAnalysis )
    DECLARE_ALGORITHM( ALFAHitAnalysis )
    DECLARE_ALGORITHM( LucidHitAnalysis )
    DECLARE_ALGORITHM( ZDCHitAnalysis )
    DECLARE_ALGORITHM( AFPHitAnalysis )
    DECLARE_ALGORITHM( MMHitAnalysis )
    DECLARE_ALGORITHM( sTGCHitAnalysis )
 

}

