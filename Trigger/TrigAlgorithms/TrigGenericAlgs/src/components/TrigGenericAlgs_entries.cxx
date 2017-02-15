#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigGenericAlgs/DummyFEX.h"
#include "TrigGenericAlgs/DummyUnseededAllTEAlgo.h"
#include "TrigGenericAlgs/DummyCopyAllTEAlgo.h"
#include "TrigGenericAlgs/DummyCombineAlgo.h"
#include "TrigGenericAlgs/OverlapRemoval.h"
#include "TrigGenericAlgs/ReverseRoI.h"
#include "TrigGenericAlgs/AcceptWhenInputMissing.h"
#include "../AcceptWhenInputPresent.h"
#include "TrigGenericAlgs/PrescaleAlgo.h"
#include "TrigGenericAlgs/SeededSuperRoiAllTEAlgo.h"
#include "TrigGenericAlgs/L1CorrelationAlgo.h"
#include "TrigGenericAlgs/DetectorTimingAlgo.h"
#include "../ROBRequestAlgo.h"
#include "../TimeBurner.h"
#include "../AcceptAnyInput.h"
#include "../TrigRoiUpdater.h"
#include "../MergeTopoStarts.h"


using namespace PESA;

DECLARE_ALGORITHM_FACTORY( DummyFEX )
DECLARE_ALGORITHM_FACTORY( DummyUnseededAllTEAlgo )
DECLARE_ALGORITHM_FACTORY( DummyCopyAllTEAlgo )
DECLARE_ALGORITHM_FACTORY( DummyCombineAlgo )
DECLARE_ALGORITHM_FACTORY( OverlapRemoval )
DECLARE_ALGORITHM_FACTORY( ReverseRoI )
DECLARE_ALGORITHM_FACTORY( AcceptWhenInputMissing )
DECLARE_ALGORITHM_FACTORY( AcceptWhenInputPresent )
DECLARE_ALGORITHM_FACTORY( PrescaleAlgo )
DECLARE_ALGORITHM_FACTORY( ROBRequestAlgo )
DECLARE_ALGORITHM_FACTORY( L1CorrelationAlgo )
DECLARE_ALGORITHM_FACTORY( DetectorTimingAlgo )
DECLARE_ALGORITHM_FACTORY( TimeBurner )
DECLARE_ALGORITHM_FACTORY( AcceptAnyInput )
DECLARE_ALGORITHM_FACTORY( SeededSuperRoiAllTEAlgo )
DECLARE_ALGORITHM_FACTORY( TrigRoiUpdater )
DECLARE_ALGORITHM_FACTORY( MergeTopoStarts )


DECLARE_FACTORY_ENTRIES(TrigGenericAlgs) {
    DECLARE_ALGORITHM( DummyFEX )
    DECLARE_ALGORITHM( DummyUnseededAllTEAlgo )
    DECLARE_ALGORITHM( DummyCopyAllTEAlgo )
    DECLARE_ALGORITHM( DummyCombineAlgo )
    DECLARE_ALGORITHM( OverlapRemoval )
    DECLARE_ALGORITHM( ReverseRoI )
    DECLARE_ALGORITHM( AcceptWhenInputMissing )
    DECLARE_ALGORITHM( AcceptWhenInputPresent )
    DECLARE_ALGORITHM( PrescaleAlgo )
    DECLARE_ALGORITHM( ROBRequestAlgo )
    DECLARE_ALGORITHM( L1CorrelationAlgo )
    DECLARE_ALGORITHM( DetectorTimingAlgo ) 
    DECLARE_ALGORITHM( TimeBurner )      
    DECLARE_ALGORITHM( AcceptAnyInput )      
    DECLARE_ALGORITHM( SeededSuperRoiAllTEAlgo )      
    DECLARE_ALGORITHM( TrigRoiUpdater )
    DECLARE_ALGORITHM( MergeTopoStarts )
}
