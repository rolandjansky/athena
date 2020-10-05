#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file CaloMonitoringConfig.py
@brief Python configuration of Calo Monitoring for the Run III
'''

def CaloMonitoringCfg(flags):
    ''' Function to configure Calo Monitoring in the monitoring system for Run III.'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'CaloMonitoringCfg' )

    environment = flags.DQ.Environment

    if environment in ('online', 'tier0', 'tier0ESD'):
        msg.info('Setup Calo Monitoring for ESD data due to environment: %s', environment)

        from CaloMonitoring.TileCalCellMonAlg import TileCalCellMonAlgConfig
        acc.merge( TileCalCellMonAlgConfig(flags) )

        from CaloMonitoring.LArCellMonAlg import LArCellMonConfig
        acc.merge( LArCellMonConfig(flags) )

        # FIXME could not be included yet, some trigger configurations are missing
        #from CaloMonitoring.CaloBaselineMonAlg import CaloBaselineMonConfig
        #acc.merge( CaloBaselineMonConfig(flags,False) )

    return acc



if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import INFO
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior = True
   log.setLevel(INFO)

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.ESD

   ConfigFlags.Output.HISTFileName = 'CaloMonitoringOutput.root'
   ConfigFlags.DQ.enableLumiAccess = True
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.DQ.Environment = 'tier0'

   ConfigFlags.lock()

   # Initialize configuration object, add accumulator, merge, and run.
   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
   acc = MainServicesCfg(ConfigFlags)
   acc.merge(PoolReadCfg(ConfigFlags))

   acc.merge( CaloMonitoringCfg(ConfigFlags) )

   acc.printConfig(withDetails = True, summariseProps = True)
   ConfigFlags.dump()
   acc.store(open("CaloMonitoring.pkl","wb"))

   sc = acc.run(maxEvents = 3)
   import sys
   # Success should be 0
   sys.exit(not sc.isSuccess())
