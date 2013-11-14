#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MboyAthTool/MboyMooSegmentCombinationFinderTester.h"
#include "MboyAthTool/MboyMuonSegmentMakerTester.h"
#include "MboyAthTool/MboyMuonTrackBuilderTester.h"
#include "MboyAthTool/MuonDynamicLayerCreatorTester.h"
#include "MboyAthTool/MuonBackTrackerTester.h"
#include "MboyAthTool/MboyMuPatTrackBuilderTester.h"
#include "MboyAthTool/MboyCollectMatterTool.h"
#include "MboyAthTool/MuonDynamicLayerCreator.h"
#include "MboyAthTool/MboyMuonSegmentMaker.h"
#include "MboyAthTool/MboyMooSegmentCombinationFinder.h"
#include "MboyAthTool/MboyAthToolHelper.h"
#include "MboyAthTool/MboyDumpNativeSegmentTool.h"
#include "MboyAthTool/MboyDumpMboyStoreTool.h"
#include "MboyAthTool/MboyFullRecTool.h"
#include "MboyAthTool/MboyMuonTrackBuilder.h"
#include "MboyAthTool/MuonBackTracker.h"
#include "MboyAthTool/FindMonitor.h"
#include "MboyAthTool/RefineMonitor.h"

using namespace Trk;
using namespace Muon;

DECLARE_ALGORITHM_FACTORY( MboyMooSegmentCombinationFinderTester )
DECLARE_ALGORITHM_FACTORY( MboyMuonSegmentMakerTester )
DECLARE_ALGORITHM_FACTORY( MboyMuonTrackBuilderTester )
DECLARE_ALGORITHM_FACTORY( MuonDynamicLayerCreatorTester )
DECLARE_ALGORITHM_FACTORY( MuonBackTrackerTester )
DECLARE_ALGORITHM_FACTORY( MboyMuPatTrackBuilderTester )
DECLARE_TOOL_FACTORY( MboyCollectMatterTool  )
DECLARE_TOOL_FACTORY( MuonDynamicLayerCreator  )
DECLARE_TOOL_FACTORY( MboyMuonSegmentMaker  )
DECLARE_TOOL_FACTORY( MboyMooSegmentCombinationFinder  )
DECLARE_TOOL_FACTORY( MboyAthToolHelper  )
DECLARE_TOOL_FACTORY( MboyDumpNativeSegmentTool  )
DECLARE_TOOL_FACTORY( MboyDumpMboyStoreTool  )
DECLARE_TOOL_FACTORY( MboyFullRecTool  )
DECLARE_TOOL_FACTORY( MboyMuonTrackBuilder  )
DECLARE_TOOL_FACTORY( MuonBackTracker  )
DECLARE_TOOL_FACTORY( FindMonitor  )
DECLARE_TOOL_FACTORY( RefineMonitor  )


DECLARE_FACTORY_ENTRIES( MboyAthTool ) {
        DECLARE_ALGORITHM( MboyMooSegmentCombinationFinderTester );
        DECLARE_ALGORITHM( MboyMuonSegmentMakerTester );
        DECLARE_ALGORITHM( MboyMuonTrackBuilderTester );
        DECLARE_ALGORITHM( MuonDynamicLayerCreatorTester );
        DECLARE_ALGORITHM( MuonBackTrackerTester );
        DECLARE_ALGORITHM( MboyMuPatTrackBuilderTester );
        DECLARE_TOOL( MboyCollectMatterTool );
	DECLARE_TOOL( MuonDynamicLayerCreator );
        DECLARE_TOOL( MboyMuonSegmentMaker );
        DECLARE_TOOL( MboyMooSegmentCombinationFinder );
        DECLARE_TOOL( MboyAthToolHelper );
        DECLARE_TOOL( MboyDumpNativeSegmentTool );
        DECLARE_TOOL( MboyDumpMboyStoreTool );
        DECLARE_TOOL( MboyFullRecTool );
        DECLARE_TOOL( MboyMuonTrackBuilder );
        DECLARE_TOOL( MuonBackTracker );
        DECLARE_TOOL( FindMonitor );
        DECLARE_TOOL( RefineMonitor );
}                                                   
