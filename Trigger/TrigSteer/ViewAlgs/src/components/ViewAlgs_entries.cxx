
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../EventViewCreatorAlgorithm.h"
#include "../MergeViews.h"
#include "../TrigL2ElectronViewsMergerTool.h"
#include "../TrigL2CaloViewsMergerTool.h"


DECLARE_ALGORITHM_FACTORY( EventViewCreatorAlgorithm )
DECLARE_ALGORITHM_FACTORY( MergeViews )
DECLARE_TOOL_FACTORY( TrigL2ElectronViewsMergerTool )
DECLARE_TOOL_FACTORY( TrigL2CaloViewsMergerTool )


DECLARE_FACTORY_ENTRIES( ViewAlgs )
{
  DECLARE_ALGORITHM( EventViewCreatorAlgorithm )
  DECLARE_ALGORITHM( MergeViews )
  DECLARE_TOOL( TrigL2ElectronViewsMerger )
  DECLARE_TOOL( TrigL2CaloViewsMerger )
}

