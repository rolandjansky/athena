# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

############################################################
# Functionality to add generic topo hypos to combined chains
############################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Combined' )
logging.getLogger().info("Importing %s",__name__)

def addTopoInfo(theChainConfig, chainDict, listOfChainDefs, lengthOfChainConfigs):
    
    theTopoInfo = chainDict['extraComboHypos']
    
    bonus_debug = False
    
    if bonus_debug:
        log.debug("[addTopoInfo] theChainConfig %s", theChainConfig)
        log.debug("[addTopoInfo] listOfChainDefs %s", listOfChainDefs)
        log.debug("[addTopoInfo] theTopoInfo being added is %s",theTopoInfo)

    if len(theChainConfig.steps[-1].comboToolConfs) > 0:
        log.warning("[addTopoInfo] last step already has ComboHypo tools %s",theChainConfig.steps[-1].comboToolConfs)
        log.warning("[addTopoInfo] these will be added to, make sure this is the behaviour you want.")

    comboTools = []
    for topoInfo in theTopoInfo:
        if "dR" in topoInfo:  # I think we don't need this anymore, 
                              # because TrigComboHypoToolFromDict checks 
                              # if the observable is present in the dict of the allowed ones
            log.debug("[addTopoInfo] topoInfo being added is %s", topoInfo)
            comboTools += [TrigComboHypoToolFromDict]
        else:
            log.error("[addTopoInfo] does not yet know what to do with topo %s",theTopoInfo)
            raise Exception("[addTopoInfo] cannot proceed, exiting.")

    for comboTool in comboTools:
        theChainConfig.steps[-1].addComboHypoTools(comboTool)
    theChainConfig.steps[-1].name = theChainConfig.steps[-1].name+'_combo_'+'_'.join(theTopoInfo) 
    
    theChainConfig.steps[-1].makeCombo()
    log.debug("[addTopoInfo] new combo hypo name: %s",theChainConfig.steps[-1].combo.name)
    
    if bonus_debug:
        log.debug("[addTopoInfo] new theChainConfig %s", theChainConfig)

    return theChainConfig

def TrigComboHypoToolFromDict(chainDict):#, topoInfo): #I'm not sure why when I try to change the constructor in order to specify the topoInfo to use, I get the error:
    from TrigComboHypo.TrigComboHypoConf import TrigComboHypoTool
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    name     = chainDict['chainName']
    topoInfo = chainDict['extraComboHypos'][0]
    #here we need to decompress the name to get: variable_name, min, max
    log.debug("[TrigComboHypoToolFromDict] new combo hypo name: %s, topoInfo = %s", name, topoInfo)

    #get the min and max values
    import re
    legs   = re.findall("leg"+r"\d{3}", topoInfo)
    if len(legs)!=2:
        log.error("[TrigComboHypoToolFromDict] N_legs = %d", len(legs))
        raise Exception("[TrigComboHypoToolFromDict] Number of legs is different from 2")
    #now extract the legIDs
    legA = int(re.findall(r"\d+", legs[0])[0])
    legB = int(re.findall(r"\d+", legs[1])[0])
        
    #now remove the legs from the chain name
    l_name = topoInfo
    for l in legs:
        l_name = l_name.replace(l, '')
    #get the cut values
    limits = re.findall(r"\d+", l_name)
    if len(limits)!=2:
        log.error("[TrigComboHypoToolFromDict] unable to get min and max values. N_limits = %d", len(limits))
        raise Exception("[TrigComboHypoToolFromDict] cannot set limits")
    cut_min = float(limits[0])/10.
    cut_max = float(limits[1])/10.
    
    allowed_obs = {
        'dR' : {
            'hist_nbins' : 50,
            'hist_min'   : 0.,
            'hist_max'   : 5.
        },
        'mass' : {
            'hist_nbins' : 100,
            'hist_min'   : 0.,
            'hist_max'   : 1000.
        }
    }
    var_to_use = 'undef'
    for v in allowed_obs:
        if v in topoInfo:
            if var_to_use == 'undef':
                var_to_use = v
            else:
                log.error("[TrigComboHypoToolFromDict] attempt to use multiple observables: %s, %s", var_to_use, v)
                raise Exception("[TrigComboHypoToolFromDict] attempt to use multiple observables.")
    
    if var_to_use == 'undef':
        log.error("[TrigComboHypoToolFromDict] didn't find any observable to use in chainName: %s", name)
        raise Exception("[TrigComboHypoToolFromDict] missing observable tag in the chainName")
    #we probably need a sequence of if to set up properly the histograms for the different cases?
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram(var_to_use+'OfAccepted', type='TH1F', path='EXPERT', title=var_to_use+" in accepted combinations [MeV]", xbins=allowed_obs[var_to_use]['hist_nbins'], xmin=allowed_obs[var_to_use]['hist_min'], xmax=allowed_obs[var_to_use]['hist_max'])]
    monTool.Histograms = [defineHistogram(var_to_use+'OfProcessed', type='TH1F', path='EXPERT', title=var_to_use+" in accepted combinations [MeV]", xbins=allowed_obs[var_to_use]['hist_nbins'], xmin=allowed_obs[var_to_use]['hist_min'], xmax=allowed_obs[var_to_use]['hist_max'])]
    tool= TrigComboHypoTool(name)
    tool.Variable    = var_to_use
    tool.LegA        = legA
    tool.LegB        = legB
    tool.LowerCut    = cut_min
    tool.UpperCut    = cut_max
    monTool.HistPath = 'ComboHypo/'+tool.getName() #FIXME! I'm not entrely sure how to set this path
    tool.MonTool     = monTool
    log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", name, topoInfo)
    return tool
