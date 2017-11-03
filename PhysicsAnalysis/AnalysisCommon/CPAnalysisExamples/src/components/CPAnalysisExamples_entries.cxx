#include "CPAnalysisExamples/MuonEfficiencyToolExample.h"
#include "CPAnalysisExamples/MuonSmearingToolExample.h"
#include "CPAnalysisExamples/MuonSelectionToolExample.h"
#include "CPAnalysisExamples/JetCalibrationToolExample.h"

#include "../ToolTester.h"

DECLARE_COMPONENT( CP::MuonEfficiencyToolExample )
DECLARE_COMPONENT( CP::MuonSmearingToolExample )
DECLARE_COMPONENT( CP::MuonSelectionToolExample )
DECLARE_COMPONENT( CP::JetCalibrationToolExample )

DECLARE_COMPONENT( CP::ToolTester )


#include "CPAnalysisExamples/xAthExample.h"
DECLARE_COMPONENT( xAthExample )


#include "../Test__TrackLinkTest.h"
DECLARE_COMPONENT( Test::TrackLinkTest )

#include "CPAnalysisExamples/ToolExamplesAlg.h"
DECLARE_COMPONENT( ToolExamplesAlg )

#include "../Test__EgammaCalibrationAndSmearingTool.h"
DECLARE_COMPONENT( Test::EgammaCalibrationAndSmearingTool )


#include "CPAnalysisExamples/MetadataToolExample.h"
DECLARE_COMPONENT( CP::MetadataToolExample )

