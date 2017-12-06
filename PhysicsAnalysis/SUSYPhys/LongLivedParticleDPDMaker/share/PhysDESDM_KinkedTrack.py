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
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool

#import AthenaCommon.SystemOfUnits as Units
jetContainer = primRPVLLDESDM.KinkedTrack_containerFlags.jetCollectionName
electronContainer = primRPVLLDESDM.KinkedTrack_containerFlags.electronCollectionName
muonContainer = primRPVLLDESDM.KinkedTrack_containerFlags.muonCollectionName
METContainer = primRPVLLDESDM.KinkedTrack_containerFlags.METCollectionName
METTerm = primRPVLLDESDM.KinkedTrack_containerFlags.METTermName
egClusterContainer = primRPVLLDESDM.KinkedTrack_containerFlags.egClusterCollectionName
msTrackContainer = primRPVLLDESDM.KinkedTrack_containerFlags.msTrackCollectionName

def KinkTrkTriggerFilterString(flags):
    selectionString = ''
    if flags.triggerNames:
        triggers = list(set(flags.triggerNames))
        selectionString += '(' + ' || '.join(triggers) + ')'

    return selectionString




#====================================================================
# JetMetFilter
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
                                                                                   IsolatedTrack        = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.requireIsolatedTrack,
                                                                                   JetContainerKey      = jetContainer,
                                                                                   ElectronContainerKey = electronContainer,
                                                                                   ElectronIDKey        = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.electronIDKey,
                                                                                   MuonContainerKey     = muonContainer,
                                                                                   MuonIDKey            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.muonIDKey,
                                                                                   MetContainerKey      = METContainer,
                                                                                   MetTerm              = METTerm,
                                                                                   MetCut               = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetMin,
                                                                                   JetPtCuts            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutsEtMin,
                                                                                   JetEtaMax            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEtaMax,
                                                                                   JetNumCut            = 1,
                                                                                   JetMetDphiMin        = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.jetMetDphiMin,
                                                                                   JetMetPtMin          = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.jetMetPtMin,
                                                                                   MetHtCut             = -1,
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

KinkTrkMultiJetFilterTool = DerivationFramework__KinkTrkSingleJetMetFilterTool(name                 = "KinkTrkMultiJetFilterTool",
                                                                               LeptonVeto           = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.doLeptonVeto,
                                                                               IsolatedTrack        = False,
                                                                               JetContainerKey      = jetContainer,
                                                                               ElectronContainerKey = electronContainer,
                                                                               ElectronIDKey        = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.electronIDKey,
                                                                               MuonContainerKey     = muonContainer,
                                                                               MuonIDKey            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.muonIDKey,
                                                                               MetContainerKey      = METContainer,
                                                                               MetTerm              = METTerm,
                                                                               MetCut               = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetMinForMultiJets,
                                                                               JetPtCuts            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutsEtMinForMultiJets,
                                                                               JetEtaMax            = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutEtaMax,
                                                                               JetNumCut            = 2,
                                                                               JetMetDphiMin        = 0.2,
                                                                               JetMetPtMin          = 50.0*Units.GeV,
                                                                               MetHtCut             = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.cutMetHt,
                                                                               LeptonPtCut          = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.leptonPtMax,
                                                                               LeptonEtaMax         = primRPVLLDESDM.KinkedTrack_singleJetMetFilterFlags.leptonEtaMax)

print KinkTrkMultiJetFilterTool
ToolSvc += KinkTrkMultiJetFilterTool


KinkTrkTrigMetMultiJetFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkTrigMetMultiJetFilterTool",
                                                                 FilterList=[KinkTrkJetTriggerFilterTool, KinkTrkMultiJetFilterTool],
                                                                 OutputLevel=INFO)
ToolSvc+= KinkTrkTrigMetMultiJetFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackMultiJetFilterKernel",
                                                     SkimmingTools = [KinkTrkTrigMetMultiJetFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackMultiJetFilterKernel"])


#====================================================================
# Zee/Zmumu filter
#====================================================================
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

## Kernel for the augmentation tools
topSequence += DerivationFramework__DerivationKernel( "KinkTrkAugmentationKernel",
                                                      AugmentationTools = [KinkTrkZeeTagTool, KinkTrkZmumuTagTool])

## Trigger filters
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


## Selecting low-pT probe electrons
KinkTrkZeeLowPtSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeeLowPtSkimmingTool",
                                                                          expression = 'count(abs(KinkTrkDiEleMass)>0)>=1 && count(KinkTrkProbeEleEt>=%f)==0' % primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.preScaleClusterEtMax)
ToolSvc += KinkTrkZeeLowPtSkimmingTool

## Selecting high-pT probe electrons
KinkTrkZeeHighPtSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZeeHighPtSkimmingTool",
                                                                           expression = 'count(abs(KinkTrkDiEleMass)>0)>=1 && count(KinkTrkProbeEleEt>=%f)>=1' % primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.preScaleClusterEtMax)
ToolSvc += KinkTrkZeeHighPtSkimmingTool

## Prescale for low-pT probes
KinkTrkZeePrescaleTool = DerivationFramework__PrescaleTool(name = "KinkTrkZeePrescaleTool",
                                                           Prescale = primRPVLLDESDM.KinkedTrack_ZeeFilterFlags.preScale)
ToolSvc += KinkTrkZeePrescaleTool

KinkTrkZeeLowPtFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZeeLowPtFilterTool",
                                                                      FilterList = [KinkTrkZeeLowPtSkimmingTool, KinkTrkZeePrescaleTool])
ToolSvc += KinkTrkZeeLowPtFilterTool

KinkTrkZeeFilterTool = DerivationFramework__FilterCombinationOR(name = "KinkTrkZeeFilterTool",
                                                                FilterList = [KinkTrkZeeLowPtFilterTool, KinkTrkZeeHighPtSkimmingTool])
ToolSvc += KinkTrkZeeFilterTool

KinkTrkZeeFinalFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZeeFinalFilterTool",
                                                                      FilterList=[KinkTrkZeeTriggerFilterTool, KinkTrkZeeFilterTool],
                                                                      OutputLevel=INFO)
ToolSvc+= KinkTrkZeeFinalFilterTool

topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackZeeFilterKernel",
                                                     SkimmingTools = [KinkTrkZeeFinalFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackZeeFilterKernel"])


##
## Selecting low-pT probe muons
KinkTrkZmumuLowPtSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuLowPtSkimmingTool",
                                                                            expression = 'count(abs(KinkTrkDiMuMass)>0)>=1 && count(KinkTrkProbeMuPt>=%f)==0' % primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.preScaleTrackPtMax)
ToolSvc += KinkTrkZmumuLowPtSkimmingTool

## Selecting high-pT probe muons
KinkTrkZmumuHighPtSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "KinkTrkZmumuHighPtSkimmingTool",
                                                                             expression = 'count(abs(KinkTrkDiMuMass)>0)>=1 && count(KinkTrkProbeMuPt>=%f)>=1'% primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.preScaleTrackPtMax)
ToolSvc += KinkTrkZmumuHighPtSkimmingTool

## Prescale for low-pT probes
KinkTrkZmumuPrescaleTool = DerivationFramework__PrescaleTool(name = "KinkTrkZmumuPrescaleTool",
                                                             Prescale = primRPVLLDESDM.KinkedTrack_ZmumuFilterFlags.preScale)
ToolSvc += KinkTrkZmumuPrescaleTool

KinkTrkZmumuLowPtFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZmumuLowPtFilterTool",
                                                                        FilterList = [KinkTrkZmumuLowPtSkimmingTool, KinkTrkZmumuPrescaleTool])
ToolSvc += KinkTrkZmumuLowPtFilterTool

KinkTrkZmumuFilterTool = DerivationFramework__FilterCombinationOR(name = "KinkTrkZmumuFilterTool",
                                                                  FilterList = [KinkTrkZmumuLowPtFilterTool, KinkTrkZmumuHighPtSkimmingTool])
ToolSvc += KinkTrkZmumuFilterTool


KinkTrkZmumuFinalFilterTool = DerivationFramework__FilterCombinationAND(name = "KinkTrkZmumuFinalFilterTool",
                                                                        FilterList=[KinkTrkZmumuTriggerFilterTool, KinkTrkZmumuFilterTool],
                                                                        OutputLevel=INFO)
ToolSvc+= KinkTrkZmumuFinalFilterTool

## Prescale for low-pT electron probes
topSequence += DerivationFramework__DerivationKernel("RPVLL_KinkedTrackZmumuFilterKernel",
                                                     SkimmingTools = [KinkTrkZmumuFinalFilterTool])
RPVLLfilterNames.extend(["RPVLL_KinkedTrackZmumuFilterKernel"])

