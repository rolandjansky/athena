# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

############################################################
# Functionality to add generic topo hypos to combined chains
############################################################

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
logging.getLogger().info("Importing %s",__name__)
import math
import re
from TrigConfHLTUtils.HLTUtils import string2hash

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
    'deta' : {
        'n_MET_legs' : [0,1,2],
        'hist_nbins' : 50,
        'hist_min'   : 0.,
        'hist_max'   : 10.
    },
    'mT' : {
        'n_MET_legs' : [1],
        'hist_nbins' : 100,
        'hist_min'   : 0.,
        'hist_max'   : 1000.
    }
}

from TriggerMenuMT.HLT.MinBias.MinBiasChainConfiguration import TrigAFPDijetComboHypoToolCfg

def TrigComboHypoToolFromDict(chainDict):
    from TrigHypoCommonTools.TrigHypoCommonToolsConf import TrigComboHypoTool
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    chainName = chainDict['chainName']
    log.debug("[TrigComboHypoToolFromDict] chain %s, combo hypos to be processed: %s, t", chainName, chainDict['extraComboHypos'])
    #define the list for housing all the info needed to initialize the TrigComboHypoTool module in the form of a dict
    topoDefs = []

    # Define regex for parsing the topoInfo
    # Pattern is: min cut, var, legA, legB, max cut
    # Min and max are both optional, check afterwards that at least one is filled
    # Only the allowed vars and legs will be recognised, anything else fails to match
    theregex = fr"(\d*)({'|'.join(allowed_obs.keys())})([{topoLegIndices}])([{topoLegIndices}])(\d*)"
    matcher = re.compile(theregex)

    for iTopo, topoInfo in enumerate(chainDict['extraComboHypos']):
        log.debug("[TrigComboHypoToolFromDict] new combo hypo for chain: %s, topoInfo = %s", chainName, topoInfo)
        # Attempt to regex-match the topo specification
        result = matcher.match(topoInfo)
        if not result:
            log.error("[TrigComboHypoToolFromDict] Topo expression %s does not conform to format (min?)(var)(legA)(legB)(max?).",topoInfo)
            log.error("[TrigComboHypoToolFromDict] Must use leg IDs in %s, vars in {allowed_obs.keys()}",topoLegIndices)
            raise ValueError(f"[TrigComboHypoToolFromDict] Invalid topo expression {topoInfo} received in 'extraComboHypos'!")

        # Extract the matched info and validate
        str_min, var, char_legA, char_legB, str_max = result.groups()
        # Manipulation of the cuts
        # At least one must be filled
        use_min = bool(str_min)
        use_max = bool(str_max)
        if not (use_min or use_max):
            log.error("[TrigComboHypoToolFromDict] Topo expression %s does not specify a min or max cut value.",topoInfo)
            raise ValueError(f"[TrigComboHypoToolFromDict] Invalid topo expression {topoInfo} received in 'extraComboHypos'!")
        # Convert into float values, dividing for 0.1 precision as needed
        if var in ['dR','dphi','deta']:
            cut_min = float(str_min)/10. if use_min else float('nan')
            cut_max = float(str_max)/10. if use_max else float('nan')
        else:
            cut_min = float(str_min) if use_min else float('nan')
            cut_max = float(str_max) if use_max else float('nan')

        # Convert char leg representation to int
        i_legA = topoLegIndices.find(char_legA)
        i_legB = topoLegIndices.find(char_legB)

        # Fill info for each leg, looking up in chainParts
        # Convert leg name into HLT identifier for matching in the tool
        legInfo = []
        for ileg in [i_legA,i_legB]:
            cpart = chainDict['chainParts'][ileg]
            legname = f"leg{ileg:03d}_{chainName}"
            legInfo.append({
                'index'       : ileg,
                'legname'     : legname,
                'HLTId'       : string2hash(legname),
                'isMET'       : cpart['signature']=='MET',
                'multiplicity': int(cpart['multiplicity'])
            })

        # Count how many input legs are MET, for consistency checks
        n_MET_legs = legInfo[0]['isMET'] + legInfo[1]['isMET']

        # Check multiplicity of the configured legs
        # For chains like "HLT_2muX_10invm70AA", no leg ID will be attached
        # in which case set a flag to use all objects in the combination list
        skipLegCheck = False
        if i_legA==i_legB:
            if legInfo[0]['multiplicity'] != 2:
                log.error("[TrigComboHypoToolFromDict] Error configuring topo for chain %s!",chainName)
                log.error("[TrigComboHypoToolFromDict] Topo selection %s requires multiplicity 2 on leg %d, found %d!",topoInfo,i_legA,legInfo[0]['multiplicity'])
                raise Exception("[TrigComboHypoToolFromDict] Invalid multiplicity")
            if n_MET_legs==2:
                log.error("[TrigComboHypoToolFromDict] Configured with the same MET leg on both sides -- impossible to satisfy!")
                raise Exception("[TrigComboHypoToolFromDict] Identical MET legs for topo selection")
            if len(chainDict['chainParts'])==1:
                skipLegCheck=True
        else:
            for li in legInfo:
                if li['multiplicity'] != 1:
                    log.error("[TrigComboHypoToolFromDict] Error configuring topo for chain %s!",chainName)
                    log.error("[TrigComboHypoToolFromDict] Topo selection %s requires multiplicity 1 on leg %d, found %d!",topoInfo,li['index'],li['multiplicity'])
                    raise Exception("[TrigComboHypoToolFromDict] Invalid multiplicity")

        #now check that the variable we plan to use allows the use of the MET
        if n_MET_legs not in allowed_obs[var]['n_MET_legs']:
            log.error("[TrigComboHypoToolFromDict] Attempting var %s with %d MET legs, %s allowed", var, n_MET_legs, allowed_obs[var]['n_MET_legs'])
            raise Exception("[TrigComboHypoToolFromDict] Attempting to use the MET leg in var")
 
        if len(chainDict['extraComboHypos'])==1:#to avoid breaking changes in the ref files
            monToolName = "MonTool_"+chainName
            histNameTag = var
        else:
            monToolName = f"MonTool_{chainName}_{chainDict['extraComboHypos'][iTopo]}"
            histNameTag = f"{var}leg{i_legA:03d}leg{i_legB:03d}"
        monTool = GenericMonitoringTool(monToolName)
        monTool.Histograms = [defineHistogram(histNameTag+'OfAccepted', type='TH1F', path='EXPERT',
                                              title=var+" in accepted combinations; {}".format(var),
                                              xbins=allowed_obs[var]['hist_nbins'],
                                              xmin=allowed_obs[var]['hist_min'],
                                              xmax=allowed_obs[var]['hist_max']),
                              defineHistogram(histNameTag+'OfProcessed', type='TH1F', path='EXPERT',
                                              title=var+" in processed combinations; {}".format(var),
                                              xbins=allowed_obs[var]['hist_nbins'],
                                              xmin=allowed_obs[var]['hist_min'],
                                              xmax=allowed_obs[var]['hist_max'])]
        log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", chainName, topoInfo)
        log.debug("[TrigComboHypoToolFromDict] histName = %s", histNameTag)

        if len(chainDict['extraComboHypos'])==1:#to avoid breaking changes in the ref files
            monTool.HistPath = f'ComboHypo/{chainName}'
        else:
            monTool.HistPath = f'ComboHypo/{chainName}_detail_{histNameTag}'

        # Set keys of dict to match tool config properties
        singleTopoDef = {
            "Variables"    : var,
            "UseMinVec"    : use_min,
            "UseMaxVec"    : use_max,
            "LowerCutVec"  : cut_min,
            "UpperCutVec"  : cut_max,
            "LegAVec"      : legInfo[0]["HLTId"],
            "LegBVec"      : legInfo[1]["HLTId"],
            "IsLegA_METVec": legInfo[0]["isMET"],
            "IsLegB_METVec": legInfo[1]["isMET"],
            "MonTools"     : monTool,
        }
        topoDefs.append(singleTopoDef)

        #some debug info
        log.debug("[TrigComboHypoToolFromDict] tool configured for hypo name: %s, topoInfo = %s", chainName, topoInfo)
        log.debug("[TrigComboHypoToolFromDict] var  = %s", singleTopoDef['Variables'])
        log.debug("[TrigComboHypoToolFromDict] legA = %s", singleTopoDef['LegAVec'])
        log.debug("[TrigComboHypoToolFromDict] legB = %s", singleTopoDef['LegBVec'])
        if use_min:
            log.debug("[TrigComboHypoToolFromDict] min  = %10.3f", singleTopoDef['LowerCutVec'])
        if use_max:
            log.debug("[TrigComboHypoToolFromDict] max  = %10.3f", singleTopoDef['UpperCutVec'])

        #end of the loop over the hypos
    
    # convert list of dicts into dict of lists
    toolProps = {k:[thedef[k] for thedef in topoDefs] for k in topoDefs[0]}
    tool = TrigComboHypoTool(chainName, SkipLegCheck=skipLegCheck, **toolProps)

    return tool

comboConfigurator = {
    'dR':TrigComboHypoToolFromDict,
    'dphi':TrigComboHypoToolFromDict,
    'deta':TrigComboHypoToolFromDict,
    'invm':TrigComboHypoToolFromDict,
    'mT':TrigComboHypoToolFromDict,
    'afpdijet':TrigAFPDijetComboHypoToolCfg,
}

def addTopoInfo(theChainConfig, mainChainDict, listOfChainDefs, lengthOfChainConfigs):
    log.debug("[addTopoInfo] Adding topo info to chain %s", theChainConfig)

    def findStepIndexInChain(chain, step):
        for istep,chainstep in enumerate(chain.steps):
            if chainstep.name==step:
                return istep
        return None

    for step,(topoCfg,topoExpr) in theChainConfig.topoMap.items():
        thestep = theChainConfig.steps[-1] if step=="last" else theChainConfig.steps[findStepIndexInChain(theChainConfig,step)]
        log.debug("[addTopoInfo] Adding %s to step %s",topoExpr,thestep)
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
        if topoCfg not in thestep.comboToolConfs:
            if len(thestep.comboToolConfs) > 0:
                log.warning("[addTopoInfo] step %s already has ComboHypo tools %s",thestep,thestep.comboToolConfs)
                log.warning("[addTopoInfo] these will be added to, make sure this is the behaviour you want.")

            thestep.name = thestep.name+'_combo_'+topoExpr 
            thestep.addComboHypoTools(topoCfg)
    
            thestep.makeCombo()
            log.debug("[addTopoInfo] new combo hypo name: %s",thestep.combo.name)

            if bonus_debug:
                log.debug("[addTopoInfo] new theChainConfig %s", theChainConfig)
