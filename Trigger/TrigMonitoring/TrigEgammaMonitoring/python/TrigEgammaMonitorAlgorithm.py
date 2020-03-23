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



def TrigEgammaMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    #from AthenaConfiguration.ComponentFactory import CompFactory
    
    
    ##from TrigEgammaAnalysisToolsMT.TrigEgammaAnalysisToolsMTConf import TrigEgammaMonAlgorithm
    helper = AthMonitorCfgHelper(inputFlags,'TrigEgammaAthMonitorCfg')
    ##monAlg = helper.addAlgorithm(TrigEgammaMonAlgorithm,'TrigEgammaMonAlg')
    


    # configure alg and ana tools
    ##from TrigEgammaMonitoring.TrigEgammaMonitoringMTConfig import TrigEgammaMonToolBuilder
    # The last arg shold be an config flag for future
    # The helper.resobj is the component accumulator created by the helper and will hold all 
    # algs and tools created by this configuration
    ##montoolCfg = TrigEgammaMonToolBuilder( monAlg , "TrigEgammaMonAlg", helper.resobj, '2018') # Using 2018 e/g tunings




    # configure all histograms
    ##from TrigEgammaAnalysisToolsMT.TrigEgammaAnalysisMTBuilder import TrigEgammaAnalysisBuilder
    ## get the histogram builder for all analysis tools for e/g staff. 
    ## link the algotihm and helper with the ana builder.    
    ## All generic monitoring tools will be attached inside of the monAlg and componenet accumulator
    ##anatoolCfg = TrigEgammaAnalysisBuilder( helper, monAlg, 'TrigEgammaMonAlg', montoolCfg.basePath ,
    ##                                        montoolCfg.detailLevel)




    # Get only unique triggers
    #triggerList = []
    #for anaTool in montoolCfg.anaTools:
    #  triggerList.extend(anaTool.TriggerList)


    # Check there is duplicate trigger into the trigger list. If we have, abort
    #if not (len(triggerList) == len(set(triggerList))):
    #  raise RuntimeError( "There are duplicate trigger into the e/g monitoring list.")




    # Attach the histogram manager my monAlg that pointet to "TrigEgammaMonAlg"
    # All hisograms will be "TrigEgammaMonAlg/HLT/Egamma/trigger/..." where
    # "TrigEgammaMonAlg" is the monname and HLT/Egamma is the basepath
    #for trigger in triggerList:
    #  anatoolCfg.bookTriggerHistograms( trigger , doJpsiee = True if 'Jpsi' in anaTool.name() else False )



    #for anaTool in montoolCfg.anaTools:
    #  anatoolCfg.bookEventHistograms( anaTool )



    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    # return the componenet accumulator to the main call
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
    trigEgammaMonitorAcc.getEventAlgo('TrigEgammaMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run(10) #use cfg.run(20) to only run on first 20 events
