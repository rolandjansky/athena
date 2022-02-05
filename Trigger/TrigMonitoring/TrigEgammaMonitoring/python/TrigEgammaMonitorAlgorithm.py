#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigEgammaMonitorAlgorithm.py
@author J. Pinto (jodafons)
@author D. Maximov
@author E. Bergeaas Kuutmann
@author T. Bold
@date 2019-06-28
@brief Egamma trigger python configuration for the Run III AthenaMonitoring package, based on the example by E. Bergeaas Kuutmann
'''
from AthenaCommon.Logging import logging
log_trigeg = logging.getLogger( 'TrigEgammaMonitorAlgorithm' )



def TrigEgammaMonConfig(inputFlags, emulator=None):
    '''Function to configures some algorithms in the monitoring system.'''

 
    # The following class will make a sequence, configure algorithms, and link
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigEgammaAthMonitorCfg')

    if emulator: # add emulator as public
        acc = helper.resobj
        acc.addPublicTool(emulator.core())

    # configure alg and ana tools
    from TrigEgammaMonitoring.TrigEgammaMonitoringMTConfig import TrigEgammaMonAlgBuilder
    monAlgCfg = TrigEgammaMonAlgBuilder( helper, '2018', detailedHistograms=False, emulator=emulator ) # Using 2018 e/g tunings
    # build monitor and book histograms
    monAlgCfg.configure()



    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    # return the componenet accumulator to the main call
    return helper.result()
    

    




if __name__=='__main__':

    # ATR-11839 to fix the egammaPid import
    from PyUtils.Helpers import ROOT6Setup
    ROOT6Setup()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    path = '/afs/cern.ch/work/j/jodafons/public/valid_sampleA/valid1.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e5112_s3214_d1731_r13252_tid27512952_00'
    path+='/AOD.27512952._000185.pool.root.1'
    ConfigFlags.Input.Files = [path]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TrigEgammaMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigEgammaMonitorAcc = TrigEgammaMonConfig(ConfigFlags)
    cfg.merge(trigEgammaMonitorAcc)
  


    # If you want to turn on more detailed messages ...
    #trigEgammaMonitorAcc.getEventAlgo('TrigEgammaMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run(20) #use cfg.run(20) to only run on first 20 events
