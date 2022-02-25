#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def TrigBphysMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    
    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigBphysAthMonitorCfg')
    
    from TrigBphysMonitoring.TrigBphysMonitoringConfigMT import TrigBphysMonAlgBuilder
    monAlgCfg = TrigBphysMonAlgBuilder( helper, useMonGroups = True ) 
    
    # build monitor and book histograms
    monAlgCfg.configure()
    
    helper.result().printConfig(withDetails=True, summariseProps=True)
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
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigBphysMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigBphysMonitorAcc = TrigBphysMonConfig(ConfigFlags)
    cfg.merge(trigBphysMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigBphysMonitorAcc.getEventAlgo('TrigBphysMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

