#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonTrackPerformance/MuonTrackPerformanceAlg.h"
#include "../MuonPerformanceAlg.h"
#include "../MuonSegmentPerformanceAlg.h"
#include "../MuonRecoValidationTool.h"
#include "MuonTrackPerformance/MuonTrackStatisticsTool.h"
#include "MuonTrackPerformance/MuonTrackStatisticsAlg.h"

using namespace Muon;

DECLARE_ALGORITHM_FACTORY( MuonTrackPerformanceAlg )
DECLARE_ALGORITHM_FACTORY( MuonPerformanceAlg )
DECLARE_ALGORITHM_FACTORY( MuonSegmentPerformanceAlg )
DECLARE_ALGORITHM_FACTORY( MuonTrackStatisticsAlg )
DECLARE_TOOL_FACTORY( MuonTrackStatisticsTool )
DECLARE_TOOL_FACTORY( MuonRecoValidationTool )

DECLARE_FACTORY_ENTRIES( MuonTrackPerformance )
{
  DECLARE_ALGORITHM( MuonPerformanceAlg );
  DECLARE_ALGORITHM( MuonSegmentPerformanceAlg );
  DECLARE_ALGORITHM( MuonTrackPerformanceAlg );
  DECLARE_ALGORITHM( MuonTrackStatisticsAlg );
  DECLARE_TOOL( MuonTrackStatisticsTool );
  DECLARE_TOOL( MuonRecoValidationTool );
}
