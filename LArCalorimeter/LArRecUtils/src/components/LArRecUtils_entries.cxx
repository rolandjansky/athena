#include "../LArADC2MeVTool.h"
#include "../LArAutoCorrNoiseTool.h"
#include "../LArAutoCorrTotalTool.h"
#include "../LArCellFakeProbElectronics.h"
//#include "LArRecUtils/LArCellFakeProbHV.h"
#include "../LArHVCorrTool.h"
//#include "LArRecUtils/LArHVGeometryTool.h"
#include "../LArOFCTool.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArRecUtils/LArShapePeakRecoTool.h"
#include "../LArTowerBuilderTool.h"
#include "../LArFCalTowerBuilderTool.h"
#include "../LArFlatConditionSvc.h"
#include "../LArFCalTowerBuilderToolTestAlg.h"
#include "../LArFlatConditionsAlg.h"
#include "../LArOnOffMappingAlg.h"
#include "../LArCalibLineMappingAlg.h"
#include "../LArFebRodMappingAlg.h"
#include "../LArHVIdMappingAlg.h"
#include "../LArSymConditionsAlg.h"
#include "../LArMCSymCondAlg.h"
#include "../LArADC2MeVCondAlg.h"
#include "../LArAutoCorrTotalCondAlg.h"
#include "../LArOFCCondAlg.h"
#include "../LArHVPathologyDbCondAlg.h"
#include "../LArHVIdMappingAlg.h"
#include "../LArHVCondAlg.h"
#include "../LArHVScaleCorrCondAlg.h"
#include "../LArAutoCorrNoiseCondAlg.h"
#include "../LArFEBConfigCondAlg.h"


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
DECLARE_COMPONENT( LArFlatConditionSvc )
DECLARE_COMPONENT( LArFCalTowerBuilderToolTestAlg )
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
DECLARE_COMPONENT( LArMinBiasAverageSymCondAlg )
DECLARE_COMPONENT( LArNoiseSymCondAlg )
DECLARE_COMPONENT( LArShapeSymCondAlg )
DECLARE_COMPONENT( LArAutoCorrSymCondAlg )
DECLARE_COMPONENT( LArMphysOverMcalSymCondAlg ) 

DECLARE_COMPONENT( LArAutoCorrTotalCondAlg )
DECLARE_COMPONENT( LArADC2MeVCondAlg )
DECLARE_COMPONENT( LArHVPathologyDbCondAlg )
DECLARE_COMPONENT( LArHVIdMappingAlg )
DECLARE_COMPONENT( LArOFCCondAlg )
DECLARE_COMPONENT( LArHVCondAlg )
DECLARE_COMPONENT( LArHVScaleCorrCondAlg )
DECLARE_COMPONENT( LArAutoCorrNoiseCondAlg )
DECLARE_COMPONENT( LArFEBConfigCondAlg )
