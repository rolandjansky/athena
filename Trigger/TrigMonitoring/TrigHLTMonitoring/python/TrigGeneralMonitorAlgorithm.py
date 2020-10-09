#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigHLTMonitorAlgorithm.py
@date 2020-09-18
@brief TrigHLTMonitoring general monitoring
'''


def TrigGeneralMonConfig(inputFlags):
    '''Function to configure general HLT algorithms in the monitoring system.'''


    ### Set up logging functionality 
    from AthenaCommon.Logging import logging
    log_trighlt = logging.getLogger( 'TrigGeneralMonitorAlgorithm' )


    ################################
    # HLT general monitoring

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigHLTAthMonitorCfg')


    # Adding algorithms to the helper. 
    # The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    cfgsvc = CompFactory.TrigConf.xAODConfigSvc('xAODConfigSvc')
    helper.resobj.addService(cfgsvc)
    trigHLTMonAlg = helper.addAlgorithm(CompFactory.TrigHLTMonitorAlgorithm,'TrigHLTMonAlg')
    trigHLTMonAlg.TrigConfigSvc = cfgsvc



    # Edit properties of a algorithm
    # Placeholder


    # Add a generic monitoring tool (a "group" in old language). 
    hltGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMonitor',
        'HLT/ResultMon/'
    )


    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly

    ###########################################################
    ##### HLTResult and ConfigConsistency  ####################
  
    hltGroup.defineHistogram('HLTResultHLT', title='HLT Result PLACEHOLDER;result;Events',
                             path='',xbins=3,xmin=0,xmax=3)

    consistency_names=['SMK DB NULL','SMK BS NULL','SMK Inconsistent','HLT Prescale DB NULL','HLT Prescale BS NULL','HLT Prescale Inconsistent','No HLTResult']
    hltGroup.defineHistogram('ConfigConsistency_HLT', title='ConfigConsistency_HLT;;Events',
                             path='',xbins=7,xmin=1,xmax=8,xlabels=consistency_names)

 
    ############################################################
    #### Overall summary histograms ############################

    from AthenaConfiguration.AutoConfigFlags import GetFileMD

    #The HLT chains
    counter_i = 1
    for chain_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['HLTChains']:
        counter_i = counter_i+1
        log_trighlt.debug('HLT chain_name = %s',chain_name)
    max_hlt_chains = counter_i-1
    log_trighlt.info('max_hlt_chains = %i', max_hlt_chains)

    #The L1 items
    counter_L1 = 1
    for item_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['L1Items']:
        counter_L1 = counter_L1+1
        log_trighlt.debug('L1 item_name = %s',item_name)
    max_L1_items = counter_L1-1 
    log_trighlt.info('max_L1_items = %i', max_L1_items)

    ##### L1 summary histogram ################################

    hltGroup.defineHistogram('L1Events',title='Events per Item at L1;;Events',
                             path='',xbins=max_L1_items,xmin=1,xmax=max_L1_items+1)

    #### HLT summary histograms #########################

    hltGroup.defineHistogram('HLT_AllChainsRAW',title='AllChains;;Events',
                             path='AllChains',xbins=max_hlt_chains,
                             xmin=1,xmax=max_hlt_chains+1)

    hltGroup.defineHistogram('eta,phi;HLT_AllChainsRoIs',type='TH2F',title='AllChains;#eta;#phi',
                             path='AllChains',xbins=64,xmin=-3.2,xmax=3.2,
                             ybins=64,ymin=-3.2,ymax=3.2)


    #### Summary histograms for the signatures #################
    hltGroup.defineHistogram('HLT_ElectronsRAW',title='Placeholder;X;Y',
                             path='Electrons',xbins=1000000,xmin=-0.5,xmax=999999.5)



    #####################################
    # Finalize. 
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
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TrigHLTMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigHLTMonitorAcc = TrigGeneralMonConfig(ConfigFlags)
    cfg.merge(trigHLTMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigHLTMonitorAcc.getEventAlgo('TrigHLTMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

