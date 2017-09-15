#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetOverlay/InDetOverlay.h"
#include "InDetOverlay/InDetSDOOverlay.h"

DECLARE_ALGORITHM_FACTORY( InDetOverlay )
DECLARE_ALGORITHM_FACTORY( InDetSDOOverlay )
  
DECLARE_FACTORY_ENTRIES( InDetOverlay ) {
  DECLARE_ALGORITHM( InDetOverlay )
  DECLARE_ALGORITHM( InDetSDOOverlay )
}
