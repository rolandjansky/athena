#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigFTK_RawDataAlgs/FTK_RDO_CreatorAlgo.h"
#include "TrigFTK_RawDataAlgs/FTK_RDO_ReaderAlgo.h"

DECLARE_ALGORITHM_FACTORY( FTK_RDO_CreatorAlgo )
DECLARE_ALGORITHM_FACTORY( FTK_RDO_ReaderAlgo )

DECLARE_FACTORY_ENTRIES(TrigFTK_RawDataAlgs) {
  DECLARE_ALGORITHM( FTK_RDO_CreatorAlgo );
  DECLARE_ALGORITHM( FTK_RDO_ReaderAlgo );
}
