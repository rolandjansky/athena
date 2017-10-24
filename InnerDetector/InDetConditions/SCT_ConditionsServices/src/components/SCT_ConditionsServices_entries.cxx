#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SCT_ConditionsSummarySvc.h"
#include "../SCT_ModuleVetoSvc.h"
#include "../SCT_StripVetoSvc.h"

#include "../SCT_ModuleVetoTestAlg.h"
#include "../SCT_ConditionsSummaryTestAlg.h"

#include "../SCT_DCSConditionsSvc.h"
#include "../SCT_DCSConditionsHVCondAlg.h"
#include "../SCT_DCSConditionsStatCondAlg.h"
#include "../SCT_DCSConditionsTempCondAlg.h"
#include "../SCT_DCSConditionsTestAlg.h"

#include "../SCT_ByteStreamErrorsSvc.h"
#include "../SCT_ConfigurationConditionsSvc.h"
#include "../SCT_ConfigurationConditionsTestAlg.h"

#include "../SCT_MonitorConditionsSvc.h"
#include "../SCT_MonitorConditionsCondAlg.h"
#include "../SCT_MonitorConditionsTestAlg.h"

#include "../SCT_FlaggedConditionSvc.h"

#include "../SCT_ReadCalibDataSvc.h"
#include "../SCT_ReadCalibDataTestAlg.h"
#include "../SCT_ReadCalibChipDataSvc.h"
#include "../SCT_ReadCalibChipDataTestAlg.h"

#include "../SCT_SiliconConditionsSvc.h"
#include "../SCT_SiliconConditionsTestAlg.h"

#include "../SCT_ReadoutTool.h"
#include "../SCT_ReadoutTestAlg.h"

#include "../SCT_LinkMaskingSvc.h"
#include "../SCT_LinkMaskingTestAlg.h"

#include "../SCT_TdaqEnabledSvc.h"
#include "../SCT_TdaqEnabledCondAlg.h"
#include "../SCT_TdaqEnabledTestAlg.h"

#include "../SCT_ConditionsParameterSvc.h"
#include "../SCT_ConditionsParameterTestAlg.h"

#include "../SCT_SensorsSvc.h"
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

DECLARE_ALGORITHM_FACTORY(SCT_RODVetoTestAlg)
DECLARE_SERVICE_FACTORY(SCT_RODVetoSvc)

DECLARE_ALGORITHM_FACTORY(SCT_ModuleVetoTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ConditionsSummaryTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_CachedSummaryTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_DCSConditionsHVCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_DCSConditionsStatCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_DCSConditionsTempCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_DCSConditionsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_MonitorConditionsCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_MonitorConditionsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ReadCalibDataTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ReadCalibChipDataTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_SiliconConditionsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ReadoutTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_LinkMaskingTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ConfigurationConditionsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_MajorityCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_MajorityConditionsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_ConditionsParameterTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_SensorsTestAlg)
DECLARE_ALGORITHM_FACTORY(SCT_TdaqEnabledCondAlg)
DECLARE_ALGORITHM_FACTORY(SCT_TdaqEnabledTestAlg)

DECLARE_SERVICE_FACTORY(SCT_ConditionsSummarySvc)
DECLARE_SERVICE_FACTORY(SCT_CachedSummarySvc)
DECLARE_SERVICE_FACTORY(SCT_ModuleVetoSvc)
DECLARE_SERVICE_FACTORY(SCT_StripVetoSvc)
DECLARE_SERVICE_FACTORY(SCT_ConfigurationConditionsSvc)
DECLARE_SERVICE_FACTORY(SCT_DCSConditionsSvc)
DECLARE_SERVICE_FACTORY(SCT_ByteStreamErrorsSvc)
DECLARE_SERVICE_FACTORY(SCT_MonitorConditionsSvc)
DECLARE_SERVICE_FACTORY(SCT_FlaggedConditionSvc)
DECLARE_SERVICE_FACTORY(SCT_ReadCalibDataSvc)
DECLARE_SERVICE_FACTORY(SCT_ReadCalibChipDataSvc)
DECLARE_SERVICE_FACTORY(SCT_SiliconConditionsSvc)
DECLARE_SERVICE_FACTORY(SCT_LinkMaskingSvc)
DECLARE_SERVICE_FACTORY(SCT_TdaqEnabledSvc)
DECLARE_SERVICE_FACTORY(SCT_ConditionsParameterSvc)
DECLARE_SERVICE_FACTORY(SCT_SensorsSvc)
DECLARE_SERVICE_FACTORY(SCT_MajorityConditionsSvc)
DECLARE_SERVICE_FACTORY(SCT_ChargeTrappingSvc)
DECLARE_SERVICE_FACTORY(SCT_RadDamageSummarySvc)


DECLARE_TOOL_FACTORY(SCT_ReadoutTool)
DECLARE_TOOL_FACTORY(SCT_ElectricFieldTool)

DECLARE_FACTORY_ENTRIES(SCT_ConditionsServices) {
  DECLARE_SERVICE( SCT_RODVetoSvc );

  DECLARE_SERVICE( SCT_CachedSummarySvc);
  DECLARE_SERVICE( SCT_ConditionsSummarySvc);
  DECLARE_SERVICE( SCT_ModuleVetoSvc );
  DECLARE_SERVICE( SCT_StripVetoSvc );
  DECLARE_SERVICE( SCT_ConfigurationConditionsSvc );
  DECLARE_SERVICE( SCT_DCSConditionsSvc );
  DECLARE_SERVICE( SCT_ByteStreamErrorsSvc );
  DECLARE_SERVICE( SCT_MonitorConditionsSvc );
  DECLARE_SERVICE( SCT_FlaggedConditionSvc );
  DECLARE_SERVICE( SCT_ReadCalibDataSvc )
  DECLARE_SERVICE( SCT_ReadCalibChipDataSvc )
  DECLARE_SERVICE( SCT_SiliconConditionsSvc )
  DECLARE_SERVICE( SCT_LinkMaskingSvc )
  DECLARE_SERVICE( SCT_TdaqEnabledSvc )
  DECLARE_SERVICE( SCT_ConditionsParameterSvc )
  DECLARE_SERVICE( SCT_SensorsSvc )
  DECLARE_SERVICE( SCT_MajorityConditionsSvc )
  DECLARE_SERVICE( SCT_ChargeTrappingSvc )
  DECLARE_SERVICE( SCT_RadDamageSummarySvc )

  DECLARE_ALGORITHM( SCT_RODVetoTestAlg )
  DECLARE_ALGORITHM( SCT_ModuleVetoTestAlg )
  DECLARE_ALGORITHM( SCT_ConditionsSummaryTestAlg )
  DECLARE_ALGORITHM( SCT_CachedSummaryTestAlg )

  DECLARE_ALGORITHM( SCT_DCSConditionsHVCondAlg )
  DECLARE_ALGORITHM( SCT_DCSConditionsStatCondAlg )
  DECLARE_ALGORITHM( SCT_DCSConditionsTempCondAlg )
  DECLARE_ALGORITHM( SCT_DCSConditionsTestAlg )
  DECLARE_ALGORITHM( SCT_MonitorConditionsCondAlg )
  DECLARE_ALGORITHM( SCT_MonitorConditionsTestAlg )
  DECLARE_ALGORITHM( SCT_ReadCalibDataTestAlg )
  DECLARE_ALGORITHM( SCT_ReadCalibChipDataTestAlg )
  DECLARE_ALGORITHM( SCT_SiliconConditionsTestAlg )
  DECLARE_ALGORITHM( SCT_ReadoutTestAlg )
  DECLARE_ALGORITHM( SCT_LinkMaskingTestAlg )
  DECLARE_ALGORITHM( SCT_ConfigurationConditionsTestAlg )
  DECLARE_ALGORITHM( SCT_MajorityCondAlg )
  DECLARE_ALGORITHM( SCT_MajorityConditionsTestAlg )
  DECLARE_ALGORITHM( SCT_ConditionsParameterTestAlg )
  DECLARE_ALGORITHM( SCT_SensorsTestAlg )
  DECLARE_ALGORITHM(SCT_TdaqEnabledCondAlg)
  DECLARE_ALGORITHM(SCT_TdaqEnabledTestAlg)
  
  

  DECLARE_TOOL(SCT_ReadoutTool)
  DECLARE_TOOL(SCT_ElectricFieldTool)
}


