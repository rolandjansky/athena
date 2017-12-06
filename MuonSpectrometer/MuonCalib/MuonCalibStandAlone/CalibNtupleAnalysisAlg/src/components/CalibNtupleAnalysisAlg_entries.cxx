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

DECLARE_COMPONENT( CalibNtupleAnalysisAlg2 )
DECLARE_COMPONENT( WriteMdtGeometry )
DECLARE_COMPONENT( MuonCalibGeometryDumper )




DECLARE_COMPONENT( BarrelSecondCoordinatePreparationTool )
DECLARE_COMPONENT( BFieldCorrectionUpdateTool )
DECLARE_COMPONENT( UpdateBField )
DECLARE_COMPONENT( CalibNtupleLoader )
DECLARE_COMPONENT( SegmentRegionSelector )
DECLARE_COMPONENT( SegmentRawdataSelector )
DECLARE_COMPONENT( SegmentRecalibration )
DECLARE_COMPONENT( InitialDqFilter )
DECLARE_COMPONENT( SegmentRefitter )
//DECLARE_COMPONENT( RpcTimingCorr )
DECLARE_COMPONENT( ApplyRawTimes )
DECLARE_COMPONENT( GoodRunListFilter )

