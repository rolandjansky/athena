#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonDQAUtils/MuonDQATrackHistTool.h"
#include "MuonDQAUtils/MuonDQAHistTool.h"
#include "MuonDQAUtils/MuonDQAHitHistTool.h"
#include "MuonDQAUtils/MuonDQAFitFunc.h"
#include "MuonDQAUtils/InsituTrackTools.h"
#include "MuonDQAUtils/MuonSpectrometerProbeCollectorTool.h"
#include "MuonDQAUtils/InnerDetProbeCollectorTool.h"

using namespace Muon;

  DECLARE_TOOL_FACTORY(MuonDQATrackHistTool)
  DECLARE_TOOL_FACTORY(MuonDQAHitHistTool)
  DECLARE_TOOL_FACTORY(MuonDQAHistTool)
  DECLARE_TOOL_FACTORY(MuonDQAFitFunc)
  DECLARE_TOOL_FACTORY(MuonSpectrometerProbeCollectorTool)
  DECLARE_TOOL_FACTORY(InnerDetProbeCollectorTool)
  DECLARE_TOOL_FACTORY(InsituTrackTools)

  DECLARE_FACTORY_ENTRIES(MuonDQAUtils)
{
  DECLARE_ALGTOOL(MuonDQAHistTool);
  DECLARE_ALGTOOL(MuonDQATrackHistTool);
  DECLARE_ALGTOOL(MuonDQAHistHistTool);
  DECLARE_ALGTOOL(MuonDQAFitFunc);  
  DECLARE_TOOL(MuonSpectrometerProbeCollectorTool)
  DECLARE_TOOL(InnerDetProbeCollectorTool)
  DECLARE_TOOL(InsituTrackTools)
}
 
 
