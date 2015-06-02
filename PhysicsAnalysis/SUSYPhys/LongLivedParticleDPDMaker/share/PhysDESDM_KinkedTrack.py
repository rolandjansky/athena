##-----------------------------------------------------------------------------
## Name: PhysDESDM_KinkedTrack.py
##
## Author: Shimpei Yamamoto
## Email:  shimpei.yamamoto@cern.ch
##
## Description: This defines the content of the RPVLL_KinkedTrack primary physics
##              ESD-basedDPD output stream.
##
##-----------------------------------------------------------------------------

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

#import AthenaCommon.SystemOfUnits as Units
jetContainer = primRPVLLDESDM.KinkedTrack_containerFlags.jetCollectionName
electronContainer = primRPVLLDESDM.KinkedTrack_containerFlags.electronCollectionName
muonContainer = primRPVLLDESDM.KinkedTrack_containerFlags.muonCollectionName
METContainer = primRPVLLDESDM.KinkedTrack_containerFlags.METCollectionName
egClusterContainer = primRPVLLDESDM.KinkedTrack_containerFlags.egClusterCollectionName
msTrackContainer = primRPVLLDESDM.KinkedTrack_containerFlags.msTrackCollectionName

def KinkTrkTriggerFilterString(flags):
    selectionString = ''
    if flags.triggerNames:
        triggers = list(set(flags.triggerNames))
        selectionString += '(' + ' || '.join(triggers) + ')'

    return selectionString



#====================================================================
# Skimming tools
#====================================================================
#KinkTrkJetTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkJetTriggerFilterTool",
#                                                                          expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags))
KinkTrkJetTriggerFilterTool = DerivationFramework__TriggerSkimmingTool(name = "KinkTrkJetTriggerFilterTool",
                                                                       TriggerListOR = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.triggerNames)
#print KinkTrkJetTriggerFilter
ToolSvc += KinkTrkJetTriggerFilterTool


from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__KinkTrkSingleJetMetFilterTool
KinkTrkSingleJetMetFilterTool = DerivationFramework__KinkTrkSingleJetMetFilterTool(name                 = "KinkTrkSingleJetMetFilterTool",
                                                                                   LeptonVeto           = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.doLeptonVeto,
                                                                                   JetContainerKey      = jetContainer,
                                                                                   ElectronContainerKey = electronContainer,
                                                                                   ElectronIDKey       = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.electronIDKey,
                                                                                   MuonContainerKey     = muonContainer,
                                                                                   MuonIDKey           = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.muonIDKey,
                                                                                   MetContainerKey      = METContainer,
                                                                                   MetTerm              = "FinalTrk", # or "FinalClus" ??
                                                                                   MetCut               = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetMin,
                                                                                   JetPtCut             = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEtMin,
                                                                                   JetEtaMax            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEtaMax,
                                                                                   LeptonPtCut          = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.leptonPtMax,
                                                                                   LeptonEtaMax         = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.leptonEtaMax)

print KinkTrkSingleJetMetFilterTool
ToolSvc += KinkTrkSingleJetMetFilterTool


KinkTrkJetFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkJetFilterTool",
                                                                 FilterList=[KinkTrkJetTriggerFilterTool, KinkTrkSingleJetMetFilterTool],
                                                                 OutputLevel=INFO)
ToolSvc+= KinkTrkJetFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackJetFilterKernel",
                                                     SkimmingTools = [KinkTrkJetFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackJetFilterKernel"])



### Filters for Zll
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__KinkTrkZeeTagTool
KinkTrkZeeTagTool = DerivationFramework__KinkTrkZeeTagTool(name                 = "KinkTrkZeeTagTool",
                                                           Triggers             = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.triggerNames,
                                                           TriggerMatchDeltaR   = 0.1,
                                                           RequireTriggerMatch  = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.doTriggerMatch,
                                                           ElectronContainerKey = electronContainer,
                                                           ElectronIDKeys       = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.electronIDKeys,
                                                           ElectronPtMin        = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.electronPtMin,
                                                           ElectronEtaMax       = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.electronEtaMax,
                                                           ClusterContainerKey  = egClusterContainer,
                                                           ClusterEtMin         = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.clusterEtMin,
                                                           ClusterEtaMax        = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.clusterEtaMax,
                                                           DiEleMassLow         = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.diElectronMassLow,
                                                           DiEleMassHigh        = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.diElectronMassHigh,
                                                           DeltaPhiMax          = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.deltaPhiMax,
                                                           StoreGateKeyPrefix   = "KinkTrk")

print KinkTrkZeeTagTool
ToolSvc += KinkTrkZeeTagTool

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__KinkTrkZmumuTagTool
KinkTrkZmumuTagTool = DerivationFramework__KinkTrkZmumuTagTool(name            = "KinkTrkZmumuTagTool",
                                                           Triggers            = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.triggerNames,
                                                           TriggerMatchDeltaR  = 0.1,
                                                           RequireTriggerMatch = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.doTriggerMatch,
                                                           MuonContainerKey    = muonContainer,
                                                           MuonIDKeys          = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.muonIDKeys,
                                                           MuonPtMin           = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.muonPtMin,
                                                           MuonEtaMax          = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.muonEtaMax,
                                                           TrackContainerKey   = msTrackContainer,
                                                           TrackPtMin          = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.trackPtMin,
                                                           TrackEtaMax         = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.trackEtaMax,
                                                           DiMuonMassLow       = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.diMuonMassLow,
                                                           DiMuonMassHigh      = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.diMuonMassHigh,
                                                           DeltaPhiMax         = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.deltaPhiMax,
                                                           StoreGateKeyPrefix  = "KinkTrk")

print KinkTrkZmumuTagTool
ToolSvc += KinkTrkZmumuTagTool

# Kernel for the augmentation tools
topSequence += DerivationFramework__DerivationKernel( "KinkTrkAugmentationKernel",
                                                      AugmentationTools = [KinkTrkZeeTagTool, KinkTrkZmumuTagTool])

KinkTrkZeeTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeeTriggerFilterTool",
                                                                          expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_ZeeFilterFlags))
#KinkTrkZeeTriggerFilterTool = DerivationFramework__TriggerSkimmingTool(name = "KinkTrkZeeTriggerFilterTool",
#                                                                       TriggerListOR = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.triggerNames)
ToolSvc += KinkTrkZeeTriggerFilterTool

KinkTrkZmumuTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuTriggerFilterTool",
                                                                            expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags))
#KinkTrkZmumuTriggerFilterTool = DerivationFramework__TriggerSkimmingTool(name = "KinkTrkZmumuTriggerFilterTool",
#                                                                       TriggerListOR = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.triggerNames)
ToolSvc += KinkTrkZmumuTriggerFilterTool

KinkTrkZeeStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeellStringSkimmingTool",
                                                                           expression = 'count(abs(KinkTrkDiEleMass)>0)>=1')
ToolSvc += KinkTrkZeeStringSkimmingTool

KinkTrkZeeFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZeeFilterTool",
                                                                 FilterList=[KinkTrkZeeTriggerFilterTool, KinkTrkZeeStringSkimmingTool],
                                                                 OutputLevel=INFO)
ToolSvc+= KinkTrkZeeFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackZeeFilterKernel",
                                                     SkimmingTools = [KinkTrkZeeFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackZeeFilterKernel"])

KinkTrkZmumuStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuStringSkimmingTool",
                                                                             expression = 'count(abs(KinkTrkDiMuMass)>0)>=1')
ToolSvc += KinkTrkZmumuStringSkimmingTool

KinkTrkZmumuFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZmumuFilterTool",
                                                                 FilterList=[KinkTrkZmumuTriggerFilterTool, KinkTrkZmumuStringSkimmingTool],
                                                                 OutputLevel=INFO)
ToolSvc+= KinkTrkZmumuFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackZmumuFilterKernel",
                                                     SkimmingTools = [KinkTrkZmumuFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackZmumuFilterKernel"])

