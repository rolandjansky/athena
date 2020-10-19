#include "../DumpDecisions.h"
#include "../RoRSeqFilter.h"
#include "../PassFilter.h"
#include "../TriggerSummaryAlg.h"
#include "DecisionHandling/ComboHypo.h"
#include "../InputMakerForRoI.h"
#include "../DeltaRRoIComboHypoTool.h"
#include "DecisionHandling/ComboHypoToolBase.h"

#include "../ViewCreatorInitialROITool.h"
#include "../ViewCreatorPreviousROITool.h"
#include "../ViewCreatorNamedROITool.h"
#include "../ViewCreatorFSROITool.h"
#include "../ViewCreatorFetchFromViewROITool.h"
#include "../ViewCreatorCentredOnIParticleROITool.h"
#include "../ViewCreatorCentredOnClusterROITool.h"
#include "../ViewCreatorCentredOnJetWithPVConstraintROITool.h"
#include "../ITestHypoTool.h"
#include "../TestHypoAlg.h"
#include "../TestHypoTool.h"
#include "../TestRecoAlg.h"
#include "../TestInputMaker.h"


DECLARE_COMPONENT( HLTTest::TestInputMaker )
DECLARE_COMPONENT( HLTTest::TestHypoAlg )
DECLARE_COMPONENT( HLTTest::TestHypoTool )
DECLARE_COMPONENT( HLTTest::TestRecoAlg )

DECLARE_COMPONENT( DumpDecisions )
DECLARE_COMPONENT( RoRSeqFilter )
DECLARE_COMPONENT( PassFilter )
DECLARE_COMPONENT( TriggerSummaryAlg )
DECLARE_COMPONENT( ComboHypo )
DECLARE_COMPONENT( InputMakerForRoI )
DECLARE_COMPONENT( ComboHypoToolBase )
DECLARE_COMPONENT( DeltaRRoIComboHypoTool )

DECLARE_COMPONENT( ViewCreatorInitialROITool )
DECLARE_COMPONENT( ViewCreatorPreviousROITool )
DECLARE_COMPONENT( ViewCreatorNamedROITool )
DECLARE_COMPONENT( ViewCreatorFSROITool )
DECLARE_COMPONENT( ViewCreatorFetchFromViewROITool )
DECLARE_COMPONENT( ViewCreatorCentredOnIParticleROITool )
DECLARE_COMPONENT( ViewCreatorCentredOnClusterROITool )
DECLARE_COMPONENT( ViewCreatorCentredOnJetWithPVConstraintROITool )



