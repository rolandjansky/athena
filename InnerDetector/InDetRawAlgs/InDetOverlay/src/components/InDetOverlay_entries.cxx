#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetOverlay/InDetOverlay.h"
#include "InDetOverlay/InDetSDOOverlay.h"
#include "InDetOverlay/DynConfSCT.h"

DECLARE_ALGORITHM_FACTORY( InDetOverlay )
DECLARE_ALGORITHM_FACTORY( InDetSDOOverlay )
DECLARE_ALGORITHM_FACTORY( DynConfSCT )
  
DECLARE_FACTORY_ENTRIES( InDetOverlay ) {
  DECLARE_ALGORITHM( InDetOverlay )
  DECLARE_ALGORITHM( InDetSDOOverlay )
  DECLARE_ALGORITHM( DynConfSCT )
}
