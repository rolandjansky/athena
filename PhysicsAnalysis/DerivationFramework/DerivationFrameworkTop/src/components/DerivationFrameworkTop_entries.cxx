#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h"
#include "DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h"

#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterAugmentation.h"
#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterTool.h"

using namespace DerivationFramework;


DECLARE_TOOL_FACTORY( TopHeavyFlavorFilterAugmentation )
DECLARE_TOOL_FACTORY( TTbarPlusHeavyFlavorFilterTool )
DECLARE_TOOL_FACTORY( BoostedHadTopAndTopPairFilterAugmentation )
DECLARE_TOOL_FACTORY( BoostedHadTopAndTopPairFilterTool )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTop ) {

  DECLARE_TOOL( TopHeavyFlavorFilterAugmentation )
  DECLARE_TOOL( BoostedHadTopAndTopPairFilterAugmentation )
  DECLARE_TOOL( DerivationFrameworkTop )

}
