#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MboyView/MboyView.h"
#include "MboyView/MboyViewDigiMaker.h"
#include "MboyView/MboyViewServer.h"
#include "MboyView/MboyViewEventSelectionTool.h"
#include "MboyView/MboyViewGenEventSelectionTool.h"
#include "MboyView/MboyViewEventInfoTool.h"
#include "MboyView/MboyViewMuonRecDigiStoreTDSTool.h"
#include "MboyView/MboyViewTrackRecordCollectionTool.h"
#include "MboyView/MboyViewCaloTool.h"
#include "MboyView/MboyViewTrkTrackCollectionTool.h"
#include "MboyView/MboyViewTrackParticleContainerTool.h"
#include "MboyView/MboyViewCombinedMuonContainerTool.h"
#include "MboyView/MboyViewTrkSegmentCollectionTool.h"
#include "MboyView/MboyViewMcEventCollectionTool.h"
#include "MboyView/MboyViewEgammaContainerTool.h"
//apolici
#include "MboyView/MboyViewJetContainerTool.h"
#include "MboyView/MboyViewMETContainerTool.h"
#include "MboyView/MboyViewL1RoIContainerTool.h"
//apolici
#include "MboyView/MboyViewVertexContainerTool.h"
#include "MboyView/MboyViewIDRecDigiTool.h"
#include "MboyView/MboyViewPointerDumperTool.h"
#include "MboyView/MboyViewAGDD2GeoSwitchesTool.h"
#include "MboyView/MboyViewTrTrackDumpTool.h"
#include "MboyView/MboyViewDumpCombinedMuonContainerTool.h"
#include "MboyView/MboyViewTSOSDumpTool.h"

DECLARE_ALGORITHM_FACTORY( MboyView )
DECLARE_ALGORITHM_FACTORY( MboyViewDigiMaker )
DECLARE_ALGORITHM_FACTORY( MboyViewServer )
DECLARE_TOOL_FACTORY( MboyViewEventSelectionTool  )
DECLARE_TOOL_FACTORY( MboyViewGenEventSelectionTool  )
DECLARE_TOOL_FACTORY( MboyViewEventInfoTool  )
DECLARE_TOOL_FACTORY( MboyViewMuonRecDigiStoreTDSTool  )
DECLARE_TOOL_FACTORY( MboyViewTrackRecordCollectionTool  )
DECLARE_TOOL_FACTORY( MboyViewCaloTool  )
DECLARE_TOOL_FACTORY( MboyViewTrkTrackCollectionTool  )
DECLARE_TOOL_FACTORY( MboyViewTrackParticleContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewCombinedMuonContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewTrkSegmentCollectionTool  )
DECLARE_TOOL_FACTORY( MboyViewMcEventCollectionTool  )
//apolici
DECLARE_TOOL_FACTORY( MboyViewJetContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewMETContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewL1RoIContainerTool  )
//apolici
DECLARE_TOOL_FACTORY( MboyViewEgammaContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewVertexContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewIDRecDigiTool  )
DECLARE_TOOL_FACTORY( MboyViewPointerDumperTool  )
DECLARE_TOOL_FACTORY( MboyViewAGDD2GeoSwitchesTool  )
DECLARE_TOOL_FACTORY( MboyViewTrTrackDumpTool  )
DECLARE_TOOL_FACTORY( MboyViewDumpCombinedMuonContainerTool  )
DECLARE_TOOL_FACTORY( MboyViewTSOSDumpTool  )

DECLARE_FACTORY_ENTRIES( MboyView ) {
        DECLARE_ALGORITHM( MboyView );
	DECLARE_ALGORITHM( MboyViewDigiMaker );
	DECLARE_ALGORITHM( MboyViewServer );
        DECLARE_TOOL( MboyViewEventSelectionTool );
        DECLARE_TOOL( MboyViewGenEventSelectionTool );
        DECLARE_TOOL( MboyViewEventInfoTool );
        DECLARE_TOOL( MboyViewMuonRecDigiStoreTDSTool );
        DECLARE_TOOL( MboyViewTrackRecordCollectionTool );
        DECLARE_TOOL( MboyViewCaloTool );
        DECLARE_TOOL( MboyViewTrkTrackCollectionTool );
        DECLARE_TOOL( MboyViewTrackParticleContainerTool );
        DECLARE_TOOL( MboyViewCombinedMuonContainerTool );
        DECLARE_TOOL( MboyViewTrkSegmentCollectionTool );
        DECLARE_TOOL( MboyViewMcEventCollectionTool );
//apolici
        DECLARE_TOOL( MboyViewJetContainerTool );
        DECLARE_TOOL( MboyViewMETContainerTool );
        DECLARE_TOOL( MboyViewL1RoIContainerTool );
//apolici
        DECLARE_TOOL( MboyViewEgammaContainerTool );
        DECLARE_TOOL( MboyViewVertexContainerTool );
        DECLARE_TOOL( MboyViewIDRecDigiTool );
        DECLARE_TOOL( MboyViewPointerDumperTool );
        DECLARE_TOOL( MboyViewAGDD2GeoSwitchesTool );
        DECLARE_TOOL( MboyViewTrTrackDumpTool );
        DECLARE_TOOL( MboyViewDumpCombinedMuonContainerTool );
        DECLARE_TOOL( MboyViewTSOSDumpTool );
}                                                   
