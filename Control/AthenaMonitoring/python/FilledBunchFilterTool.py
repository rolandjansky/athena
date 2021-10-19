# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Set up the filled bunch filter tool
def GetFilledBunchFilterTool():
   from AthenaCommon.Logging import logging
   fbft_local_logger = logging.getLogger('FilledBunchFilterTool')
   fbft_local_logger.verbose('Configuring monFilledBunchFilterTool')

   from AthenaMonitoring.DQMonFlags import DQMonFlags
   from AthenaCommon.GlobalFlags  import globalflags
   from RecExConfig.RecFlags import rec

   if (rec.triggerStream() == 'CosmicCalo'
       or globalflags.DataSource.get_Value() == 'geant4'
       or 'collisions' not in DQMonFlags.monManDataType.get_Value()
       or DQMonFlags.disableFilledBunchFilter.get_Value()):
      fbft_local_logger.verbose('Filled bunch filter being configured to always return true')
      from AthenaMonitoring.AthenaMonitoringConf import DQDummyFilterTool
      return DQDummyFilterTool()
   else:
      fbft_local_logger.verbose('Filled bunch filter being configured')
      from AthenaMonitoring.AthenaMonitoringConf import DQFilledBunchFilterTool
      return DQFilledBunchFilterTool()