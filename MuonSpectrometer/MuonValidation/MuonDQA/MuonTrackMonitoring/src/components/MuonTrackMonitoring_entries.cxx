#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonTrackMonitoring/MuonGenericTracksMon.h"
#include "MuonTrackMonitoring/MuonSelectedTracksMon.h"
#include "MuonTrackMonitoring/TGCStandaloneTracksMon.h"
#include "MuonTrackMonitoring/RPCStandaloneTracksMon.h"
 
DECLARE_TOOL_FACTORY(MuonGenericTracksMon)
DECLARE_TOOL_FACTORY(MuonSelectedTracksMon)
DECLARE_TOOL_FACTORY(TGCStandaloneTracksMon)
DECLARE_TOOL_FACTORY(RPCStandaloneTracksMon)
 
DECLARE_FACTORY_ENTRIES(MuonTrackMonitoring)
{
   DECLARE_ALGTOOL(MuonGenericTracksMon)   
   DECLARE_ALGTOOL(MuonSelectedTracksMon)
   DECLARE_ALGTOOL(TGCStandaloneTracksMon)
   DECLARE_ALGTOOL(RPCStandaloneTracksMon)
} 

