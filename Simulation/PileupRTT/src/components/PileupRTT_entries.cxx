#include "PileupRTT/PileupValidation.h"
#include "PileupRTT/PileupPixelOccupancy.h"
#include "PileupRTT/PileupSCTOccupancy.h"
#include "PileupRTT/PileupMCEventInfo.h"
#include "PileupRTT/PileupValidationTools.h"
#include "PileupRTT/PileupMCEventCollectionChecker.h"
#include "PileupRTT/PileupSimpleEventInfo.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace PileupRTT;

DECLARE_ALGORITHM_FACTORY( PileupValidation )
DECLARE_ALGORITHM_FACTORY( PileupPixelOccupancy )
DECLARE_ALGORITHM_FACTORY( PileupSCTOccupancy )
DECLARE_ALGORITHM_FACTORY( PileupMCEventInfo )
DECLARE_ALGORITHM_FACTORY( PileupMCEventCollectionChecker )
DECLARE_ALGORITHM_FACTORY( PileupSimpleEventInfo )
DECLARE_TOOL_FACTORY( PileupValidationTools )

DECLARE_FACTORY_ENTRIES( PileupRTT ) {
  DECLARE_ALGORITHM( PileupValidation )
  DECLARE_ALGORITHM( PileupPixelOccupancy )
  DECLARE_ALGORITHM( PileupSCTOccupancy )
  DECLARE_ALGORITHM( PileupMCEventInfo )
  DECLARE_ALGORITHM( PileupMCEventCollectionChecker )
  DECLARE_ALGORITHM( PileupSimpleEventInfo )
  DECLARE_TOOL( PileupValidationTools )
}
