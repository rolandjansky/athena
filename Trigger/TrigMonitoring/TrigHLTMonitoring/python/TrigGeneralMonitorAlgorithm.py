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
  
    hltGroup.defineHistogram('HLTEvents', title='HLT events;HLT;Events',
                             path='',xbins=2,xmin=0,xmax=2)

    consistency_names=['SMK DB NULL','SMK BS NULL','SMK Inconsistent','HLT Prescale DB NULL','HLT Prescale BS NULL','HLT Prescale Inconsistent','No onlineKeys','No eventKeys']
    hltGroup.defineHistogram('ConfigConsistency_HLT', title='ConfigConsistency_HLT;;Events',
                             path='',xbins=8,xmin=1,xmax=9,xlabels=consistency_names,opt='kAlwaysCreate')

 
    ############################################################
    #### Overall summary histograms ############################

    from AthenaConfiguration.AutoConfigFlags import GetFileMD


    ########################
    ## The HLT chains
    HLT_names_AllChains = []
    log_trighlt.debug('HLT chains:')
    for chain_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['HLTChains']:
        log_trighlt.debug('HLT chain_name = %s',chain_name)
        HLT_names_AllChains.append(chain_name)
    max_hlt_chains = len(HLT_names_AllChains)
    log_trighlt.debug('max_hlt_chains = %i', max_hlt_chains) 


    #Sort HLT names alphabetically, to make the HLT histograms have the same 
    #x axes for different runs and reprocessings
    HLT_names_AllChains_sorted = sorted(HLT_names_AllChains)



    #########################
    ## The L1 items
    L1_names =[]
    for item_name in GetFileMD(inputFlags.Input.Files)['TriggerMenu']['L1Items']:
        log_trighlt.debug('L1 item_name = %s',item_name) 
        L1_names.append(item_name)
    max_L1_items = len(L1_names)
    log_trighlt.debug('max_L1_items = %i', max_L1_items) 

    #Sort L1 names alphabetically, to make the L1 histograms have the same 
    #x axes for different runs and reprocessings
    L1_names_sorted = sorted(L1_names)

    ##### L1 summary histogram ################################

    hltGroup.defineHistogram('L1Events',title='Events per Item at L1;;Events',
                             path='',xbins=len(L1_names_sorted), xmin=0, 
                             xmax=len(L1_names_sorted), xlabels=L1_names_sorted)

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
                                     path=sig,xbins=len(HLT_names_AllChains_sorted),
                                     xmin=0,xmax=len(HLT_names_AllChains_sorted), 
                                     xlabels=HLT_names_AllChains_sorted)

 
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

