#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkExamples/SkimmingToolExample.h"
#include "DerivationFrameworkExamples/NTUPSkimmingToolExample.h"
#include "DerivationFrameworkExamples/ThinningToolExample.h"
#include "DerivationFrameworkExamples/NTUPAugmentationToolExample.h"
using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolExample )
DECLARE_TOOL_FACTORY( NTUPSkimmingToolExample )
DECLARE_TOOL_FACTORY( ThinningToolExample )
DECLARE_TOOL_FACTORY( NTUPAugmentationToolExample )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkExamples ) {
   DECLARE_TOOL( SkimmingToolExample )
   DECLARE_TOOL( NTUPSkimmingToolExample )
   DECLARE_TOOL( ThinningToolExample )
   DECLARE_TOOL( NTUPAugmentationToolExample )
}

