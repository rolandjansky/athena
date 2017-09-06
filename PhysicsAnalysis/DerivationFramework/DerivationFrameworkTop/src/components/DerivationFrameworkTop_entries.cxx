#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DerivationFrameworkTop/TopHeavyFlavorFilterAugmentation.h"
#include "DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h"

#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterAugmentation.h"
#include "DerivationFrameworkTop/BoostedHadTopAndTopPairFilterTool.h"

#include "DerivationFrameworkTop/JetMSVAugmentation.h"
#include "DerivationFrameworkTop/SV1TrackThinning.h"

using namespace DerivationFramework;


DECLARE_TOOL_FACTORY( TopHeavyFlavorFilterAugmentation )
DECLARE_TOOL_FACTORY( TTbarPlusHeavyFlavorFilterTool )
DECLARE_TOOL_FACTORY( BoostedHadTopAndTopPairFilterAugmentation )
DECLARE_TOOL_FACTORY( BoostedHadTopAndTopPairFilterTool )
DECLARE_TOOL_FACTORY( JetMSVAugmentation )
DECLARE_TOOL_FACTORY( SV1TrackThinning )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTop ) {

  DECLARE_TOOL( TopHeavyFlavorFilterAugmentation )
  DECLARE_TOOL( BoostedHadTopAndTopPairFilterAugmentation )
  DECLARE_TOOL( DerivationFrameworkTop )
  DECLARE_TOOL( JetMSVAugmentation  )
  DECLARE_TOOL( SV1TrackThinning  )

}
