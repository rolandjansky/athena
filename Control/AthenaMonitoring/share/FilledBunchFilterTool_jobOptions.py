# Set up the filled bunch filter tool
include.block('AthenaMonitoring/FilledBunchFilterTool_jobOptions.py')
from AthenaCommon.Logging import logging

fbft_local_logger = logging.getLogger('FilledBunchFilterTool_jobOptions')

if not 'DQMonFlags' in dir():
   fbft_local_logger.info("FilledBunchFilterTool_jobOptions.py: DQMonFlags not yet imported - I import them now")
   from AthenaMonitoring.DQMonFlags import DQMonFlags

if not 'monFilledBunchFilterTool' in dir():
   fbft_local_logger.info('Configuring monFilledBunchFilterTool')
   from AthenaMonitoring.AthenaMonitoringConf import DQFilledBunchFilterTool
   from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
   monFilledBunchFilterTool = DQFilledBunchFilterTool()
   monFilledBunchFilterTool.bunchCrossingTool = BunchCrossingTool()
   if (rec.triggerStream() == 'CosmicCalo'
       or globalflags.DataSource.get_Value() == 'geant4'
       or 'collisions' not in DQMonFlags.monManDataType.get_Value()
       or DQMonFlags.disableFilledBunchFilter.get_Value()):
      fbft_local_logger.info('Filled bunch filter being configured to always return true')
      monFilledBunchFilterTool.alwaysReturnTrue = True
   ToolSvc += monFilledBunchFilterTool

del fbft_local_logger
