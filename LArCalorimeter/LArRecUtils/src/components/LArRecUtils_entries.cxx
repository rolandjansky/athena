#include "LArRecUtils/LArADC2MeVTool.h"
#include "LArRecUtils/LArAutoCorrNoiseTool.h"
#include "LArRecUtils/LArAutoCorrTotalTool.h"
#include "LArRecUtils/LArCellFakeProbElectronics.h"
//#include "LArRecUtils/LArCellFakeProbHV.h"
#include "LArRecUtils/LArHVCorrTool.h"
//#include "LArRecUtils/LArHVGeometryTool.h"
#include "LArRecUtils/LArOFCTool.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArRecUtils/LArShapePeakRecoTool.h"
#include "LArRecUtils/LArTowerBuilderTool.h"
#include "LArRecUtils/LArFCalTowerBuilderTool.h"
#include "LArRecUtils/LArFEBConfigReader.h"
#include "LArRecUtils/LArFlatConditionSvc.h"
#include "../LArFCalTowerBuilderToolTestAlg.h"
#include "../LArHVScaleRetriever.h"
#include "../LArFlatConditionsAlg.h"
#include "../LArOnOffMappingAlg.h"
#include "../LArCalibLineMappingAlg.h"
#include "../LArFebRodMappingAlg.h"
#include "../LArSymConditionsAlg.h"
#include "../LArMCSymCondAlg.h"

DECLARE_COMPONENT( LArADC2MeVTool )
DECLARE_COMPONENT( LArAutoCorrNoiseTool )
DECLARE_COMPONENT( LArAutoCorrTotalTool )
DECLARE_COMPONENT( LArCellFakeProbElectronics )
//DECLARE_COMPONENT( LArCellFakeProbHV )
DECLARE_COMPONENT( LArHVCorrTool )
//DECLARE_COMPONENT( LArHVGeometryTool )
DECLARE_COMPONENT( LArOFCTool )
DECLARE_COMPONENT( LArOFPeakRecoTool )
DECLARE_COMPONENT( LArParabolaPeakRecoTool )
DECLARE_COMPONENT( LArShapePeakRecoTool )
DECLARE_COMPONENT( LArTowerBuilderTool )
DECLARE_COMPONENT( LArFCalTowerBuilderTool )
DECLARE_COMPONENT( LArFEBConfigReader )
DECLARE_COMPONENT( LArFlatConditionSvc )
DECLARE_COMPONENT( LArFCalTowerBuilderToolTestAlg )
DECLARE_COMPONENT( LArHVScaleRetriever )
DECLARE_COMPONENT( LArCondAlgAutoCorrSC )
DECLARE_COMPONENT( LArCondAlgDAC2uAFlat )
DECLARE_COMPONENT( LArCondAlgDAC2uASC )
DECLARE_COMPONENT( LArCondAlgHVScaleCorrFlat )
DECLARE_COMPONENT( LArCondAlgHVScaleCorrSC )
DECLARE_COMPONENT( LArCondAlgMinBiasSC )
DECLARE_COMPONENT( LArCondAlgMphysOverMcalFlat )
DECLARE_COMPONENT( LArCondAlgMphysOverMcalSC )
DECLARE_COMPONENT( LArCondAlgNoiseSC )
DECLARE_COMPONENT( LArCondAlgOFCFlat )
DECLARE_COMPONENT( LArCondAlgPedestalFlat )
DECLARE_COMPONENT( LArCondAlgPedestalSC )
DECLARE_COMPONENT( LArCondAlgRampFlat )
DECLARE_COMPONENT( LArCondAlgRampSC )
DECLARE_COMPONENT( LArCondAlgShapeFlat )
DECLARE_COMPONENT( LArCondAlgShapeSC )
DECLARE_COMPONENT( LArCondAlgfSamplSC )
DECLARE_COMPONENT( LArCondAlguA2MeVFlat )
DECLARE_COMPONENT( LArCondAlguA2MeVSC )
DECLARE_COMPONENT( LArOnOffMappingAlg )
DECLARE_COMPONENT( LArCalibLineMappingAlg )
DECLARE_COMPONENT( LArFebRodMappingAlg )
DECLARE_COMPONENT( LArMCSymCondAlg )

DECLARE_COMPONENT( LArRampSymCondAlg )
DECLARE_COMPONENT( LAruA2MeVSymCondAlg )
DECLARE_COMPONENT( LArDAC2uASymCondAlg )
DECLARE_COMPONENT( LArfSamplSymCondAlg )
DECLARE_COMPONENT( LArMinBiasSymCondAlg )
DECLARE_COMPONENT( LArNoiseSymCondAlg )

