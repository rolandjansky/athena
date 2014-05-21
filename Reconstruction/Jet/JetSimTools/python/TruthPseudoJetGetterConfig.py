# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetSimTools.JetSimToolsConf import TruthPseudoJetGetter, JetTruthParticleSelectorTool

def getJetTruthParticleSelectorTool(selectionMode="StableNoMuonNoNu" , **options):
    toolName = "JetTruthSelector"+selectionMode
    selecTool = JetTruthParticleSelectorTool(toolName,
                                             SelectionMode = selectionMode,
                                             )
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += selecTool
    return selecTool

from AthenaCommon.AppMgr import ToolSvc

ToolSvc += JetTruthParticleSelectorTool("JetTruthSelectorStableNoMuonNoNu",
                                        SelectionMode = "StableNoMuonNoNu"
                                        )


## def getTruthPseudoJetGetter(input, asGhost, **options):

##     inCont = "TruthParticle"

##     prefix = "Ghost" if asGhost else ""

##     outCont = prefix+"PseudoJet_"+inCont

    
##     tool = TruthPseudoJetGetter( "PJGetter"+prefix+inCont,
##                                  InputContainer = inCont,
##                                  OutputContainer = outCont,
##                                  TruthSelector = getJetTruthParticleSelectorTool(**options),
##                                  SkipNegativeEnergy = True)

##     from AthenaCommon.AppMgr import ToolSvc
##     ToolSvc += tool
##     return tool
