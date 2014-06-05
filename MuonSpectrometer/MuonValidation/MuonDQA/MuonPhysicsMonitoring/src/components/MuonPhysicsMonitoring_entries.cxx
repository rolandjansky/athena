#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonPhysicsMonitoring/ZmumuPhysMonTool.h"
#include "MuonPhysicsMonitoring/JpsimumuPhysMonTool.h"
#include "MuonPhysicsMonitoring/JpsiMuonInsituPerformance.h"
#include "MuonPhysicsMonitoring/MuonInsituPerformance.h"
#include "MuonPhysicsMonitoring/MuTrackInsituPerformance.h"
#include "MuonPhysicsMonitoring/MuTrackPhysMonTool.h"
//#include "MuonPhysicsMonitoring/MuonInsituMonitoring.h"
//#include "MuonPhysicsMonitoring/MuonInsituResolution.h"

DECLARE_TOOL_FACTORY(ZmumuPhysMonTool)
DECLARE_TOOL_FACTORY(MuonInsituPerformance)
DECLARE_TOOL_FACTORY(MuTrackInsituPerformance)
DECLARE_TOOL_FACTORY(MuTrackPhysMonTool)
DECLARE_TOOL_FACTORY(JpsimumuPhysMonTool)
DECLARE_TOOL_FACTORY(JpsiMuonInsituPerformance)
//DECLARE_TOOL_FACTORY(MuonInsituMonitoring)
//DECLARE_TOOL_FACTORY(MuonInsituResolution)
   
DECLARE_FACTORY_ENTRIES(MuonPhysicsMonitoring)
{
    DECLARE_ALGTOOL(ZmumuPhysMonTool)
    DECLARE_ALGTOOL(MuonInsituPerformance)
    DECLARE_ALGTOOL(MuTrackInsituPerformance)
    DECLARE_ALGTOOL(MuTrackPhysMonTool)
    DECLARE_ALGTOOL(JpsimumuPhysMonTool)
    DECLARE_ALGTOOL(JpsiMuonInsituPerformance)
  //  DECLARE_ALGTOOL(MuonInsituMonitoring)
  //  DECLARE_ALGTOOL(MuonInsituResolution)   
}
