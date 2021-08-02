# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

############################################################
# Functionality to add generic topo hypos to combined chains
############################################################

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
logging.getLogger().info("Importing %s",__name__)
import math

topoLegIndices = "ABCDEF"

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

from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasChainConfiguration import TrigAFPDijetComboHypoToolCfg

def TrigComboHypoToolFromDict(chainDict):
    from TrigHypoCommonTools.TrigHypoCommonToolsConf import TrigComboHypoTool
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    name     = chainDict['chainName']
    log.debug("[TrigComboHypoToolFromDict] chain %s, combo hypos to be processed: %s, t", name, chainDict['extraComboHypos'])
    #define the list for housing all the info needed to initialize the TrigComboHypoTool module in the form of a dict
    topoDefs = []

    for topoID in range(len(chainDict['extraComboHypos'])):
        topoInfo = chainDict['extraComboHypos'][topoID]
        #here we need to decompress the name to get: variable_name, min, max
        log.debug("[TrigComboHypoToolFromDict] new combo hypo name: %s, topoInfo = %s", name, topoInfo)
        singleTopoDef = {}

        isLegMET = []
        for chId in range(len(chainDict['chainParts'])):
            if chainDict['chainParts'][chId]['signature'] == 'MET':
                isLegMET.append(True)
            else:
                isLegMET.append(False)            
            log.debug("[TrigComboHypoToolFromDict] chainParts[%i]: %s", chId, chainDict['chainParts'][chId])
        
        import re
        # get the variable
        obs_to_use = []
        for obs in allowed_obs.keys():
            if obs in topoInfo:
                obs_to_use.append(obs)
        if len(obs_to_use)!=1:
            log.error("[TrigComboHypoToolFromDict] N of vars found in he hypo name = %d in chain name %s", len(obs_to_use), name)
            raise Exception("[TrigComboHypoToolFromDict] N of vars found in the hypo name is different from 1")
        singleTopoDef['var'] = obs_to_use[0]

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

        legA = -1
        legB = -1
        for i in range(len(topoLegIndices)):
            if topoLegIndices[i] == l_names[0]:
                legA = i
            elif topoLegIndices[i] == l_names[1]:
                legB = i
        if legA<0 or legB<0:
            log.error("[TrigComboHypoToolFromDict] Didn't find leg indexes in %s", l_names)
            raise Exception("[TrigComboHypoToolFromDict]  Didn't find leg indexes")
        singleTopoDef['legA'] = "leg{:03d}".format(legA)
        singleTopoDef['legB'] = "leg{:03d}".format(legB)

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
        singleTopoDef['isLegA_MET'] = isLegMET[legA]
        singleTopoDef['isLegB_MET'] = isLegMET[legB]

        if len(chainDict['extraComboHypos'])==1:#to avoid breaking changes in the ref files
            monToolName = "MonTool_"+name
            histNameTag = obs_to_use[0]
        else:
            monToolName = "MonTool_"+name+"_"+chainDict['extraComboHypos'][topoID]
            histNameTag = obs_to_use[0] + "leg{:03d}".format(legA) + "leg{:03d}".format(legB)
        monTool = GenericMonitoringTool(monToolName)
        monTool.Histograms = [defineHistogram(histNameTag+'OfAccepted', type='TH1F', path='EXPERT', 
                                              title=obs_to_use[0]+" in accepted combinations; {}".format(obs_to_use[0]), 
                                              xbins=allowed_obs[obs_to_use[0]]['hist_nbins'], 
                                              xmin=allowed_obs[obs_to_use[0]]['hist_min'], 
                                              xmax=allowed_obs[obs_to_use[0]]['hist_max']), 
                              defineHistogram(histNameTag+'OfProcessed', type='TH1F', path='EXPERT', 
                                              title=obs_to_use[0]+" in processed combinations; {}".format(obs_to_use[0]), 
                                              xbins=allowed_obs[obs_to_use[0]]['hist_nbins'], 
                                              xmin=allowed_obs[obs_to_use[0]]['hist_min'], 
                                              xmax=allowed_obs[obs_to_use[0]]['hist_max'])]
        log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", name, topoInfo)
        #now fill the holders needed to initialize the TrigComboHypoTool
        if len(l_min)==1:
            singleTopoDef['useMin']   = True
            singleTopoDef['lowerCut'] = cut_min
        else:
            singleTopoDef['useMin']   = False
            singleTopoDef['lowerCut'] = 0.
            
        if len(l_max)==1:
            singleTopoDef['useMax']   = True
            singleTopoDef['upperCut'] = cut_max
        else:
            singleTopoDef['useMax']   = False
            singleTopoDef['upperCut'] = 0.

        if len(chainDict['extraComboHypos'])==1:#to avoid breaking changes in the ref files
            monTool.HistPath = 'ComboHypo/'+name
        else:
            monTool.HistPath = 'ComboHypo/'+name+"_detail_"+singleTopoDef['var'] + singleTopoDef['legA'] + singleTopoDef['legB']
        singleTopoDef['monTool'] = monTool

        topoDefs.append(singleTopoDef)

        #some debug info
        log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", name, topoInfo)
        log.debug("[TrigComboHypoToolFromDict] var  = %s", singleTopoDef['var'])
        log.debug("[TrigComboHypoToolFromDict] legA = %d", singleTopoDef['legA'])
        log.debug("[TrigComboHypoToolFromDict] legB = %d", singleTopoDef['legB'])
        if len(l_min)==1:
            log.debug("[TrigComboHypoToolFromDict] min  = %10.3f", singleTopoDef['lowerCut'])
        if len(l_max)==1:
            log.debug("[TrigComboHypoToolFromDict] max  = %10.3f", singleTopoDef['upperCut'])

        #end of the loop over the hypos
    tool= TrigComboHypoTool(name)
    tool.Variables      = [x['var']        for x in topoDefs]
    tool.LegAVec        = [x['legA']       for x in topoDefs]
    tool.IsLegA_METVec  = [x['isLegA_MET'] for x in topoDefs]
    tool.LegBVec        = [x['legB']       for x in topoDefs]
    tool.IsLegB_METVec  = [x['isLegB_MET'] for x in topoDefs]
    tool.UseMinVec      = [x['useMin']     for x in topoDefs]
    tool.LowerCutVec    = [x['lowerCut']   for x in topoDefs]
    tool.UseMaxVec      = [x['useMax']     for x in topoDefs]
    tool.UpperCutVec    = [x['upperCut']   for x in topoDefs]
    tool.MonTools       = [x['monTool']    for x in topoDefs]

    return tool

comboConfigurator = {
    'dR':TrigComboHypoToolFromDict,
    'dphi':TrigComboHypoToolFromDict,
    'invm':TrigComboHypoToolFromDict,
    'mT':TrigComboHypoToolFromDict,
    'afpdijet':TrigAFPDijetComboHypoToolCfg,
}

def addTopoInfo(theChainConfig, mainChainDict, listOfChainDefs, lengthOfChainConfigs):

    def findStepIndexInChain(chain, step):
        for istep,chainstep in enumerate(chain.steps):
            if chainstep.name==step:
                return istep
        return None

    for step,(topoCfg,topoExpr) in theChainConfig.topoMap.items():
        thestep = -1 if step=="last" else findStepIndexInChain(theChainConfig,step)
        if thestep is None:
            log.error("Failed to find step %s in Chain! ChainDict follows:", step)
            log.error(mainChainDict)
            raise RuntimeError("Step not found when adding topo to chain")

        bonus_debug = False

        if bonus_debug:
            log.debug("[addTopoInfo] theChainConfig %s", theChainConfig)
            log.debug("[addTopoInfo] listOfChainDefs %s", listOfChainDefs)
            log.debug("[addTopoInfo] theTopoInfo being added is %s",topoExpr)

        # No need to add if it has been added previously
        # Handle better and avoid doing this repeatedly on the same steps?
        if topoCfg not in theChainConfig.steps[thestep].comboToolConfs:
            if len(theChainConfig.steps[thestep].comboToolConfs) > 0:
                log.warning("[addTopoInfo] step %s already has ComboHypo tools %s",theChainConfig.steps[thestep],theChainConfig.steps[thestep].comboToolConfs)
                log.warning("[addTopoInfo] these will be added to, make sure this is the behaviour you want.")

            theChainConfig.steps[thestep].addComboHypoTools(topoCfg)
            theChainConfig.steps[thestep].name = theChainConfig.steps[thestep].name+'_combo_'+topoExpr 
    
            theChainConfig.steps[thestep].makeCombo()
            log.debug("[addTopoInfo] new combo hypo name: %s",theChainConfig.steps[thestep].combo.name)

            if bonus_debug:
                log.debug("[addTopoInfo] new theChainConfig %s", theChainConfig)
