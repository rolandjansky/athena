#include "../SCT_ConditionsSummarySvc.h"
#include "../SCT_ConditionsSummaryTestAlg.h"

#include "../SCT_StripVetoSvc.h"

#include "../SCT_ModuleVetoSvc.h"
#include "../SCT_ModuleVetoCondAlg.h"
#include "../SCT_ModuleVetoTestAlg.h"

#include "../SCT_DCSConditionsSvc.h"
#include "../SCT_DCSConditionsHVCondAlg.h"
#include "../SCT_DCSConditionsStatCondAlg.h"
#include "../SCT_DCSConditionsTempCondAlg.h"
#include "../SCT_DCSConditionsTestAlg.h"

#include "../SCT_ByteStreamErrorsSvc.h"

#include "../SCT_ConfigurationConditionsSvc.h"
#include "../SCT_ConfigurationCondAlg.h"
#include "../SCT_ConfigurationConditionsTestAlg.h"

#include "../SCT_MonitorConditionsSvc.h"
#include "../SCT_MonitorConditionsCondAlg.h"
#include "../SCT_MonitorConditionsTestAlg.h"

#include "../SCT_FlaggedConditionSvc.h"

#include "../SCT_ReadCalibDataSvc.h"
#include "../SCT_ReadCalibDataTestAlg.h"
#include "../SCT_ReadCalibDataCondAlg.h"

#include "../SCT_ReadCalibChipDataSvc.h"
#include "../SCT_ReadCalibChipDataTestAlg.h"
#include "../SCT_ReadCalibChipGainCondAlg.h"
#include "../SCT_ReadCalibChipNoiseCondAlg.h"

#include "../SCT_SiliconConditionsSvc.h"
#include "../SCT_SiliconConditionsTestAlg.h"

#include "../SCT_ReadoutTool.h"
#include "../SCT_ReadoutTestAlg.h"

#include "../SCT_LinkMaskingSvc.h"
#include "../SCT_LinkMaskingCondAlg.h"
#include "../SCT_LinkMaskingTestAlg.h"

#include "../SCT_TdaqEnabledSvc.h"
#include "../SCT_TdaqEnabledCondAlg.h"
#include "../SCT_TdaqEnabledTestAlg.h"

#include "../SCT_ConditionsParameterSvc.h"
#include "../SCT_ConditionsParameterTestAlg.h"

#include "../SCT_SensorsSvc.h"
#include "../SCT_SensorsCondAlg.h"
#include "../SCT_SensorsTestAlg.h"

#include "../SCT_MajorityConditionsSvc.h"
#include "../SCT_MajorityCondAlg.h"
#include "../SCT_MajorityConditionsTestAlg.h"

#include "../SCT_CachedSummarySvc.h"
#include "../SCT_CachedSummaryTestAlg.h"

#include "../SCT_ChargeTrappingSvc.h"
#include "../SCT_RadDamageSummarySvc.h"
#include "../SCT_ElectricFieldTool.h"

#include "../SCT_RODVetoSvc.h"
#include "../SCT_RODVetoTestAlg.h"

//specific to rod veto testing:

DECLARE_COMPONENT( SCT_RODVetoTestAlg )
DECLARE_COMPONENT( SCT_RODVetoSvc )

DECLARE_COMPONENT( SCT_ModuleVetoCondAlg )
DECLARE_COMPONENT( SCT_ModuleVetoTestAlg )
DECLARE_COMPONENT( SCT_ConditionsSummaryTestAlg )
DECLARE_COMPONENT( SCT_CachedSummaryTestAlg )
DECLARE_COMPONENT( SCT_DCSConditionsHVCondAlg )
DECLARE_COMPONENT( SCT_DCSConditionsStatCondAlg )
DECLARE_COMPONENT( SCT_DCSConditionsTempCondAlg )
DECLARE_COMPONENT( SCT_DCSConditionsTestAlg )
DECLARE_COMPONENT( SCT_MonitorConditionsCondAlg )
DECLARE_COMPONENT( SCT_MonitorConditionsTestAlg )
DECLARE_COMPONENT( SCT_ReadCalibDataTestAlg )
DECLARE_COMPONENT( SCT_ReadCalibChipDataTestAlg )
DECLARE_COMPONENT( SCT_ReadCalibChipGainCondAlg )
DECLARE_COMPONENT( SCT_ReadCalibChipNoiseCondAlg )
DECLARE_COMPONENT( SCT_SiliconConditionsTestAlg )
DECLARE_COMPONENT( SCT_ReadoutTestAlg )
DECLARE_COMPONENT( SCT_ReadCalibDataCondAlg )
DECLARE_COMPONENT( SCT_LinkMaskingCondAlg )
DECLARE_COMPONENT( SCT_LinkMaskingTestAlg )
DECLARE_COMPONENT( SCT_ConfigurationConditionsTestAlg )
DECLARE_COMPONENT( SCT_ConfigurationCondAlg )
DECLARE_COMPONENT( SCT_MajorityCondAlg )
DECLARE_COMPONENT( SCT_MajorityConditionsTestAlg )
DECLARE_COMPONENT( SCT_ConditionsParameterTestAlg )
DECLARE_COMPONENT( SCT_SensorsCondAlg )
DECLARE_COMPONENT( SCT_SensorsTestAlg )
DECLARE_COMPONENT( SCT_TdaqEnabledCondAlg )
DECLARE_COMPONENT( SCT_TdaqEnabledTestAlg )

DECLARE_COMPONENT( SCT_ConditionsSummarySvc )
DECLARE_COMPONENT( SCT_CachedSummarySvc )
DECLARE_COMPONENT( SCT_ModuleVetoSvc )
DECLARE_COMPONENT( SCT_StripVetoSvc )
DECLARE_COMPONENT( SCT_ConfigurationConditionsSvc )
DECLARE_COMPONENT( SCT_DCSConditionsSvc )
DECLARE_COMPONENT( SCT_ByteStreamErrorsSvc )
DECLARE_COMPONENT( SCT_MonitorConditionsSvc )
DECLARE_COMPONENT( SCT_FlaggedConditionSvc )
DECLARE_COMPONENT( SCT_ReadCalibDataSvc )
DECLARE_COMPONENT( SCT_ReadCalibChipDataSvc )
DECLARE_COMPONENT( SCT_SiliconConditionsSvc )
DECLARE_COMPONENT( SCT_LinkMaskingSvc )
DECLARE_COMPONENT( SCT_TdaqEnabledSvc )
DECLARE_COMPONENT( SCT_ConditionsParameterSvc )
DECLARE_COMPONENT( SCT_SensorsSvc )
DECLARE_COMPONENT( SCT_MajorityConditionsSvc )
DECLARE_COMPONENT( SCT_ChargeTrappingSvc )
DECLARE_COMPONENT( SCT_RadDamageSummarySvc )


DECLARE_COMPONENT( SCT_ReadoutTool )
DECLARE_COMPONENT( SCT_ElectricFieldTool )




  
  

