# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

############################################################
# Functionality to add generic topo hypos to combined chains
############################################################

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
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
        if 'dR' in topoInfo or 'invm' in topoInfo:  # We definitely do need this because not all combo hypos 
                                                    # are TrigComboHypoToolFromDict
            log.debug("[addTopoInfo] topoInfo being added is %s", topoInfo)
            comboTools += [TrigComboHypoToolFromDict]
        elif 'afpdijet' in theTopoInfo:

            from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasChainConfiguration import TrigAFPDijetComboHypoToolCfg
            comboTools += [TrigAFPDijetComboHypoToolCfg]
            
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

def TrigComboHypoToolFromDict(chainDict):
    from TrigHypoCommonTools.TrigHypoCommonToolsConf import TrigComboHypoTool
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    import math

    name     = chainDict['chainName']
    topoInfo = chainDict['extraComboHypos'][0]
    #here we need to decompress the name to get: variable_name, min, max
    log.debug("[TrigComboHypoToolFromDict] new combo hypo name: %s, topoInfo = %s", name, topoInfo)
    
    isLegMET = []
    for chId in range(len(chainDict['chainParts'])):
        if chainDict['chainParts'][chId]['signature'] == 'MET':
            isLegMET.append(True)
        else:
            isLegMET.append(False)            
        log.debug("[TrigComboHypoToolFromDict] chainParts[%i]: %s", chId, chainDict['chainParts'][chId])
        
    #the list of the variables reported below must match the one specified                                
    # here: Trigger/TrigHypothesis/TrigHypoCommonTools/src/TrigComboHypoTool.cxx::fillVarMap()
    # (and accordingly the list of the enums comboHypoVars in the header file)              
    allowed_obs = {
        'dR' : {
            'n_MET_legs' : [0],
            'hist_nbins' : 50,
            'hist_min'   : 0.,
            'hist_max'   : 5.
        },
        'invm' : {
            'n_MET_legs' : [0],
            'hist_nbins' : 100,
            'hist_min'   : 0.,
            'hist_max'   : 1000.
        },
        'dphi' : {
            'n_MET_legs' : [0,1,2],
            'hist_nbins' : 40,
            'hist_min'   : 0.,
            'hist_max'   : math.pi
        },
        'mT' : {
            'n_MET_legs' : [1],
            'hist_nbins' : 100,
            'hist_min'   : 0.,
            'hist_max'   : 1000.
        }
    }
    
    import re
    # get the variable
    obs_to_use = []
    for obs in allowed_obs.keys():
        if obs in topoInfo:
            obs_to_use.append(obs)
    if len(obs_to_use)!=1:
        log.error("[TrigComboHypoToolFromDict] N of vars found in he hypo name = %d in chain name %s", len(obs_to_use), name)
        raise Exception("[TrigComboHypoToolFromDict] N of vars found in the hypo name is different from 1")

    #get the limits
    l_min = re.findall(r"\d+"+obs_to_use[0], topoInfo)
    if len(l_min)==1:
        l_min[0] = l_min[0].replace(obs_to_use[0],"")
        if obs_to_use[0] in ['dR','dPhi']:
            cut_min = float(l_min[0])/10.
        else:
            cut_min = float(l_min[0])
    if len(l_min)>1:
        log.error("[TrigComboHypoToolFromDict] unable to get min value: N min = %d, l_min = %d", len(l_min), l_min)
        raise Exception("[TrigComboHypoToolFromDict] cannot set min value")

    if len(l_min)==1:#remove the min value from the string name
        l_max = re.findall(r"\d+", topoInfo.replace(l_min[0],""))
    else:#no min value was found
        l_max = re.findall(r"\d+", topoInfo)
    if len(l_max)>1:
        log.error("[TrigComboHypoToolFromDict] unable to get max value: N max = %d, l_max = %d", len(l_max), l_max)
        raise Exception("[TrigComboHypoToolFromDict] cannot set max value")
    if len(l_max)==1:
        if obs_to_use[0] in ['dR','dPhi']:
            cut_max = float(l_max[0])/10.
        else:
            cut_max = float(l_min[0])
        
    
    #get the legs
    l_names = topoInfo.replace(obs_to_use[0], "")
    if len(l_min)>0: 
        l_names = l_names.replace(l_min[0], "") 
    if len(l_max)>0: 
        l_names = l_names.replace(l_max[0], "") 

    if len(l_names)!=2:
        log.error("[TrigComboHypoToolFromDict] N_legs = %d, legs_name = %s", len(l_names), l_names)
        raise Exception("[TrigComboHypoToolFromDict] Number of legs is different from 2")

    legIndexes="ABCDEF"
    legA = -1
    legB = -1
    for i in range(len(legIndexes)):
        if legIndexes[i] == l_names[0]:
            legA = i
        elif legIndexes[i] == l_names[1]:
            legB = i
    if legA<0 or legB<0:
        log.error("[TrigComboHypoToolFromDict] Didn't find leg indexes in %s", l_names)
        raise Exception("[TrigComboHypoToolFromDict]  Didn't find leg indexes")
    
    #count the number of MET legs used in the hypo
    n_MET_legs=0
    if isLegMET[legA]:
        n_MET_legs += 1
    if isLegMET[legB]:
        n_MET_legs += 1
    #now check that the variable we plan to use allows the use of the MET
    if n_MET_legs not in allowed_obs[obs_to_use[0]]['n_MET_legs']:
        log.error("[TrigComboHypoToolFromDict] Attempting to use the MET leg in var %s. N_MET_legs = %d", obs_to_use[0], isLegMET.count(True))
        raise Exception("[TrigComboHypoToolFromDict] Attempting to use the MET leg in var")

    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram(obs_to_use[0]+'OfAccepted', type='TH1F', path='EXPERT', 
                                          title=obs_to_use[0]+" in accepted combinations; {}".format(obs_to_use[0]), 
                                          xbins=allowed_obs[obs_to_use[0]]['hist_nbins'], 
                                          xmin=allowed_obs[obs_to_use[0]]['hist_min'], 
                                          xmax=allowed_obs[obs_to_use[0]]['hist_max']), 
                          defineHistogram(obs_to_use[0]+'OfProcessed', type='TH1F', path='EXPERT', 
                                          title=obs_to_use[0]+" in processed combinations; {}".format(obs_to_use[0]), 
                                          xbins=allowed_obs[obs_to_use[0]]['hist_nbins'], 
                                          xmin=allowed_obs[obs_to_use[0]]['hist_min'], 
                                          xmax=allowed_obs[obs_to_use[0]]['hist_max'])]
    log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", name, topoInfo)

    tool= TrigComboHypoTool(name)
    tool.Variable    = obs_to_use[0]
    tool.LegA        = "leg{:03d}".format(legA)
    tool.IsLegA_MET  = isLegMET[legA]
    tool.LegB        = "leg{:03d}".format(legB)
    tool.IsLegB_MET  = isLegMET[legB]
    if len(l_min)==1:
        tool.UseMin    = True
        tool.LowerCut  = cut_min
    if len(l_max)==1:
        tool.UseMax    = True
        tool.UpperCut  = cut_max
    monTool.HistPath = 'ComboHypo/'+tool.getName() 
    tool.MonTool     = monTool
    #some debug info
    log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", name, topoInfo)
    log.debug("[TrigComboHypoToolFromDict] var  = %s", obs_to_use[0])
    log.debug("[TrigComboHypoToolFromDict] legA = %d", legA)
    log.debug("[TrigComboHypoToolFromDict] legB = %d", legB)
    if len(l_min)==1:
        log.debug("[TrigComboHypoToolFromDict] min  = %10.3f", cut_min)
    if len(l_max)==1:
        log.debug("[TrigComboHypoToolFromDict] max  = %10.3f", cut_max)

    return tool
