#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetOverlay/InDetOverlay.h"
#include "InDetOverlay/DynConfSCT.h"

DECLARE_ALGORITHM_FACTORY( InDetOverlay )
DECLARE_ALGORITHM_FACTORY( DynConfSCT )
  
DECLARE_FACTORY_ENTRIES( InDetOverlay ) {
  DECLARE_ALGORITHM( InDetOverlay )
  DECLARE_ALGORITHM( DynConfSCT )
}
