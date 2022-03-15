#include "../FastTrackFinderLRTHypoTool.h"
#include "../FastTrackFinderLRTHypoAlg.h"
#include "../TrigIsoHPtTrackTriggerHypoAlg.h"
#include "../TrigIsoHPtTrackTriggerHypoTool.h"
#include "../TrigdEdxTrackHypoAlg.h"
#include "../TrigdEdxTrackHypoTool.h"
#include "../TrigHitDVHypoAlg.h"
#include "../TrigHitDVHypoTool.h"
#include "../TrigDisappearingTrackHypoAlg.h"
#include "../TrigDisappearingTrackHypoTool.h"

#include "../../TrigLongLivedParticlesHypo/MuonClusterHypoAlg.h"
#include "../../TrigLongLivedParticlesHypo/MuonClusterHypoTool.h"

#include "../DisplacedJetPromptHypoTool.h"
#include "../DisplacedJetPromptHypoAlg.h"

#include "../DisplacedJetDispHypoTool.h"
#include "../DisplacedJetDispHypoAlg.h"

#include "../DisplacedJetEventDecisionHypoAlg.h"
#include "../DisplacedJetEventDecisionHypoTool.h"

DECLARE_COMPONENT( TrigDisappearingTrackHypoAlg )
DECLARE_COMPONENT( TrigDisappearingTrackHypoTool )
DECLARE_COMPONENT( TrigHitDVHypoAlg )
DECLARE_COMPONENT( TrigHitDVHypoTool )
DECLARE_COMPONENT( TrigdEdxTrackHypoAlg )
DECLARE_COMPONENT( TrigdEdxTrackHypoTool )
DECLARE_COMPONENT( TrigIsoHPtTrackTriggerHypoAlg )
DECLARE_COMPONENT( TrigIsoHPtTrackTriggerHypoTool )
DECLARE_COMPONENT( FastTrackFinderLRTHypoTool )
DECLARE_COMPONENT( FastTrackFinderLRTHypoAlg )

DECLARE_COMPONENT( DisplacedJetDispHypoAlg )
DECLARE_COMPONENT( DisplacedJetDispHypoTool )
DECLARE_COMPONENT( DisplacedJetPromptHypoAlg )
DECLARE_COMPONENT( DisplacedJetPromptHypoTool )
DECLARE_COMPONENT( DisplacedJetEventDecisionHypoAlg )
DECLARE_COMPONENT( DisplacedJetEventDecisionHypoTool )
DECLARE_COMPONENT( MuonClusterHypoAlg )
DECLARE_COMPONENT( MuonClusterHypoTool )
