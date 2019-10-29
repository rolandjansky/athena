#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigEgammaMonitorAlgorithm.py
@author D. Maximov
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-06-28
@brief Egamma trigger python configuration for the Run III AthenaMonitoring package, based on the example by E. Bergeaas Kuutmann
'''
from AthenaCommon.Logging import logging
log_trigeg = logging.getLogger( 'TrigEgammaMonitorAlgorithm' )

from TrigEgammaMonitoring.TrigEgammaMonitoringConfigRun3 import TrigEgammaMonToolBuilderRun3


def TrigEgammaMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigEgammaAthMonitorCfg')

    histogramsRoot = '/HLT/Egamma'

    montoolCfg = TrigEgammaMonToolBuilderRun3()

    montoolCfg.bookHistograms(histogramsRoot, helper)

    from AthenaCommon.AppMgr import ToolSvc

    result.addService(ToolSvc)
    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can
    # just return directly (and not create "result" above)
    #return helper.result()

    # # Otherwise, merge with result object and return
    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/eos/user/f/fernando/data18_13TeV/'
    file = 'AOD.15529645._000080.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigEgammaMonitorOutput.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigEgammaMonitorAcc = TrigEgammaMonConfig(ConfigFlags)
    cfg.merge(trigEgammaMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigEgammaMonitorAcc.getEventAlgo('TrigEgammaMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
