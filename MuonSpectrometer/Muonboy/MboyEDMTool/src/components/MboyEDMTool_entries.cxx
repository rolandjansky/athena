#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MboyEDMTool/MboyEDMHelper.h"
#include "MboyEDMTool/MboyNativeHelper.h"
#include "MboyEDMTool/MboyBFTool.h"
#include "MboyEDMTool/MboyRTTool.h"
#include "MboyEDMTool/MboyGenTrackTool.h"
#include "MboyEDMTool/MuonboyToTrackTool.h"
#include "MboyEDMTool/MuonboyToSegmentTool.h"
#include "MboyEDMTool/MuonboyToParticleTool.h"
#include "MboyEDMTool/MboyDumpTrackCollectionTool.h"
#include "MboyEDMTool/MboyDumpSegmentCollectionTool.h"
#include "MboyEDMTool/MboyDumpTrackParticleContainerTool.h"
#include "MboyEDMTool/MboyPrepDataToDigiStoreTool.h"
#include "MboyEDMTool/MboyGetPrepDataFromEventTool.h"
#include "MboyEDMTool/AdjustableT0Tool.h"
#include "MboyEDMTool/MuonboyToTrackParametersTool.h"
#include "MboyEDMTool/MuonboyToIdentifier.h"
#include "MboyEDMTool/MuonboyToSurfaceTool.h"
#include "MboyEDMTool/MuonboyToLayer.h"
#include "MboyEDMTool/MuonboyToMaterialEffectsOnTrack.h"
#include "MboyEDMTool/MuonSegmentToDratTool.h"
#include "MboyEDMTool/TrackToProtoTrackTool.h"
#include "MboyEDMTool/MboyDetectorLimitsTool.h"
#include "MboyEDMTool/MboyTrackUndressingTool.h"
#include "MboyEDMTool/MboySortingTool.h"
#include "MboyEDMTool/MeasurementBaseToDratTool.h"
#include "MboyEDMTool/MboyTrackRepresentationTransformTool.h"
#include "MboyEDMTool/MboyProperTrackRepresentationTransformTool.h"
#include "MboyEDMTool/MbMuScToTSOSTool.h"

using namespace Trk;
using namespace Muon;
using namespace AdjT0;

DECLARE_TOOL_FACTORY( MboyEDMHelper  )
DECLARE_TOOL_FACTORY( MboyNativeHelper  )
DECLARE_TOOL_FACTORY( MboyBFTool  )
DECLARE_TOOL_FACTORY( MboyRTTool  )
DECLARE_TOOL_FACTORY( MboyGenTrackTool  )
DECLARE_TOOL_FACTORY( MuonboyToTrackTool  )
DECLARE_TOOL_FACTORY( MuonboyToSegmentTool  )
DECLARE_TOOL_FACTORY( MuonboyToParticleTool  )
DECLARE_TOOL_FACTORY( MboyDumpTrackCollectionTool  )
DECLARE_TOOL_FACTORY( MboyDumpSegmentCollectionTool  )
DECLARE_TOOL_FACTORY( MboyDumpTrackParticleContainerTool  )
DECLARE_TOOL_FACTORY( MboyPrepDataToDigiStoreTool  )
DECLARE_TOOL_FACTORY( MboyGetPrepDataFromEventTool  )
DECLARE_TOOL_FACTORY( AdjustableT0Tool  )
DECLARE_TOOL_FACTORY( MuonboyToTrackParametersTool  )
DECLARE_TOOL_FACTORY( MuonboyToIdentifier  )
DECLARE_TOOL_FACTORY( MuonboyToSurfaceTool  )
DECLARE_TOOL_FACTORY( MuonboyToLayer  )
DECLARE_TOOL_FACTORY( MuonboyToMaterialEffectsOnTrack  )
DECLARE_TOOL_FACTORY( MuonSegmentToDratTool  )
DECLARE_TOOL_FACTORY( TrackToProtoTrackTool  )
DECLARE_TOOL_FACTORY( MboyDetectorLimitsTool  )
DECLARE_TOOL_FACTORY( MboyTrackUndressingTool  )
DECLARE_TOOL_FACTORY( MboySortingTool  )
DECLARE_TOOL_FACTORY( MeasurementBaseToDratTool  )
DECLARE_TOOL_FACTORY( MboyTrackRepresentationTransformTool  )
DECLARE_TOOL_FACTORY( MboyProperTrackRepresentationTransformTool  )
DECLARE_TOOL_FACTORY( MbMuScToTSOSTool  )

DECLARE_FACTORY_ENTRIES( MboyEDMTool ) {
        DECLARE_TOOL( MboyEDMHelper );
        DECLARE_TOOL( MboyNativeHelper );
        DECLARE_TOOL( MboyBFTool );
        DECLARE_TOOL( MboyRTTool );
        DECLARE_TOOL( MboyGenTrackTool );
        DECLARE_TOOL( MuonboyToTrackTool );
        DECLARE_TOOL( MuonboyToSegmentTool );
        DECLARE_TOOL( MuonboyToParticleTool );
        DECLARE_TOOL( MboyDumpTrackCollectionTool );
        DECLARE_TOOL( MboyDumpSegmentCollectionTool );
        DECLARE_TOOL( MboyDumpTrackParticleContainerTool );
        DECLARE_TOOL( MboyPrepDataToDigiStoreTool );
        DECLARE_TOOL( MboyGetPrepDataFromEventTool );
        DECLARE_TOOL( AdjustableT0Tool );
        DECLARE_TOOL( MuonboyToTrackParametersTool );
        DECLARE_TOOL( MuonboyToIdentifier );
        DECLARE_TOOL( MuonboyToSurfaceTool );
        DECLARE_TOOL( MuonboyToLayer );
        DECLARE_TOOL( MuonboyToMaterialEffectsOnTrack );
        DECLARE_TOOL( MuonSegmentToDratTool );
        DECLARE_TOOL( TrackToProtoTrackTool );
        DECLARE_TOOL( MboyDetectorLimitsTool );
        DECLARE_TOOL( MboyTrackUndressingTool );
        DECLARE_TOOL( MboySortingTool );
        DECLARE_TOOL( MeasurementBaseToDratTool );
        DECLARE_TOOL( MboyTrackRepresentationTransformTool );
        DECLARE_TOOL( MboyProperTrackRepresentationTransformTool );
        DECLARE_TOOL( MbMuScToTSOSTool );
}                                                   
