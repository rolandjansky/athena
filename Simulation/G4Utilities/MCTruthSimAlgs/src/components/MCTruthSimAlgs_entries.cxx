#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MergeMcEventCollection.h"
#include "../MergeMcEventCollTool.h"
#include "../MergeTrackRecordCollection.h"
#include "../MergeTrackRecordCollTool.h"
#include "../MergeCalibHits.h"
#include "../MergeHijingPars.h"
#include "../MergeHijingParsTool.h"
#include "../MergeCalibHitsTool.h"
#include "../MergeTruthJets.h"
#include "../MergeTruthJetsTool.h"
#include "../MergeRecoTimingObj.h"
#include "../MergeRecoTimingObjTool.h"
#include "../MergeGenericMuonSimHitColl.h"
#include "../MergeGenericMuonSimHitCollTool.h"


DECLARE_ALGORITHM_FACTORY( MergeMcEventCollection )
DECLARE_ALGORITHM_FACTORY( MergeTrackRecordCollection )
DECLARE_ALGORITHM_FACTORY( MergeCalibHits )
DECLARE_ALGORITHM_FACTORY( MergeHijingPars )
DECLARE_ALGORITHM_FACTORY( MergeTruthJets )
DECLARE_ALGORITHM_FACTORY( MergeRecoTimingObj )
DECLARE_ALGORITHM_FACTORY( MergeGenericMuonSimHitColl )
DECLARE_TOOL_FACTORY( MergeMcEventCollTool )
DECLARE_TOOL_FACTORY( MergeTrackRecordCollTool )
DECLARE_TOOL_FACTORY( MergeCalibHitsTool )
DECLARE_TOOL_FACTORY( MergeHijingParsTool )
DECLARE_TOOL_FACTORY( MergeTruthJetsTool )
DECLARE_TOOL_FACTORY( MergeRecoTimingObjTool )
DECLARE_TOOL_FACTORY( MergeGenericMuonSimHitCollTool )

DECLARE_FACTORY_ENTRIES(MCTruthSimAlgs)
{
                DECLARE_ALGORITHM( MergeMcEventCollection )
                DECLARE_ALGORITHM(MergeTrackRecordCollection)
                DECLARE_ALGORITHM(MergeCalibHits)
                DECLARE_ALGORITHM(MergeHijingPars)
                DECLARE_ALGORITHM(MergeTruthJets)
                DECLARE_ALGORITHM(MergeRecoTimingObj)
                DECLARE_ALGORITHM(MergeGenericMuonSimHitColl)
                DECLARE_TOOL( MergeMcEventCollTool )
                DECLARE_TOOL( MergeTrackRecordCollTool )
                DECLARE_TOOL( MergeCalibHitsTool )
                DECLARE_TOOL( MergeHijingParsTool )
                DECLARE_TOOL( MergeTruthJetsTool )
                DECLARE_TOOL( MergeRecoTimingObjTool )
                DECLARE_TOOL( MergeGenericMuonSimHitCollTool )
}
