#include "GaudiKernel/DeclareFactoryEntries.h"

// Top level tool
#include "METUtilities/METMaker.h"
#include "METUtilities/METRebuilder.h"
#include "METUtilities/METSystematicsTool.h"
#include "METUtilities/METSignificance.h"
// Algs
#include "../METUtilAlg.h"
#include "../METMakerAlg.h"

using namespace met;

DECLARE_TOOL_FACTORY(METMaker)
DECLARE_TOOL_FACTORY(METRebuilder)
DECLARE_TOOL_FACTORY(METSystematicsTool)
DECLARE_TOOL_FACTORY(METSignificance)
//
DECLARE_ALGORITHM_FACTORY(METUtilAlg)
DECLARE_ALGORITHM_FACTORY(METMakerAlg)

DECLARE_FACTORY_ENTRIES(METReconstruction) {
  DECLARE_TOOL(METMaker)
  DECLARE_TOOL(METRebuilder)
  DECLARE_TOOL(METSystematicsTool)
  DECLARE_TOOL(METSignificance)
  //
  DECLARE_ALGORITHM(METUtilAlg)
  DECLARE_ALGORITHM(METMakerAlg)
}
