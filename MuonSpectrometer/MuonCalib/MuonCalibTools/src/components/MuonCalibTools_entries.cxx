#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibTools/PatternNtupleMaker.h"
#include "MuonCalibTools/IdToFixedIdTool.h"

using namespace MuonCalib;

DECLARE_TOOL_FACTORY( PatternNtupleMaker )
DECLARE_TOOL_FACTORY( IdToFixedIdTool )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( MuonCalibTools )
{
  DECLARE_TOOL( PatternNtupleMaker )
  DECLARE_TOOL( IdToFixedIdTool )
}
