# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration                                                                                                         


import AthenaCommon.SystemOfUnits as Units


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

def VHTrkTriggerFilterString(flags):
    selectionString = ''
    if flags.triggerNames:
        triggers = list(set(flags.triggerNames))
        selectionString += '(' + ' || '.join(triggers) + ')'

    return selectionString



VHJetTriggerFilterTool = DerivationFramework__TriggerSkimmingTool( name = "VHJetTriggerFilterTool",
                                                                   TriggerListOR = primRPVLLDESDM.VHLowTrackJet_FilterFlags.triggerNames)

ToolSvc += VHJetTriggerFilterTool


from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__VHLowTrackJetFilterTool

VHLowTrackJetFilterTool=DerivationFramework__VHLowTrackJetFilterTool(name="VHLowTrackJetFilterTool",
                                                                     Debug         =primRPVLLDESDM.VHLowTrackJet_FilterFlags.debug,
                                                                     JetPtCut      =primRPVLLDESDM.VHLowTrackJet_FilterFlags.cutJet ,
                                                                     JetEtaCut     =primRPVLLDESDM.VHLowTrackJet_FilterFlags.cutJetEta,
                                                                     NJetsRequired =primRPVLLDESDM.VHLowTrackJet_FilterFlags.nJets ,
                                                                     ElectronPtCut =primRPVLLDESDM.VHLowTrackJet_FilterFlags.cutEl ,
                                                                     MuonPtCut     =primRPVLLDESDM.VHLowTrackJet_FilterFlags.cutMu )

ToolSvc += VHLowTrackJetFilterTool

VHLTrkJetFilterTool=DerivationFramework__FilterCombinationAND(name="VHLTrkJetFilterTool",
                                                              FilterList=[VHJetTriggerFilterTool,VHLowTrackJetFilterTool],
                                                              OutputLevel=INFO)

ToolSvc += VHLTrkJetFilterTool

topSequence += DerivationFramework__DerivationKernel( "RPVLL_VHLowTrackJetFilterKernel",
                       SkimmingTools = [VHLTrkJetFilterTool] )

RPVLLfilterNames.extend(["RPVLL_VHLowTrackJetFilterKernel"])
