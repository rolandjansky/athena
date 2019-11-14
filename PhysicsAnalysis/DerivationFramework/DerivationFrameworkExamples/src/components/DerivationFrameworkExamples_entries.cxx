#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkExamples/SkimmingToolExample.h"
#include "DerivationFrameworkExamples/ThinningToolExample.h"
#include "DerivationFrameworkExamples/AugmentationToolExample.h"
#include "DerivationFrameworkExamples/PHYSLITEThinningTool.h"
#include "DerivationFrameworkExamples/FloatCompressionAlg.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolExample )
DECLARE_TOOL_FACTORY( ThinningToolExample )
DECLARE_TOOL_FACTORY( AugmentationToolExample )
DECLARE_TOOL_FACTORY( PHYSLITEThinningTool )
DECLARE_ALGORITHM_FACTORY(FloatCompressionAlg )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkExamples ) {
   DECLARE_TOOL( SkimmingToolExample )
   DECLARE_TOOL( ThinningToolExample )
   DECLARE_TOOL( AugmentationToolExample )
   DECLARE_TOOL( PHYSLITEThinningTool )
   DECLARE_ALGORITHM( FloatCompressionAlg )
}

