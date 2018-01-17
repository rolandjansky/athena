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
#include "../AcceptL1TopoMonitor.h"


DECLARE_COMPONENT( DummyFEX )
DECLARE_COMPONENT( PESA::DummyUnseededAllTEAlgo )
DECLARE_COMPONENT( PESA::DummyCopyAllTEAlgo )
DECLARE_COMPONENT( PESA::DummyCombineAlgo )
DECLARE_COMPONENT( OverlapRemoval )
DECLARE_COMPONENT( ReverseRoI )
DECLARE_COMPONENT( AcceptWhenInputMissing )
DECLARE_COMPONENT( AcceptWhenInputPresent )
DECLARE_COMPONENT( PrescaleAlgo )
DECLARE_COMPONENT( ROBRequestAlgo )
DECLARE_COMPONENT( L1CorrelationAlgo )
DECLARE_COMPONENT( DetectorTimingAlgo )
DECLARE_COMPONENT( TimeBurner )
DECLARE_COMPONENT( AcceptAnyInput )
DECLARE_COMPONENT( PESA::SeededSuperRoiAllTEAlgo )
DECLARE_COMPONENT( PESA::TrigRoiUpdater )
DECLARE_COMPONENT( MergeTopoStarts )
DECLARE_COMPONENT( AcceptL1TopoMonitor )
