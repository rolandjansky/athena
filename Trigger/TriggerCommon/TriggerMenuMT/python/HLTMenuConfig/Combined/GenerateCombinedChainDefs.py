# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

############################################################
# Functionality to add generic topo hypos to combined chains
############################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Combined' )
logging.getLogger().info("Importing %s",__name__)

def addTopoInfo(theChainConfig, chainDict, listOfChainDefs, lengthOfChainConfigs):
    
    theTopoInfo = chainDict['extraComboHypos']
    
    log.debug("[addTopoInfo] theChainConfig %s", theChainConfig)
    log.debug("[addTopoInfo] listOfChainDefs %s", listOfChainDefs)
    log.debug("[addTopoInfo] theTopoInfo being added is %s",theTopoInfo)

    if len(theChainConfig.steps[-1].comboToolConfs) > 0:
        log.warning("[addTopoInfo] last step already has ComboHypo tools %s",theChainConfig.steps[-1].comboToolConfs)
        log.warning("[addTopoInfo] these will be added to, make sure this is the behaviour you want.")

    comboTools = []
    for topoInfo in theTopoInfo:
        if topoInfo == '03dRtt':
            comboTools += [TrigTauXComboHypoToolFromDict]
        else:
            log.error("[addTopoInfo] does not yet know what to do with topo %s",theTopoInfo)
            raise Exception("[addTopoInfo] cannot proceed, exiting.")

    for comboTool in comboTools:
        theChainConfig.steps[-1].addComboHypoTools(comboTool)
    theChainConfig.steps[-1].name = theChainConfig.steps[-1].name+'_combo_'+'_'.join(theTopoInfo) 
    
#    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
#    theChainConfig.steps[-1].combo.name = CFNaming.comboHypoName(theChainConfig.steps[-1].name)
    theChainConfig.steps[-1].makeCombo()
    log.debug("[addTopoInfo] new combo hypo name: %s",theChainConfig.steps[-1].combo.name)
    log.debug("[addTopoInfo] theChainConfig %s", theChainConfig)

    return theChainConfig

# this must be moved to the HypoTool file:                                                                                                 
def TrigTauXComboHypoToolFromDict(chainDict):
    from TrigTauHypo.TrigTauHypoConf import TrigTauXComboHypoTool
    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('dROfAccepted', type='TH1F', path='EXPERT', title="dR in accepted combinations [MeV]", xbins=50, xmin=0, xmax=5.)]
    monTool.Histograms = [defineHistogram('dROfProcessed', type='TH1F', path='EXPERT', title="dR in accepted combinations [MeV]", xbins=50, xmin=0, xmax=5.)]
    tool= TrigTauXComboHypoTool(name)
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool
