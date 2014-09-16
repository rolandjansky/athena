#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigL2MuonSA/MuFastSteering.h"
#include "TrigL2MuonSA/MuFastDataPreparator.h"
#include "TrigL2MuonSA/TgcDataPreparator.h"
#include "TrigL2MuonSA/RpcDataPreparator.h"
#include "TrigL2MuonSA/MdtDataPreparator.h"
//#include "TrigL2MuonSA/CscDataPreparator.h"
#include "TrigL2MuonSA/MuFastPatternFinder.h"
#include "TrigL2MuonSA/MuFastTrackFitter.h"
#include "TrigL2MuonSA/MuFastStationFitter.h"
#include "TrigL2MuonSA/MuFastTrackExtrapolator.h"
#include "TrigL2MuonSA/PtBarrelLUTSvc.h"
#include "TrigL2MuonSA/PtEndcapLUTSvc.h"
#include "TrigL2MuonSA/AlignmentBarrelLUTSvc.h"
#include "TrigL2MuonSA/MuCalStreamer.h"
#include "TrigL2MuonSA/MuCalStreamerTool.h"

using namespace TrigL2MuonSA;

DECLARE_ALGORITHM_FACTORY(MuFastSteering)
DECLARE_ALGORITHM_FACTORY(MuCalStreamer)
DECLARE_TOOL_FACTORY(MuCalStreamerTool)
DECLARE_TOOL_FACTORY(MuFastDataPreparator)
DECLARE_TOOL_FACTORY(TgcDataPreparator)
DECLARE_TOOL_FACTORY(RpcDataPreparator)
DECLARE_TOOL_FACTORY(MdtDataPreparator)
//DECLARE_TOOL_FACTORY(CscDataPreparator)
DECLARE_TOOL_FACTORY(MuFastPatternFinder)
DECLARE_TOOL_FACTORY(MuFastStationFitter)
DECLARE_TOOL_FACTORY(MuFastTrackFitter)
DECLARE_TOOL_FACTORY(MuFastTrackExtrapolator)

DECLARE_SERVICE_FACTORY(PtBarrelLUTSvc)
DECLARE_SERVICE_FACTORY(PtEndcapLUTSvc)
DECLARE_SERVICE_FACTORY(AlignmentBarrelLUTSvc)

DECLARE_FACTORY_ENTRIES(TrigL2MuonSA)
{
  DECLARE_ALGORITHM(MuFastSteering);
  DECLARE_ALGORITHM(MuCalStreamer);
  DECLARE_TOOL(MuCalStreamerTool)
  DECLARE_TOOL(MuFastDataPreparator);
  DECLARE_TOOL(TgcDataPreparator);
  DECLARE_TOOL(RpcDataPreparator);
  DECLARE_TOOL(MdtDataPreparator);
  //  DECLARE_TOOL(CscDataPreparator);
  DECLARE_TOOL(MuFastPatternFinder);
  DECLARE_TOOL(MuFastStationFitter);
  DECLARE_TOOL(MuFastTrackFitter);
  DECLARE_TOOL(MuFastTrackExtrapolator);

  DECLARE_SERVICE(PtBarrelLUTSvc);
  DECLARE_SERVICE(PtEndcapLUTSvc);
  DECLARE_SERVICE(AlignmentBarrelLUTSvc);
}
