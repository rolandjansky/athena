// $Id: CPAnalysisExamples_entries.cxx 299890 2014-03-29 08:54:38Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
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

DECLARE_FACTORY_ENTRIES( CPAnalysisExamples ) {
  DECLARE_ALGORITHM( ToolExamplesAlg );
  DECLARE_NAMESPACE_TOOL( CP, MetadataToolExample );
  DECLARE_NAMESPACE_ALGORITHM(Test, EgammaCalibrationAndSmearingTool );
  DECLARE_NAMESPACE_ALGORITHM(Test, TrackLinkTest );
  DECLARE_ALGORITHM( xAthExample );

   DECLARE_NAMESPACE_TOOL( CP, MuonEfficiencyToolExample )
   DECLARE_NAMESPACE_TOOL( CP, MuonSmearingToolExample )
   DECLARE_NAMESPACE_TOOL( CP, MuonSelectionToolExample )
   DECLARE_NAMESPACE_TOOL( CP, JetCalibrationToolExample )

   DECLARE_NAMESPACE_ALGORITHM( CP, ToolTester )

}
