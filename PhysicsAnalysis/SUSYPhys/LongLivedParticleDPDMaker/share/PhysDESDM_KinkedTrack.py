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
       selectionString += '(' + ' || '.join(flags.triggerNames) + ')'

    return selectionString



#====================================================================
# Skimming tools
#====================================================================
KinkTrkJetTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkJetTriggerFilterTool",
                                                                          expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags))
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
                                                                 OutputLevel=DEBUG)
ToolSvc+= KinkTrkJetFilterTool



KinkTrkZeeTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeeTriggerFilterTool",
                                                                          expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_ZeeFilterFlags))
ToolSvc += KinkTrkZeeTriggerFilterTool

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

KinkTrkZeeStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeellStringSkimmingTool",
                                                                           expression = 'count(abs(KinkTrkDiEleMass)>0)>=1')
ToolSvc += KinkTrkZeeStringSkimmingTool

KinkTrkZeeFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZeeFilterTool",
                                                                 FilterList=[KinkTrkZeeTriggerFilterTool, KinkTrkZeeStringSkimmingTool],
                                                                 OutputLevel=DEBUG)
ToolSvc+= KinkTrkZeeFilterTool


KinkTrkZmumuTriggerFilterTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuTriggerFilterTool",
                                                                            expression = KinkTrkTriggerFilterString(primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags))
ToolSvc += KinkTrkZmumuTriggerFilterTool

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
                                                           DiMuonMassHigh       = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.diMuonMassHigh,
                                                           DeltaPhiMax         = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.deltaPhiMax,
                                                           StoreGateKeyPrefix  = "KinkTrk")

print KinkTrkZmumuTagTool
ToolSvc += KinkTrkZmumuTagTool

KinkTrkZmumuStringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuStringSkimmingTool",
                                                                             expression = 'count(abs(KinkTrkDiMuMass)>0)>=1')
ToolSvc += KinkTrkZmumuStringSkimmingTool

KinkTrkZmumuFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZmumuFilterTool",
                                                                 FilterList=[KinkTrkZmumuTriggerFilterTool, KinkTrkZmumuStringSkimmingTool],
                                                                 OutputLevel=DEBUG)
ToolSvc+= KinkTrkZmumuFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackFilterKernel",
                                                     SkimmingTools = [KinkTrkJetFilterTool, KinkTrkZeeFilterTool, KinkTrkZmumuFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackFilterKernel"])

