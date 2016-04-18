#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IsolationTool/TrackIsolationTool.h"
#include "IsolationTool/CaloIsolationTool.h"
#include "IsolationTool/IsolationHelper.h"
#include "../TestIsolationToolAthenaAlg.h"
#include "../IsolationTool_AthTest.h"

using namespace xAOD;

DECLARE_TOOL_FACTORY( TrackIsolationTool )
DECLARE_TOOL_FACTORY( CaloIsolationTool )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, IsolationHelper )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP , TestIsolationToolAthenaAlg )
// DECLARE_ALGORITHM_FACTORY( IsolationTool_AthTest )

DECLARE_FACTORY_ENTRIES( IsolationTool )
{
  DECLARE_TOOL( TrackIsolationTool )
  DECLARE_TOOL( CaloIsolationTool )
  DECLARE_NAMESPACE_TOOL( CP , IsolationHelper )
  DECLARE_NAMESPACE_ALGORITHM( CP , TestIsolationToolAthenaAlg )
//   DECLARE_ALGORITHM( IsolationTool_AthTest )
}