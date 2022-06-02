#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


    ###Read the menu from the database
    from TrigConfigSvc.TriggerConfigAccess import getHLTMenuAccess
    from TrigConfigSvc.TriggerConfigAccess import getL1MenuAccess

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


    ####### Signature names and regexes

    signature_names_regexes = [('HLT_AllChains','HLT_.*'), #0
                               ('HLT_Electrons','HLT_[0-9]*e[0-9]+.*'), #1
                               ('HLT_Gamma','HLT_[0-9]*g[0-9]+.*'), #2
                               ('HLT_Muons','HLT_[0-9]*mu[0-9]+.*'), #3
                               ('HLT_Taus','HLT_(tau[0-9]*|trk.*Tau).*'), #4
                               ('HLT_MissingET','HLT_(t|x)e[0-9]+.*'), #5
                               ('HLT_Jets','HLT_[0-9]*j[0-9]+.*'), #6
                               ('HLT_MinBias','HLT_mb.*')] #7
  

    #top level group
    hltGroup = helper.addGroup(
        trigHLTMonAlg,
        'TrigHLTMonitor',
        'HLT/ResultMon/'
    )


    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly

    ###########################################################
    ##### HLTResult and ConfigConsistency  ####################
  
    hltGroup.defineHistogram('HLTEvents', title='HLT events;HLT;Events',
                             path='',xbins=2,xmin=0,xmax=2)

    consistency_names=['SMK DB NULL','SMK BS NULL','SMK Inconsistent','HLT Prescale DB NULL',
                       'HLT Prescale BS NULL','HLT Prescale Inconsistent',
                       'No onlineKeys','No eventKeys']
    hltGroup.defineHistogram('ConfigConsistency_HLT', title='ConfigConsistency_HLT;;Events',
                             path='',xbins=8,xmin=1,xmax=9,xlabels=consistency_names,
                             opt='kAlwaysCreate')

 
    ############################################################
    #### Overall summary histograms ############################


    ########################
    ## The HLT chains
    ListOf_HLT_names = []
    HLT_names_signature = []
    ListOf_signatures = []

    import re
    for signame, m_this_regex in signature_names_regexes:
        log_trighlt.debug('Signature: %s', signame)
        ListOf_signatures.append(signame)
        this_regex = re.compile(m_this_regex)
        HLT_names_signature.clear() #re-use the list
        for chain_name in getHLTMenuAccess(inputFlags):
            if this_regex.match(chain_name):
                log_trighlt.debug('chain name %s matches regex %s',chain_name,m_this_regex)
                HLT_names_signature.append(chain_name)
        ListOf_HLT_names.append(list(HLT_names_signature))


    #Sort HLT names alphabetically, to make the HLT histograms have the same 
    #x axes for different runs and reprocessings
    ListOf_HLT_names_sorted = []
    HLT_names_signature_sorted = []
    for i in range(len(ListOf_HLT_names)):
        HLT_names_signature.clear()
        HLT_names_signature=ListOf_HLT_names[i]

        HLT_names_signature_sorted.clear()
        HLT_names_signature_sorted = sorted(HLT_names_signature)
        ListOf_HLT_names_sorted.append(list(HLT_names_signature_sorted))
        for chain in HLT_names_signature_sorted:
            ## Print sorted HLT chain list 
            log_trighlt.debug('sorted HLT chain: %s',chain)

    #########################
    ## The L1 items
    L1_names =[]
    log_trighlt.debug('L1 items: ') 
    for item_name in getL1MenuAccess(inputFlags):
        L1_names.append(item_name)
    max_L1_items = len(L1_names)
    log_trighlt.debug('max_L1_items = %i', max_L1_items) 

    #Sort L1 names alphabetically, to make the L1 histograms have the same 
    #x axes for different runs and reprocessings
    L1_names_sorted = sorted(L1_names)
    #Print the L1 items in the menu
    for item_name in L1_names_sorted:
        log_trighlt.debug('L1 item: = %s',item_name)


    ##### L1 summary histogram ################################

    hltGroup.defineHistogram('L1Events',title='Events per Item at L1;;Events',
                             path='',xbins=len(L1_names_sorted), xmin=0, 
                             xmax=len(L1_names_sorted), xlabels=L1_names_sorted)

    #### HLT summary histograms for the signatures #############

    triggerstatus = ['RAW','PS'] #all chains or prescaled chains

    for i, pair in enumerate(signature_names_regexes):
        sig = pair[0]
        m_this_regex = pair[1]
        log_trighlt.debug("Signature %s, pattern %s:",sig, m_this_regex)

        ### Events
        titlename = sig+";;"+"Events" 
        HLT_names_signature_sorted.clear()
        HLT_names_signature_sorted = ListOf_HLT_names_sorted[i]

        Xmax=len(HLT_names_signature_sorted)
        if Xmax<1:
            Xmax = 1 #Avoid error messages if there are no chains in this signature
        log_trighlt.debug('Xmax= %i', Xmax)

        for trigstatus in triggerstatus: #loop over RAW/PS
            histname = sig+trigstatus
            titlename = sig+trigstatus+";;"+"Events" 
            hltGroup.defineHistogram(histname,title=titlename,
                                     path=sig,xbins=len(HLT_names_signature_sorted),
                                     xmin=0,xmax=Xmax, 
                                     xlabels=HLT_names_signature_sorted,opt='kAlwaysCreate')

        #eta and phi, 1D
        histname = sig+"_eta"
        titlename = histname+";#eta;"+"Events"
        hltGroup.defineHistogram(histname,title=titlename,
                                 path=sig,xbins=64,xmin=-3.2,xmax=3.2)

        histname = sig+"_phi"
        titlename = histname+";#phi;"+"Events"
        hltGroup.defineHistogram(histname,title=titlename,
                                 path=sig,xbins=64,xmin=-3.2,xmax=3.2)

        #RoI count
        histname = sig+"RoI_N"
        titlename = histname+";Number of RoIs;"+"Events"
        hltGroup.defineHistogram(histname,title=titlename,
                                 path=sig,xbins=100,xmin=0,xmax=500)


 
    ### RoIs, one per signature
    #Tools for the signatures, including AllChains as first item
    listOfSignatureGroups =[]
    for signame in ListOf_signatures:
        thistoolname = 'Trig'+signame+'Monitor'
        thisGroup = helper.addGroup(
            trigHLTMonAlg,
            thistoolname,
            'HLT/ResultMon/'+signame
        )
        listOfSignatureGroups.append(thisGroup)


    #Creating the RoI histograms
    log_trighlt.debug('Creating RoI histograms...')
    for i, thisGroup in enumerate(listOfSignatureGroups):
        thispair = signature_names_regexes[i] 
        thissigname = thispair[0] #first item in the pair is the signature name
        histname = "eta,phi;"+thissigname+"RoIs" 

        titlename = thissigname+"RoIs;#eta;#phi"
        thisGroup.defineHistogram(histname,type='TH2F',title=titlename,
                                  path='',xbins=64,xmin=-3.2,xmax=3.2,
                                  ybins=64,ymin=-3.2,ymax=3.2)



    #####################################
    # Finalize. 
    return helper.result()
    

if __name__=='__main__':
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

