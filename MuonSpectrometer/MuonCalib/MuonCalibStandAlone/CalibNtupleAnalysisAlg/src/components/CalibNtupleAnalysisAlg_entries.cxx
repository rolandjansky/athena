#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CalibNtupleAnalysisAlg/CalibNtupleAnalysisAlg2.h"
#include "CalibNtupleAnalysisAlg/BarrelSecondCoordinatePreparationTool.h"
#include "CalibNtupleAnalysisAlg/BFieldCorrectionUpdateTool.h"
#include "CalibNtupleAnalysisAlg/WriteMdtGeometry.h"
#include "CalibNtupleAnalysisAlg/UpdateBField.h"
#include "CalibNtupleAnalysisAlg/CalibNtupleLoader.h"
#include "CalibNtupleAnalysisAlg/SegmentRegionSelector.h"
#include "CalibNtupleAnalysisAlg/SegmentRawdataSelector.h"
#include "CalibNtupleAnalysisAlg/SegmentRecalibration.h"
#include "CalibNtupleAnalysisAlg/SegmentRefitter.h"
#include "CalibNtupleAnalysisAlg/InitialDqFilter.h"
//#include "CalibNtupleAnalysisAlg/RpcTimingCorr.h"
#include "CalibNtupleAnalysisAlg/ApplyRawTimes.h"
#include "CalibNtupleAnalysisAlg/GoodRunListFilter.h"
#include "CalibNtupleAnalysisAlg/MuonCalibGeometryDumper.h"


using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY ( CalibNtupleAnalysisAlg2 )
DECLARE_ALGORITHM_FACTORY ( WriteMdtGeometry )
DECLARE_ALGORITHM_FACTORY ( MuonCalibGeometryDumper )

DECLARE_FACTORY_ENTRIES(CalibNtupleAnalysisAlg) {
  DECLARE_ALGORITHM(CalibNtupleAnalysisAlg2)
}

DECLARE_FACTORY_ENTRIES(WriteMdtGeometry) {
  DECLARE_ALGORITHM(WriteMdtGeometry)
}

DECLARE_FACTORY_ENTRIES(MuonCalibGeometryDumper) {
  DECLARE_ALGORITHM(MuonCalibGeometryDumper)
}

DECLARE_TOOL_FACTORY ( BarrelSecondCoordinatePreparationTool )
DECLARE_TOOL_FACTORY ( BFieldCorrectionUpdateTool )
DECLARE_TOOL_FACTORY ( UpdateBField )
DECLARE_TOOL_FACTORY (CalibNtupleLoader)
DECLARE_TOOL_FACTORY (SegmentRegionSelector)
DECLARE_TOOL_FACTORY (SegmentRawdataSelector)
DECLARE_TOOL_FACTORY (SegmentRecalibration)
DECLARE_TOOL_FACTORY (InitialDqFilter)
DECLARE_TOOL_FACTORY (SegmentRefitter)
//DECLARE_TOOL_FACTORY (RpcTimingCorr)
DECLARE_TOOL_FACTORY (ApplyRawTimes)
DECLARE_TOOL_FACTORY (GoodRunListFilter)


DECLARE_FACTORY_ENTRIES( CalibSegmentPreparationTools ) {
  DECLARE_TOOL (BarrelSecondCoordinatePreparationTool)
  DECLARE_TOOL (BFieldCorrectionUpdateTool)
  DECLARE_TOOL (UpdateBField)
  DECLARE_TOOL (CalibNtupleLoader)
  DECLARE_TOOL (SegmentRegionSelector)
  DECLARE_TOOL (SegmentRawdataSelector)
  DECLARE_TOOL (SegmentRecalibration)
  DECLARE_TOOL (InitialDqFilter)
  DECLARE_TOOL (SegmentRefitter)
//  DECLARE_TOOL (RpcTimingCorr)
  DECLARE_TOOL (ApplyRawTimes)
  DECLARE_TOOL (GoodRunListFilter)
}
