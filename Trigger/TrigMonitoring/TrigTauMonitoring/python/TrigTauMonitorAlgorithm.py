#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def TrigTauMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigTauAthMonitorCfg')

    from TrigTauMonitoring.TrigTauMonitoringConfigMT import TrigTauMonAlgBuilder
    monAlgCfg = TrigTauMonAlgBuilder( helper, '2018', detailedHistogram=True ) # Using 2018 e/g tunings
    # build monitor and book histograms
    monAlgCfg.configure()

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
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
    ConfigFlags.Output.HISTFileName = 'TrigTauMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigTauMonitorAcc = TrigTauMonConfig(ConfigFlags)
    cfg.merge(trigTauMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigJetMonitorAcc.getEventAlgo('TrigTauMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    sc = cfg.run() #use cfg.run(20) to only run on first 20 events
    if not sc.isSuccess():
       import sys
       sys.exit("Execution failed")


