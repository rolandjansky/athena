#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    # 

    ####### Signature names 

    signature_names = []
    signature_names.append("AllChains") #0
    signature_names.append("Electrons") #1
    signature_names.append("Gamma") #2
    signature_names.append("Muons") #3
    signature_names.append("MissingET") #4
    signature_names.append("Taus") #5
    signature_names.append("Jets") #6
    signature_names.append("MinBias") #7
  
    # Add a monitoring tool per signature (a "group" in old language). 
    hltGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMonitor',
        'HLT/ResultMon/'
    )
    hltAllGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTAllMonitor',
        'HLT/ResultMon/AllChains'
    )
    hltEleGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTEleMonitor',
        'HLT/ResultMon/Electrons'
    )
    hltGamGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTGamMonitor',
        'HLT/ResultMon/Gamma'
    )
    hltMuoGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMuoMonitor',
        'HLT/ResultMon/Muons'
    )
    hltMETGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMETMonitor',
        'HLT/ResultMon/MissingET'
    )
    hltTauGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTTauMonitor',
        'HLT/ResultMon/Taus'
    )
    hltJetGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTJetMonitor',
        'HLT/ResultMon/Jets'
    )
    #No RoI map for minbias


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

    ### Set up counters ########################################

    ## The HLT chains
    counter_i = 1
    for chain_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['HLTChains']:
        counter_i = counter_i+1
        log_trighlt.debug('HLT chain_name = %s',chain_name)
    max_hlt_chains = counter_i-1
    log_trighlt.debug('max_hlt_chains = %i', max_hlt_chains)


    ## The L1 items
    counter_L1 = 1
    for item_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['L1Items']:
        counter_L1 = counter_L1+1
        log_trighlt.debug('L1 item_name = %s',item_name)
    max_L1_items = counter_L1-1 
    log_trighlt.debug('max_L1_items = %i', max_L1_items)


    ##### L1 summary histogram ################################

    hltGroup.defineHistogram('L1Events',title='Events per Item at L1;;Events',
                             path='',xbins=max_L1_items,xmin=1,xmax=max_L1_items+1)


    #### HLT summary histograms for the signatures #############

    triggerstatus = ['RAW','PS'] #all chains or prescaled chains
    for sig in signature_names: #loop over signatures
        log_trighlt.debug("Signature %s",sig)

        ### Events
        titlename = sig+";;"+"Events" 
        for trigstatus in triggerstatus: #loop over RAW/PS
            histname = "HLT_"+sig+trigstatus
            log_trighlt.debug('Histname = %s', histname)
            hltGroup.defineHistogram(histname,title=titlename,
                                     path=sig,xbins=max_hlt_chains+1,
                                     xmin=1,xmax=max_hlt_chains+1)

 
    ### RoIs, one per signature
    histname = "eta,phi;HLT_"+signature_names[0]+"RoIs" #AllChains
    titlename = signature_names[0]+";#eta;#phi"
    hltAllGroup.defineHistogram(histname,type='TH2F',title=titlename,
                             path='',xbins=64,xmin=-3.2,xmax=3.2,
                             ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[1]+"RoIs" #Electrons
    titlename = signature_names[1]+";#eta;#phi"
    hltEleGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[2]+"RoIs" #Gamma
    titlename = signature_names[2]+";#eta;#phi"
    hltGamGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[3]+"RoIs" #Muons
    titlename = signature_names[3]+";#eta;#phi"
    hltMuoGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[4]+"RoIs" #MissingET
    titlename = signature_names[4]+";#eta;#phi"
    hltMETGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[5]+"RoIs" #Taus
    titlename = signature_names[5]+";#eta;#phi"
    hltTauGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)

    histname = "eta,phi;HLT_"+signature_names[6]+"RoIs" #Jets
    titlename = signature_names[6]+";#eta;#phi"
    hltJetGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                path='',xbins=64,xmin=-3.2,xmax=3.2,
                                ybins=64,ymin=-3.2,ymax=3.2)


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

