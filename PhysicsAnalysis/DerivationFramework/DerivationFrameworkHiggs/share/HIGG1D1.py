#==================================================================== 
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#==================================================================== 

#====================================================================
# DAOD_HIGG1D1.py
# This defines DAOD_HIGG1D1, an Diphoton skimmed DAOD format for Run 3.
# It requires the reductionConf flag HIGG1D1 in Reco_tf.py   
#====================================================================

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr
from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkMuons import MuonsCommon
# Common augmentations ("DFCommon")
InDetCommon.makeInDetDFCommon()
EGammaCommon.makeEGammaDFCommon()
MuonsCommon.makeMuonsDFCommon()
from DerivationFrameworkJetEtMiss.JetCommon import addEventCleanFlags, addBadBatmanFlag, addDistanceInTrain, addDAODJets, addSidebandEventShape
from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo,AntiKt4EMPFlow,AntiKtVR30Rmax4Rmin02PV0Track
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopoTrimmed
from DerivationFrameworkJetEtMiss.METCommon import scheduleStandardMETContent
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
import re
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc


from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D1Stream )
HIGG1D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D1Stream.AcceptAlgs(["HIGG1D1Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
augmentationTools   = []

# Special sequence 
SeqHIGG1D1 = CfgMgr.AthSequencer("SeqHIGG1D1")


#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if DerivationFrameworkIsMonteCarlo:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   HIGG1D1TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "HIGG1D1TruthCharmTool",
                                                                        NewCollectionName       = "TruthCharm",
                                                                        KeepNavigationInfo      = False,
                                                                        ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                        Do_Compress             = True)
   ToolSvc += HIGG1D1TruthCharmTool 
   SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("HIGG1D1TruthCharmKernel",AugmentationTools=[HIGG1D1TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqHIGG1D1)
   # Add standard truth
   addStandardTruthContents(SeqHIGG1D1,prefix="DFCommon_")
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   HIGG1D1TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="HIGG1D1TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += HIGG1D1TruthNavigationDecorator
   SeqHIGG1D1.DFCommon_MCTruthNavigationDecoratorKernel.AugmentationTools = [HIGG1D1TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqHIGG1D1)
   addPVCollection(SeqHIGG1D1)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"



#====================================================================
# PV refitting after removing Z->ee tracks, for vertex studies
#====================================================================

# Creates a vertex container (ZeeRefittedPrimaryVertices) where the type=1 vertex is refitted
# after removing tracks that are associated with Z->ee decay candidates
# Tool runs only for data and Zee MC samples (must be defined in the MCSamples list)
from InDetRecExample import TrackingCommon
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__ZeeVertexRefittingTool
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__PrimaryVertexRefitter
import AthenaCommon.SystemOfUnits as Units
PrimaryVertexRefitter = Analysis__PrimaryVertexRefitter( TrackToVertexIPEstimator = TrackingCommon.getTrackToVertexIPEstimator() )
ToolSvc += PrimaryVertexRefitter

HIGG1D1_ZeeVertexRefitterTool = DerivationFramework__ZeeVertexRefittingTool( name = "HIGG1D1_ZeeVertexRefitterTool",
                                    ObjectRequirements="(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.*GeV)",
                                    LowMassCut=50*Units.GeV,
                                    RefittedPVContainerName="ZeeRefittedPrimaryVertices",                                    
                                    ElectronContainerName="Electrons",
                                    PVContainerName="PrimaryVertices",
                                    MCSamples = [361106],
                                    PrimaryVertexRefitterTool = PrimaryVertexRefitter )
ToolSvc += HIGG1D1_ZeeVertexRefitterTool
SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("ZeeVertexRefitKernel", AugmentationTools = [HIGG1D1_ZeeVertexRefitterTool])

#====================================================================
# Diphoton vertex decoration tool
#====================================================================

# Creates a shallow copy of PrimaryVertices (HggPrimaryVertices) for diphoton events
# Must be created before the jetalg in the sequence as it is input to the modified PFlow jets

import PhotonVertexSelection.PhotonVertexSelectionConf as PVS 
PhotonPointingTool = PVS.CP__PhotonPointingTool( name = "yyVtxPhotonPointingTool", isSimulation = rec.doTruth() )
ToolSvc += PhotonPointingTool
PhotonVertexSelectionTool = PVS.CP__PhotonVertexSelectionTool( name = "PhotonVertexSelectionTool", PhotonPointingTool = PhotonPointingTool)
ToolSvc += PhotonVertexSelectionTool

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__DiphotonVertexDecorator
DiphotonVertexDecorator = DerivationFramework__DiphotonVertexDecorator(name = "DiphotonVertexDecorator",  
                                                                       PhotonVertexSelectionTool = PhotonVertexSelectionTool)
ToolSvc += DiphotonVertexDecorator
SeqHIGG1D1 += CfgMgr.DerivationFramework__CommonAugmentation("DiphotonVertexKernel", AugmentationTools = [DiphotonVertexDecorator])


isMC = False
if globalflags.DataSource()=="geant4":
    isMC = True
    from DerivationFrameworkMCTruth.MCTruthCommon import addPVCollection, addStandardTruthContents
    addStandardTruthContents(SeqHIGG1D1,prefix="HIGG1D1_")
    addPVCollection()
    print( "HIGG1D1.py Applying MCTruthCommon")


#====================================================================
# SKIMMING TOOLS 
#====================================================================

print( "HIGG1D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy() )
SkipTriggerRequirement=((globalflags.DataSource()=="geant4") and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print( "HIGG1D1.py SkipTriggerRequirement", SkipTriggerRequirement)
TriggerExp = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerExp               = ["EF_g35_loose_g25_loose"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerExp               = ["HLT_2g50_loose_L12EM20VH","HLT_2g25_loose_g15_loose","HLT_g35_medium_g25_medium_L12EM20VH","HLT_2g25_tight_L12EM20VH","HLT_2g22_tight_L12EM15VHI","HLT_g35_loose_g25_loose","HLT_g35_medium_g25_medium","HLT_2g50_loose","HLT_2g20_tight","HLT_2g22_tight","HLT_2g20_tight_icalovloose_L12EM15VHI","HLT_2g20_tight_icalotight_L12EM15VHI","HLT_2g22_tight_L12EM15VHI","HLT_2g22_tight_icalovloose_L12EM15VHI","HLT_2g22_tight_icalotight_L12EM15VHI","HLT_2g22_tight_icalovloose","HLT_2g25_tight_L12EM20VH","HLT_2g20_loose","HLT_2g20_loose_L12EM15","HLT_g35_medium_g25_medium","HLT_g35_medium_g25_medium_L12EM15VH","HLT_g35_loose_g25_loose","HLT_g35_loose_g25_loose_L12EM15VH", "HLT_2g20_loose_g15_loose", "HLT_3g20_loose", "HLT_3g15_loose", "HLT_2g6_tight_icalotight_L1J100", "HLT_2g6_loose_L1J100", "HLT_2g6_tight_icalotight_L1J50", "HLT_2g6_loose_L1J50","HLT_g120_loose","HLT_g140_loose"]


print( "HIGG1D1.py TriggerExp", TriggerExp )

KeepTrigger=not SkipTriggerRequirement
#KeepTrigger=False

print( "HIGG1D1.py KeepTrigger", KeepTrigger )

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
SkimmingToolHIGG1D1 = DerivationFramework__SkimmingToolHIGG1(
                                 name = "SkimmingToolHIGG1D1",
                                 RequireGRL = True,
                                 ReqireLArError = True,
                                 RequireTrigger = KeepTrigger,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False)

ToolSvc += SkimmingToolHIGG1D1


#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
# TODO correct this so it keeps tracks from the selected PV
HIGG1D1_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D1TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG1D1TrackParticleThinningTool",
                                                                           StreamName              = HIGG1D1Stream.Name, 
                                                                           SelectionString         = HIGG1D1_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D1TrackParticleThinningTool
thinningTools.append(HIGG1D1TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG1D1MuonTPThinningTool",
                                                                        StreamName              = HIGG1D1Stream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG1D1MuonTPThinningTool
thinningTools.append(HIGG1D1MuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
HIGG1D1TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "HIGG1D1TauJetsThinningTool",
                                                                     StreamName      = HIGG1D1Stream.Name,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += HIGG1D1TauJetsThinningTool
thinningTools.append(HIGG1D1TauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG1D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG1D1TauTPThinningTool",
                                                                      StreamName             = HIGG1D1Stream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += HIGG1D1TauTPThinningTool
thinningTools.append(HIGG1D1TauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
HIGG1D1DiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "HIGG1D1DiTauTPThinningTool",
                                                                          StreamName              = HIGG1D1Stream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D1DiTauTPThinningTool
thinningTools.append(HIGG1D1DiTauTPThinningTool)





from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=="geant4":
    # Truth thinning
    truth_cond_1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_3 = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
    truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_cond_5 = "(abs(TruthParticles.pdgId) >=  1000000)" # BSM
    truth_cond_finalState = "(TruthParticles.status == 1 && TruthParticles.barcode < 200000 && (TruthParticles.pt > 1*GeV))" # stable particles
    truth_expression = "("+truth_cond_1+" || "+truth_cond_2 +" || "+truth_cond_3 +" || "+truth_cond_4+ " || "+truth_cond_5+") || ("+truth_cond_finalState+")"

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG1D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D1TruthThinningTool", 
                                                                        StreamName              = HIGG1D1Stream.Name,
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = False,
                                                                        PreserveGeneratorDescendants  = True,
                                                                        PreserveAncestors             = True)
    ToolSvc += HIGG1D1TruthThinningTool
    thinningTools.append(HIGG1D1TruthThinningTool)

print( "HIGG1D1.py thinningTools", thinningTools )

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from LArCabling.LArCablingAccess import LArOnOffIdMapping
#LArOnOffIdMapping()



from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
HIGG1D1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "HIGG1D1_MaxCellDecoratorTool",
                                                                      SGKey_electrons = "Electrons",
                                                                      SGKey_photons   = "Photons"
                                                                      )
ToolSvc += HIGG1D1_MaxCellDecoratorTool
augmentationTools.append(HIGG1D1_MaxCellDecoratorTool)
#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
HIGG1D1_GainDecoratorTool = GainDecorator()
ToolSvc += HIGG1D1_GainDecoratorTool
augmentationTools.append(HIGG1D1_GainDecoratorTool)


cluster_sizes = (3,5), (5,7), (7,7), (7,11)
HIGG1D1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
augmentationTools += HIGG1D1_ClusterEnergyPerLayerDecorators



#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
#from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqHIGG1D1 += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D1Kernel",
                                                            SkimmingTools = [SkimmingToolHIGG1D1],
                                                            AugmentationTools = augmentationTools,
                                                            ThinningTools = thinningTools)


#====================================================================
# JET/MET   
#====================================================================

from JetRecConfig.StandardJetMods import stdJetModifiers
for key in stdJetModifiers:
  print( "Key is : {} ".format(key) )

AntiKt4EMTopo_deriv = AntiKt4EMTopo.clone(
   modifiers = AntiKt4EMTopo.modifiers #+("JetPtAssociation","QGTagging")
)

AntiKt4EMPFlow_deriv = AntiKt4EMPFlow.clone(
   modifiers = AntiKt4EMPFlow.modifiers #+("JetPtAssociation","QGTagging","fJVT")
)

jetList = [AntiKt4EMTopo_deriv,
           AntiKt4EMPFlow_deriv,
           AntiKtVR30Rmax4Rmin02PV0Track,
           AntiKt10LCTopoTrimmed]

addDAODJets(jetList,SeqHIGG1D1)
# Special rho definition for PFlow jets
addSidebandEventShape(sequence=SeqHIGG1D1)
# Event cleaning flags
addEventCleanFlags(sequence=SeqHIGG1D1)
# Bad batman flag for events with large EMEC-IW Noise
addBadBatmanFlag(sequence=SeqHIGG1D1)
# Distance in train
addDistanceInTrain(sequence=SeqHIGG1D1)
#MET reconstruction
scheduleStandardMETContent(sequence=SeqHIGG1D1, algname="METAssociationAlg")

#====================================================================
# Custom JETS
#====================================================================

# Ideally there would be a nice way to configure the PFlowCustomVtx jets  but at the moment 
# all tools need to be configured manually to ensure that the track to vertex association is done correctly.
PrefixForHggCollection   = "Hgg"
HggVertexContainerName   = PrefixForHggCollection+"PrimaryVertices"
CustomPFJetContainerName = "AntiKt4EMPFlowCustomVtxJets"

from JetRecConfig.StandardJetConstits import stdInputExtDic, JetInputExternal,JetInputConstit, JetInputConstitSeq, JetConstitModifier, xAODType
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow
from JetRecConfig.JetDefinition import JetDefinition 
from JetRecTools import JetRecToolsConfig as jrtcfg
from JetMomentTools import JetMomentToolsConfig
from JetRecConfig.StandardJetConstits import stdConstitDic, stdContitModifDic
from JetRecConfig.StandardJetContext import propFromContext, inputsFromContext
from JetRecConfig.JetInputConfig import buildEventShapeAlg
from AthenaConfiguration.ComponentFactory import CompFactory

# Create new jet context
from JetRecConfig.StandardJetContext import jetContextDic
jetContextName = 'CustomVtx'
if jetContextName not in jetContextDic:
  jetContextDic[jetContextName] = jetContextDic['default'].clone(
    Vertices         = HggVertexContainerName,
    GhostTracks      = "PseudoJetGhostTrack"+jetContextName, 
    GhostTracksLabel = "GhostTrack"+jetContextName,
    TVA              = "JetTrackVtxAssoc"+jetContextName,
    JetTracks        = "JetSelectedTracks"+jetContextName,
    JetTracksQualityCuts = "JetSelectedTracks"+jetContextName+"_trackSelOpt"
  )
context =  jetContextDic[jetContextName] 

def replaceItemsAndAdd(tup,orgName,newName,suffix=""):
    newList = list(tup)
    for i, item in enumerate(newList):
        if orgName in item:
            newList[i] = item.replace(orgName,newName)+suffix
            print( "Updated ", orgName, " to ", newName )
            return tuple(newList)
    print( "Failed to update ", orgName, " to ", newName )
    return tuple(newList)
    

# Create modifier list and JetDefinition 
modsCustomVtx = AntiKt4EMPFlow.modifiers
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"Calib","CalibCustomVtx")
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"TrackMoments","TrackMomentsCustomVtx")
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"TrackSumMoments","TrackSumMomentsCustomVtx")
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"JVF","JVFCustomVtx")
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"JVT","JVTCustomVtx")
modsCustomVtx = replaceItemsAndAdd(modsCustomVtx,"Charge","ChargeCustomVtx")

ghostCustomVtx = AntiKt4EMPFlow.ghostdefs
ghostCustomVtx = replaceItemsAndAdd(ghostCustomVtx,"Track","TrackCustomVtx")

# GPFlow are the same than EMPFlow except they have pflow linked to elec or muons filtered out.
stdConstitDic["TrackCustomVtx"]  = JetInputConstit("TrackCustomVtx", xAODType.TrackParticle,"JetSelectedTracksCustomVtx" )


stdConstitDic["GPFlowCustomVtx"] = JetInputConstitSeq("GPFlowCustomVtx", xAODType.FlowElement,["CorrectPFOCustomVtx", "CHSCustomVtx"] , 'GlobalParticleFlowObjects', 'CHSGCustomVtxParticleFlowObjects',
                    label='EMPFlow')

stdContitModifDic["CorrectPFOCustomVtx"] = JetConstitModifier("CorrectPFOCustomVtx", "CorrectPFOTool",
                                                              prereqs=[inputsFromContext("Vertices")],
                                                              properties=dict(VertexContainerKey=propFromContext("Vertices"),
                                                                              WeightPFOTool= CompFactory.getComp("CP::WeightPFOTool")("weightPFO") )
                                                              )

stdContitModifDic["CHSCustomVtx"] = JetConstitModifier("CHSCustomVtx",    "ChargedHadronSubtractionTool",
                                                      prereqs=  [inputsFromContext("Vertices"),inputsFromContext("TVA")],
                                                      properties=dict(VertexContainerKey=propFromContext("Vertices"),                                       
                                                                      TrackVertexAssociation=propFromContext("TVA"),
                                                                      UseTrackToVertexTool=True, 
                                                                      ))



AntiKt4EMPFlowCustomVtx = JetDefinition("AntiKt",0.4,stdConstitDic.GPFlowCustomVtx,
                                    infix = "CustomVtx",
                                    context = jetContextName,
                                    ghostdefs = ghostCustomVtx,
                                    modifiers = modsCustomVtx+("JetPtAssociation","QGTaggingCustomVtx","fJVTCustomVtx","NNJVTCustomVtx"),
                                    ptmin = 10000,
)

def getUsedInVertexFitTrackDecoratorAlgCustomVtx(jetdef, jetmod):
    """ Create the alg  to decorate the used-in-fit information for AMVF """
    from JetRecConfig.StandardJetContext import jetContextDic
    context  = jetContextDic[jetdef.context] 
    
    alg = CompFactory.getComp("InDet::InDetUsedInVertexFitTrackDecorator")(
        "UsedInVertexFitTrackDecoratorAlgCustomVtxAlg" ,
        UsedInFitDecoratorTool=CompFactory.getComp("InDet::InDetUsedInFitTrackDecoratorTool")(
            "UsedInFitTrackDecoratorToolCustomVtx",
            AMVFVerticesDecoName= 'TTVA_AMVFVertices_forReco',
            AMVFWeightsDecoName= 'TTVA_AMVFWeights_forReco',
            TrackContainer=context['Tracks'],
            VertexContainer=context['Vertices'],
        ),
    )
    return alg


# Define new input variables for jet configuration
stdInputExtDic[context['Vertices']] = JetInputExternal( context['Vertices'],   xAODType.Vertex )

stdInputExtDic["JetSelectedTracksCustomVtx"] = JetInputExternal("JetSelectedTracksCustomVtx",     xAODType.TrackParticle,
                                                                                prereqs= [ f"input:{context['Tracks']}" ], # in std context, this is InDetTrackParticles (see StandardJetContext)
                                                                                algoBuilder = lambda jdef,_ : jrtcfg.getTrackSelAlg(jdef.context, trackSelOpt=False )
                                                                              )

stdInputExtDic["JetTrackUsedInFitDecoCustomVtx"] = JetInputExternal("JetTrackUsedInFitDecoCustomVtx", xAODType.TrackParticle,
                                                                    prereqs= [ f"input:{context['Tracks']}" , # in std context, this is InDetTrackParticles (see StandardJetContext)
                                                                              f"input:{context['Vertices']}"],
                                                                    algoBuilder = getUsedInVertexFitTrackDecoratorAlgCustomVtx
                                                                    )

stdInputExtDic["JetTrackVtxAssocCustomVtx"] = JetInputExternal("JetTrackVtxAssocCustomVtx",  xAODType.TrackParticle,
                                          algoBuilder = lambda jdef,_ : jrtcfg.getJetTrackVtxAlg(jdef.context, algname="jetTVACustomVtx", WorkingPoint="Nonprompt_All_MaxWeight"),
                                          prereqs = [ "input:JetTrackUsedInFitDecoCustomVtx", f"input:{context['Vertices']}" ] )

stdInputExtDic["EventDensityCustomVtx"] =     JetInputExternal("EventDensityCustomVtx", "EventShape", algoBuilder = buildEventShapeAlg,
                  containername = lambda jetdef, _ : +"Kt4"+jetdef.inputdef.label+"CustomVtxEventShape",
                  prereqs = lambda jetdef : ["input:"+jetdef.inputdef.name] )

from JetRecConfig.StandardJetMods import stdJetModifiers
from JetRecConfig.JetDefinition import JetModifier
from JetCalibTools import JetCalibToolsConfig


# This method instantiates the JetCalibTool given the input mod specification
def getJetCalibToolFromStringCustomVtx(jetdef, modspec):
    calibcontext, data_type, calibseq, rhoname, pvname, gscdepth = JetCalibToolsConfig.getCalibSpecsFromString(modspec)
    rhoname = "Kt4EMPFlowCustomVtxEventShape"
    return JetCalibToolsConfig.getJetCalibTool(jetdef.basename,calibcontext,data_type,calibseq,rhoname,pvname,gscdepth)


stdJetModifiers.update(
  CalibCustomVtx = JetModifier("JetCalibrationTool","jetcalib_jetcoll_calibseqCustomVtx",
                                    createfn=getJetCalibToolFromStringCustomVtx,
                                    prereqs=lambda mod,jetdef : JetCalibToolsConfig.getJetCalibToolPrereqs(mod,jetdef)+[f"input:{context['Vertices']}"]),


  JVFCustomVtx =             JetModifier("JetVertexFractionTool", "jvfCustomVtx",
                                    createfn= lambda jdef,_ : JetMomentToolsConfig.getJVFTool(jdef.context,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = ["mod:TrackMomentsCustomVtx", f"input:{context['Vertices']}"] ,JetContainer = CustomPFJetContainerName),

  JVTCustomVtx =             JetModifier("JetVertexTaggerTool", "jvtCustomVtx",
                                    createfn= lambda jdef,_ : JetMomentToolsConfig.getJVTTool(jdef.context,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName),

  NNJVTCustomVtx =           JetModifier("JetVertexNNTagger", "nnjvtCustomVtx",
                                          createfn=lambda jdef,_ :JetMomentToolsConfig.getNNJvtTool(jdef.context,"CustomVtx"),
                                          prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName),

  OriginSetPVCustomVtx =     JetModifier("JetOriginCorrectionTool", "origin_setpvCustomVtx",
                                    modspec = "CustomVtx",
                                    prereqs = [ "mod:JVFCustomVtx" ],JetContainer = CustomPFJetContainerName, OnlyAssignPV=True),

  TrackMomentsCustomVtx =    JetModifier("JetTrackMomentsTool", "trkmomsCustomVtx",
                                    createfn= lambda jdef,_ : JetMomentToolsConfig.getTrackMomentsTool(jdef,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = [ "input:JetTrackVtxAssocCustomVtx","ghost:TrackCustomVtx" ],JetContainer = CustomPFJetContainerName),

  TrackSumMomentsCustomVtx = JetModifier("JetTrackSumMomentsTool", "trksummomsCustomVtx",
                                    createfn=lambda jdef,_ :JetMomentToolsConfig.getTrackSumMomentsTool(jdef,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = [ "input:JetTrackVtxAssocCustomVtx","ghost:TrackCustomVtx" ],JetContainer = CustomPFJetContainerName),

  ChargeCustomVtx =          JetModifier("JetChargeTool", "jetchargeCustomVtx", 
                                    prereqs = [ "ghost:TrackCustomVtx" ]),


  QGTaggingCustomVtx =       JetModifier("JetQGTaggerVariableTool", "qgtaggingCustomVtx",
                                    createfn=lambda jdef,_ :JetMomentToolsConfig.getQGTaggingTool(jdef,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = lambda _,jdef : ["mod:JetPtAssociation", "mod:TrackMomentsCustomVtx"] if not ConfigFlags.Input.isMC else ["mod:TrackMomentsCustomVtx"],
                                    JetContainer = CustomPFJetContainerName),

  fJVTCustomVtx =            JetModifier("JetForwardPFlowJvtTool", "fJVTCustomVtx",
                                    createfn=lambda jdef,_ :JetMomentToolsConfig.getPFlowfJVTTool(jdef,"CustomVtx"),
                                    modspec = "CustomVtx",
                                    prereqs = ["input:EventDensity",f"input:{context['Vertices']}"],
                                    JetContainer = CustomPFJetContainerName),
)
addDAODJets([AntiKt4EMPFlowCustomVtx], SeqHIGG1D1)


#MET associated to HggPrimaryVertices
from DerivationFrameworkJetEtMiss import METCommon
METCommon.scheduleCustomVtxMETContent(vxColl="Hgg", jetColl="AntiKt4EMPFlowCustomVtx",  sequence=SeqHIGG1D1)


#====================================================================
# FLAVOUR TAGGING   ol90
#====================================================;;p================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
#FtagJetCollections(["AntiKt4EMPFlowCustomVtxJets","AntiKt4EMPFlowJets"], SeqHIGG1D1, pvCols=[HggVertexContainerName,"PrimaryVertices"])


#====================================================================
# ED
#====================================================================
if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ["EDTruthCentralAlg", "EDTruthForwardAlg"]:
      if hasattr(topSequence, alg):
         edtalg = getattr(topSequence, alg)
         delattr(topSequence, alg)
         SeqHIGG1D1 += edtalg

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D1SlimmingHelper = SlimmingHelper("HIGG1D1SlimmingHelper")

HIGG1D1SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

# Trigger content
HIGG1D1SlimmingHelper.IncludeTriggerNavigation = False
HIGG1D1SlimmingHelper.IncludeJetTriggerContent = False
HIGG1D1SlimmingHelper.IncludeMuonTriggerContent = False
HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = False
HIGG1D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
HIGG1D1SlimmingHelper.IncludeTauTriggerContent = False
HIGG1D1SlimmingHelper.IncludeEtMissTriggerContent = False
HIGG1D1SlimmingHelper.IncludeBJetTriggerContent = False
HIGG1D1SlimmingHelper.IncludeBPhysTriggerContent = False
HIGG1D1SlimmingHelper.IncludeMinBiasTriggerContent = False
HIGG1D1SlimmingHelper.AllVariables = ["HLT_xAOD__PhotonContainer_egamma_Iso_Photons","Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","TruthElectrons","TruthPhotons","TruthMuons","TruthTaus","TruthBoson","PrimaryVertices","MET_Truth", "MET_Track","egammaTruthParticles","CaloCalTopoClusters","HggPrimaryVertices","ZeeRefittedPrimaryVertices"]


# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(HIGG1D1SlimmingHelper,["HIGG1D1"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   HIGG1D1SlimmingHelper.AppendToDictionary = {"TruthEvents":"xAOD::TruthEventContainer","TruthEventsAux":"xAOD::TruthEventAuxContainer",
                                            "MET_Truth":"xAOD::MissingETContainer","MET_TruthAux":"xAOD::MissingETAuxContainer",
                                            "TruthElectrons":"xAOD::TruthParticleContainer","TruthElectronsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthMuons":"xAOD::TruthParticleContainer","TruthMuonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthPhotons":"xAOD::TruthParticleContainer","TruthPhotonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthTaus":"xAOD::TruthParticleContainer","TruthTausAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthNeutrinos":"xAOD::TruthParticleContainer","TruthNeutrinosAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBSM":"xAOD::TruthParticleContainer","TruthBSMAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBoson":"xAOD::TruthParticleContainer","TruthBosonAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthTop":"xAOD::TruthParticleContainer","TruthTopAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthForwardProtons":"xAOD::TruthParticleContainer","TruthForwardProtonsAux":"xAOD::TruthParticleAuxContainer",
                                            "BornLeptons":"xAOD::TruthParticleContainer","BornLeptonsAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBosonsWithDecayParticles":"xAOD::TruthParticleContainer","TruthBosonsWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBosonsWithDecayVertices":"xAOD::TruthVertexContainer","TruthBosonsWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthBSMWithDecayParticles":"xAOD::TruthParticleContainer","TruthBSMWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthBSMWithDecayVertices":"xAOD::TruthVertexContainer","TruthBSMWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "HardScatterParticles":"xAOD::TruthParticleContainer","HardScatterParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "HardScatterVertices":"xAOD::TruthVertexContainer","HardScatterVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthHFWithDecayParticles":"xAOD::TruthParticleContainer","TruthHFWithDecayParticlesAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthHFWithDecayVertices":"xAOD::TruthVertexContainer","TruthHFWithDecayVerticesAux":"xAOD::TruthVertexAuxContainer",
                                            "TruthCharm":"xAOD::TruthParticleContainer","TruthCharmAux":"xAOD::TruthParticleAuxContainer",
                                            "TruthPrimaryVertices":"xAOD::TruthVertexContainer","TruthPrimaryVerticesAux":"xAOD::TruthVertexAuxContainer",
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(HIGG1D1SlimmingHelper)
   HIGG1D1SlimmingHelper.AllVariables += ["TruthHFWithDecayParticles","TruthHFWithDecayVertices","TruthCharm"]


HIGG1D1SlimmingHelper.AllVariables   += ["EventInfo"]
HIGG1D1SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "AntiKt4EMPFlowJetsCustomVtx.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z",
                                      "InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights"]

#Custom Jet Varibles

HIGG1D1SlimmingHelper.AppendToDictionary.update({  "AntiKt4EMPFlowJetsCustomVtx": "xAOD::JetContainer", "AntiKt4EMPFlowCustomVtxJetsAux":"xAOD::JetAuxContainer",
        "METAssoc_AntiKt4EMPFlowCustomVtxHgg":"xAOD::MissingETAssociationMap", "METAssoc_AntiKt4EMPFlowCustomVtxHggAux":"xAOD::MissingETAuxAssociationMap",
        "MET_Core_AntiKt4EMPFlowCustomVtxHgg":"xAOD::MissingETContainer", "MET_Core_AntiKt4EMPFlowCustomVtxHggAux":"xAOD::MissingETAuxContainer",
        HggVertexContainerName:"xAOD::VertexContainer", HggVertexContainerName+"Aux":"xAOD::ShallowAuxContainer",
        "Kt4PFlowCustomVtxEventShape":"xAOD::EventShape", "Kt4PFlowCustomVtxEventShapeAux":"xAOD::EventShapeAuxInfo",
        "Kt4EMPFlowEventShape":"xAOD::EventShape", "Kt4EMPFlowEventShapeAux":"xAOD::EventShapeAuxInfo",
        "BTagging_AntiKt4EMPFlowCustomVtx":"xAOD::BTaggingContainer", "BTagging_AntiKt4EMPFlowCustomVtxAux" : "xAOD::BTaggingAuxContainer",
        "ZeeRefittedPrimaryVertices":"xAOD::VertexContainer","ZeeRefittedPrimaryVerticesAux":"xAOD:VertexAuxContainer",
        "MET_Track":"xAOD::MissingETContainer","MET_TrackAux":"xAOD::MissingETAuxContainer",
        "egammaTopoSeededClusters":"xAOD::CaloClusterContainer","egammaTopoSeededClustersAux":"xAOD::CaloClusterAuxContainer",
        "NeutralParticleFlowIsoCentralEventShape":"xAOD::EventShape", "NeutralParticleFlowIsoForwardEventShapeAux":"xAOD::EventShapeAuxInfo",
        "NeutralParticleFlowIsoForwardEventShape":"xAOD::EventShape", "NeutralParticleFlowIsoForwardEventShapeAux":"xAOD::EventShapeAuxInfo",
        "Kt4EMTopoOriginEventShape":"xAOD::EventShape", "Kt4EMTopoOriginEventShapeAux":"xAOD::EventShapeAuxInfo",
     })

HIGG1D1SlimmingHelper.AllVariables += ["HggPrimaryVertices","ZeeRefittedPrimaryVertices","AntiKt4EMPFlowCustomVtxJets","Kt4PFlowCustomVtxEventShape","Kt4EMPFlowEventShape"]


from DerivationFrameworkFlavourTag.BTaggingContent import BTaggingStandardContent, BTaggingXbbContent
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowJetsCustomVtx")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMPFlowJets")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4EMPFlowJetsCustomVtx")
HIGG1D1SlimmingHelper.ExtraVariables += BTaggingXbbContent("AntiKt4EMPFlowJets")

# Add AFP information
HIGG1D1Stream.AddItem("xAOD::AFPSiHitContainer#AFPSiHitContainer")
HIGG1D1Stream.AddItem("xAOD::AFPSiHitAuxContainer#AFPSiHitContainerAux.")
HIGG1D1Stream.AddItem("xAOD::AFPToFHitContainer#AFPToFHitContainer")
HIGG1D1Stream.AddItem("xAOD::AFPToFHitAuxContainer#AFPToFHitContainerAux.")

# is this really needed given Photons are in the AllVariables list ?
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import PhotonsCPDetailedContent
HIGG1D1SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

HIGG1D1SlimmingHelper.ExtraVariables.extend( getGainDecorations(HIGG1D1_GainDecoratorTool) )
for tool in HIGG1D1_ClusterEnergyPerLayerDecorators:
    HIGG1D1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

from DerivationFrameworkJetEtMiss.METCommon import addMETOutputs
addMETOutputs(HIGG1D1SlimmingHelper,["AntiKt4EMPFlow","AntiKt4EMPFlowCustomVtxHgg"])
HIGG1D1Stream.AddItem("xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMPFlowCustomVtxHgg")
HIGG1D1Stream.AddItem("xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMPFlowCustomVtxHggAux.")
HIGG1D1Stream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4EMPFlowCustomVtxHgg")
HIGG1D1Stream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMPFlowCustomVtxHggAux.")

HIGG1D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D1Stream.AddItem("xAOD::CutBookkeeperContainer#CutBookkeepers")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperAuxContainer#CutBookkeepersAux.")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers")
HIGG1D1Stream.AddItem("xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepersAux.")
HIGG1D1Stream.AddItem("ByteStreamMetadataContainer#ByteStreamMetadata")


#====================================================================
# TRIGGER CONTENT   
#====================================================================
from DerivationFrameworkPhys import PhysCommonTrigger
PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(HIGG1D1SlimmingHelper)
PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(HIGG1D1SlimmingHelper)

# Final construction of output stream
HIGG1D1SlimmingHelper.AppendContentToStream(HIGG1D1Stream)
#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job
DerivationFrameworkJob += SeqHIGG1D1

from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)


