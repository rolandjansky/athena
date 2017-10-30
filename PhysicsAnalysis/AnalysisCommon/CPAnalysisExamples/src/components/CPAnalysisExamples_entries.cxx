#include "CPAnalysisExamples/MuonEfficiencyToolExample.h"
#include "CPAnalysisExamples/MuonSmearingToolExample.h"
#include "CPAnalysisExamples/MuonSelectionToolExample.h"
#include "CPAnalysisExamples/JetCalibrationToolExample.h"

#include "../ToolTester.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonEfficiencyToolExample )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonSmearingToolExample )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, MuonSelectionToolExample )
DECLARE_NAMESPACE_TOOL_FACTORY( CP, JetCalibrationToolExample )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, ToolTester )


#include "CPAnalysisExamples/xAthExample.h"
DECLARE_ALGORITHM_FACTORY( xAthExample )


#include "../Test__TrackLinkTest.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Test, TrackLinkTest )

#include "CPAnalysisExamples/ToolExamplesAlg.h"
DECLARE_ALGORITHM_FACTORY( ToolExamplesAlg )

#include "../Test__EgammaCalibrationAndSmearingTool.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Test, EgammaCalibrationAndSmearingTool )


#include "CPAnalysisExamples/MetadataToolExample.h"
DECLARE_NAMESPACE_TOOL_FACTORY( CP, MetadataToolExample )

