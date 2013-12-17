#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibStandAloneTools/NtupleMTT0Tool.h"
#include "MuonCalibStandAloneTools/NtupleCalibADCTool.h"
#include "MuonCalibStandAloneTools/NtupleClassicT0Tool.h"
#include "MuonCalibStandAloneTools/NtupleIntegrationTool.h"
#include "MuonCalibStandAloneTools/NtupleAnalyticAutocalibrationTool.h"
#include "MuonCalibStandAloneTools/NtupleSimpleresolutionTool.h"
#include "MuonCalibStandAloneTools/NtupleChisqResolutionTool.h"
#include "MuonCalibStandAloneTools/NtupleTubeEfficiencyTool.h"
#include "MuonCalibStandAloneTools/NtupleRunScanTool.h"
#include "MuonCalibStandAloneTools/NtupleControlHistogramsTool.h"
#include "MuonCalibStandAloneTools/NtupleResidualTool.h"
#include "MuonCalibStandAloneTools/NtupleDisplayTool.h"
#include "MuonCalibStandAloneTools/NtupleWireCentricityTool.h"
#include "MuonCalibStandAloneTools/NtupleCurvedAutocalibrationTool.h"
#include "MuonCalibStandAloneTools/NtupleDBCheckTool.h"
#include "MuonCalibStandAloneTools/NtupleResidualVsTimeTool.h"

using namespace MuonCalib;

DECLARE_TOOL_FACTORY ( NtupleMTT0Tool ) 
DECLARE_TOOL_FACTORY ( NtupleCalibADCTool ) 
DECLARE_TOOL_FACTORY ( NtupleClassicT0Tool ) 
DECLARE_TOOL_FACTORY ( NtupleIntegrationTool ) 
DECLARE_TOOL_FACTORY ( NtupleAnalyticAutocalibrationTool )
DECLARE_TOOL_FACTORY ( NtupleChisqResolutionTool )
DECLARE_TOOL_FACTORY ( NtupleSimpleResolutionTool )
DECLARE_TOOL_FACTORY ( NtupleTubeEfficiencyTool )
DECLARE_TOOL_FACTORY ( NtupleRunScanTool )
DECLARE_TOOL_FACTORY ( NtupleControlHistogramsTool )
DECLARE_TOOL_FACTORY ( NtupleResidualTool )
DECLARE_TOOL_FACTORY ( NtupleDisplayTool )
DECLARE_TOOL_FACTORY ( NtupleWireCentricityTool )
DECLARE_TOOL_FACTORY ( NtupleCurvedAutocalibrationTool )
DECLARE_TOOL_FACTORY ( NtupleDbCheckTool )
DECLARE_TOOL_FACTORY ( NtupleResidualVsTimeTool )

DECLARE_FACTORY_ENTRIES(MuonCalibStandAloneTools) {
  DECLARE_TOOL (NtupleMTT0Tool)
  DECLARE_TOOL (NtupleCalibADCTool)
  DECLARE_TOOL (NtupleClassicT0Tool)
  DECLARE_TOOL (NtupleIntegrationTool)
  DECLARE_TOOL (NtupleAnalyticAutocalibrationTool)
  DECLARE_TOOL ( NtupleClassicAutocalibrationTool )
  DECLARE_TOOL (NtupleChisqResolutionTool)
  DECLARE_TOOL (NtupleSimpleResolutionTool)
  DECLARE_TOOL (NtupleTubeEfficiencyTool)
  DECLARE_TOOL (NtupleRunScanTool)
  DECLARE_TOOL (NtupleControlHistogramsTool)
  DECLARE_TOOL (NtupleResidualTool)
  DECLARE_TOOL (NtupleDisplayTool)
  DECLARE_TOOL (NtupleWireCentricityTool)
  DECLARE_TOOL (NtupleCurvedAutocalibrationTool)
  DECLARE_TOOL (NtupleDbCheckTool)
  DECLARE_TOOL (NtupleResidualVsTimeTool)
}

