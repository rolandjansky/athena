# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Set up the ATLAS Ready filter tool
def GetAtlasReadyFilterTool():
   from AthenaCommon.Logging import logging
   arft_local_logger = logging.getLogger('AtlasReadyFilterTool')

   from AthenaMonitoring.DQMonFlags import DQMonFlags
   from AthenaCommon.GlobalFlags  import globalflags

   if (DQMonFlags.monManEnvironment == 'online'
       or globalflags.DataSource.get_Value() == 'geant4'
       or 'collisions' not in DQMonFlags.monManDataType.get_Value()
       or DQMonFlags.disableAtlasReadyFilter.get_Value()):
      arft_local_logger.info("Disabling ATLAS Ready monitoring filter")
      from AthenaMonitoring.AthenaMonitoringConf import DQDummyFilterTool
      return DQDummyFilterTool()

   if globalflags.DataSource.get_Value() != 'geant4':
      from IOVDbSvc.CondDB import conddb
      if not conddb.folderRequested('/TDAQ/RunCtrl/DataTakingMode'):
         conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/DataTakingMode', className='AthenaAttributeList')

   from AthenaMonitoring.AthenaMonitoringConf import DQAtlasReadyFilterTool
   return DQAtlasReadyFilterTool()
