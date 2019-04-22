#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MergeMcEventCollection.h"
#include "../MergeMcEventCollTool.h"
#include "../NewMergeMcEventCollTool.h"
#include "../SimpleMergeMcEventCollTool.h"
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
#include "../MergeMMSimHitColl.h"
#include "../MergeMMSimHitCollTool.h"
#include "../MergesTGCSimHitColl.h"
#include "../MergesTGCSimHitCollTool.h"

DECLARE_ALGORITHM_FACTORY( MergeMcEventCollection )
DECLARE_ALGORITHM_FACTORY( MergeTrackRecordCollection )
DECLARE_ALGORITHM_FACTORY( MergeCalibHits )
DECLARE_ALGORITHM_FACTORY( MergeHijingPars )
DECLARE_ALGORITHM_FACTORY( MergeTruthJets )
DECLARE_ALGORITHM_FACTORY( MergeRecoTimingObj )
DECLARE_ALGORITHM_FACTORY( MergeGenericMuonSimHitColl )
DECLARE_ALGORITHM_FACTORY( MergeMMSimHitColl )
DECLARE_ALGORITHM_FACTORY( MergesTGCSimHitColl )
DECLARE_TOOL_FACTORY( MergeMcEventCollTool )
DECLARE_TOOL_FACTORY( NewMergeMcEventCollTool )
DECLARE_TOOL_FACTORY( SimpleMergeMcEventCollTool )
DECLARE_TOOL_FACTORY( MergeTrackRecordCollTool )
DECLARE_TOOL_FACTORY( MergeCalibHitsTool )
DECLARE_TOOL_FACTORY( MergeHijingParsTool )
DECLARE_TOOL_FACTORY( MergeTruthJetsTool )
DECLARE_TOOL_FACTORY( MergeRecoTimingObjTool )
DECLARE_TOOL_FACTORY( MergeGenericMuonSimHitCollTool )
DECLARE_TOOL_FACTORY( MergeMMSimHitCollTool )
DECLARE_TOOL_FACTORY( MergesTGCSimHitCollTool )
