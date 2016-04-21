include.block('AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py')

# Set up the ATLAS Ready filter tool
from AthenaCommon.Logging import logging

arft_local_logger = logging.getLogger('AtlasReadyFilterTool_jobOptions')

if not 'DQMonFlags' in dir():
   arft_local_logger.debug("AtlasReadyFilterTool_jobOptions.py: DQMonFlags not yet imported - I import them now")
   from AthenaMonitoring.DQMonFlags import DQMonFlags

if not 'monAtlasReadyFilterTool' in dir():
   if globalflags.DataSource.get_Value() != 'geant4':
      from IOVDbSvc.CondDB import conddb
      conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/DataTakingMode')

   from AthenaMonitoring.AthenaMonitoringConf import DQAtlasReadyFilterTool
   monAtlasReadyFilterTool = DQAtlasReadyFilterTool()
   if (DQMonFlags.monManEnvironment == 'online'
       or globalflags.DataSource.get_Value() == 'geant4'
       or 'collisions' not in DQMonFlags.monManDataType.get_Value()
       or DQMonFlags.disableAtlasReadyFilter.get_Value()):
      arft_local_logger.info("Disabling ATLAS Ready monitoring filter")
      monAtlasReadyFilterTool.alwaysReturnTrue = True
   ToolSvc += monAtlasReadyFilterTool

del arft_local_logger
