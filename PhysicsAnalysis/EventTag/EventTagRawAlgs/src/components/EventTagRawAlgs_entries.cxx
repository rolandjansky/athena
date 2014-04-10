#include "GaudiKernel/DeclareFactoryEntries.h"

#include "EventTagRawAlgs/RawInfoSummaryForTagWriter.h"

DECLARE_ALGORITHM_FACTORY( RawInfoSummaryForTagWriter )
DECLARE_FACTORY_ENTRIES( EventTagRawAlgs ) {
  DECLARE_ALGORITHM( RawInfoSummaryForTagWriter )
}
