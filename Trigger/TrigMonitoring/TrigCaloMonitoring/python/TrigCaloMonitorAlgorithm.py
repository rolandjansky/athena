#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigCaloMonitorAlgorithm.py
@author D. Maximov
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-06-28
@brief Calorimeter trigger python configuration for the Run III AthenaMonitoring package, based on the example by E. Bergeaas Kuutmann
'''
#from AthenaCommon.Logging import logging
#log_trigeg = logging.getLogger( 'TrigCaloMonitorAlgorithm' )

#from TrigCaloMonitoring.TrigCaloMonitoringConfigRun3 import HLTCaloMonitoringTool

def TrigCaloMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigCaloAthMonitorCfg')

    from TrigCaloMonitoring.TrigCaloMonitoringConf import TrigCaloMonitorAlgorithm
    trigCaloMonAlg = helper.addAlgorithm(TrigCaloMonitorAlgorithm,'TrigCaloMonAlg')

#    histogramsRoot = '/HLT/HLTCalo'

#    montoolCfg = HLTCaloMonitoringTool()

#    montoolCfg.bookHistograms(histogramsRoot, helper)

    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)

    CaloMonGroup = helper.addGroup(trigCaloMonAlg,'TrigCaloMonitor','HLT/HLTCalo')

    CaloMonGroup.defineHistogram('test',title='Test;testx;Events',path='Test',xbins=100,xmin=0.0,xmax=100.)
    CaloMonGroup.defineHistogram('lumiPerBCID',title='Luminosity;L/BCID;Events',
                            path='Test',xbins=10,xmin=0.0,xmax=10.0)
    CaloMonGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
                            path='Test',xbins=1000,xmin=-0.5,xmax=999.5)

    return helper.result()
    
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

    # Input
    file = '/afs/cern.ch/work/j/joheinri/public/TestAOD/AOD.pool.root'
    ConfigFlags.Input.Files = [file]
    ConfigFlags.Input.isMC = True

    # Output
    ConfigFlags.Output.HISTFileName = 'TrigCaloMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigCaloMonitorAcc = TrigCaloMonConfig(ConfigFlags)
    cfg.merge(trigCaloMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigCaloMonitorAcc.getEventAlgo('TrigCaloMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events
