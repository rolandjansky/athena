//#include "LArRecUtils/LArCellFakeProbHV.h"
//#include "LArRecUtils/LArHVGeometryTool.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArRecUtils/LArShapePeakRecoTool.h"
#include "../LArTowerBuilderTool.h"
#include "../LArFCalTowerBuilderTool.h"
#include "../LArFlatConditionSvc.h"
#include "../LArFCalTowerBuilderToolTestAlg.h"
#include "../LArFlatConditionsAlg.h"
#include "../LArOnOffMappingAlg.h"
#include "../LArLATOMEMappingAlg.h"
#include "../LArCalibLineMappingAlg.h"
#include "../LArFebRodMappingAlg.h"
#include "../LArSymConditionsAlg.h"
#include "../LArMCSymCondAlg.h"
#include "../LArADC2MeVCondAlg.h"
#include "../LArAutoCorrTotalCondAlg.h"
#include "../LArOFCCondAlg.h"
#include "../LArHVPathologyDbCondAlg.h"
#include "../LArHVIdMappingAlg.h"
#include "../LArHVCondAlg.h"
#include "../LArAutoCorrNoiseCondAlg.h"
#include "../LArFEBConfigCondAlg.h"
#include "../LArRoIMapCondAlg.h"
#include "../LArXTalkWeightCondAlg.h"
#include "../LArXTalkWeightGlobalCondAlg.h"


//DECLARE_COMPONENT( LArCellFakeProbHV )
//DECLARE_COMPONENT( LArHVGeometryTool )
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
DECLARE_COMPONENT( LArCondAlgMinBiasAverageSC )
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
DECLARE_COMPONENT( LArLATOMEMappingAlg )
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
DECLARE_COMPONENT( LArAutoCorrNoiseCondAlg )
DECLARE_COMPONENT( LArFEBConfigCondAlg )
DECLARE_COMPONENT( LArRoIMapCondAlg )
DECLARE_COMPONENT( LArXTalkWeightGlobalCondAlg )
DECLARE_COMPONENT( LArXTalkWeightCondAlg )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_strip )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_strip_ec )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middleback )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middleback_ecow )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middleback_eciw )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_stripmiddle )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_stripmiddle_ec )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_2strip )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_2strip_ec )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middle1 )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middle2 )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middle1_ec )
DECLARE_COMPONENT( LArXTalkWeightCondAlg_middle2_ec )
