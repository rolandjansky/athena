#********************************************************************
# STDM2.py 
# reductionConf flag STDM2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from AthenaCommon.GlobalFlags import globalflags

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
photonRequirements = '(DFCommonPhotons_et >= 15*GeV) && (abs(DFCommonPhotons_eta) < 2.5)'# && (Photons.Loose)'
electronRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.5) && (Electrons.DFCommonElectronsLHLoose)'

photonSelection = '(count(' + photonRequirements + ') >= 1)'
electronSelection = '(count(' + electronRequirements + ') >= 1)'

# 2015 data triggers. unprescaled list validated with https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled#Egamma_MET.
# full list avaiable here: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/TrigMenuRulebook/trunk/python/Physics_pp_v5_rules.py (good luck)
singleElectronTriggerRequirements = '( HLT_e24_lhmedium_L1EM20VH || HLT_e24_lhmedium_L1EM18VH || HLT_e24_lhmedium_iloose_L1EM18VH || HLT_e24_medium_iloose_L1EM18VH  || HLT_e24_medium_iloose_L1EM20VH || HLT_e24_tight_iloose_L1EM20VH || HLT_e24_tight_iloose || HLT_e26_tight_iloose || HLT_e60_medium || HLT_e120_loose || HLT_e140_loose  || HLT_e24_lhmedium_iloose_EM20VH || HLT_e24_lhtight_iloose_EM20VH || HLT_e24_lhtight_iloose || HLT_e26_lhtight_iloose || HLT_e60_lhmedium || HLT_e120_lhloose || HLT_e140_lhloose || HLT_e26_lhtight_nod0_iloose || HLT_e60_lhmedium_nod0 || HLT_e24_lhtight_nod0_iloose || HLT_e24_lhmedium_nod0_iloose_L1EM20VH || HLT_e24_lhmedium_nod0_iloose_L1EM18VH || HLT_e120_lhloose_nod0 )'

diElectronTriggerRequirements = '( HLT_2e12_loose_L12EM10VH || HLT_2e15_loose_L12EM13VH || HLT_2e17_loose || HLT_2e17_loose_L12EM15 || HLT_2e12_lhloose_L12EM10VH || HLT_2e15_lhloose_L12EM13VH || HLT_2e17_lhloose || HLT_2e17_lhloose_L12EM15 )'

triElectronTriggerRequirements = '( HLT_e17_loose_2e9_loose || HLT_e17_medium_2e9_medium || HLT_e17_medium_iloose_2e9_medium || HLT_e17_lhloose_2e9_lhloose || HLT_e17_lhmedium_2e9_lhmedium || HLT_e17_lhmedium_iloose_2e9_lhmedium || HLT_e17_lhloose_nod0_2e9_lhloose_nod0 || HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0 || HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0 )'


singlePhotonTriggerRequirements = '( HLT_g10_loose || HLT_g15_loose_L1EM7 || HLT_g20_loose_L1EM12 || HLT_g25_loose_L1EM15  || HLT_g35_loose_L1EM15 || HLT_g25_loose || HLT_g25_medium || HLT_g35_loose || HLT_g35_medium || HLT_g40_loose_L1EM15 || HLT_g45_loose_L1EM15 || HLT_g50_loose_L1EM15 || HLT_g50_loose || HLT_g60_loose || HLT_g70_loose || HLT_g80_loose || HLT_g100_loose || HLT_g120_loose || HLT_g140_loose || HLT_g140_loose_HLTCalo || HLT_g200_etcut || HLT_g10_etcut || HLT_g20_etcut_L1EM12 || HLT_g300_etcut_L1EM24VHIM || HLT_g22_tight || HLT_g25_medium_L1EM22VHI || HLT_g35_loose_L1EM22VHI || HLT_g45_tight_L1EM22VHI || HLT_g35_loose_L1EM24VHI || HLT_g35_loose_L1EM26VHI || HLT_g10_medium || HLT_g15_loose_L1EM3 || HLT_g15_loose || HLT_g20_loose || HLT_g20_tight || HLT_g40_tight || HLT_g45_tight || HLT_g60_loose_L1EM15VH  || HLT_g180_loose || HLT_g60_loose_L1EM24VHI || HLT_g70_loose_L1EM24VHI || HLT_g80_loose_L1EM24VHI || HLT_g100_loose_L1EM24VHI || HLT_g120_loose_L1EM24VHI || HLT_g60_loose_L1EM26VHI || HLT_g70_loose_L1EM26VHI || HLT_g80_loose_L1EM26VHI || HLT_g100_loose_L1EM26VHI || HLT_g120_loose_L1EM26VHI || HLT_g140_loose_L1EM26VHI || HLT_g160_loose_L1EM26VHI || HLT_g180_loose_L1EM26VHI || HLT_200_loose_L1EM26VHI || HLT_g20_loose_L1EM18VH || HLT_g24_loose || HLT_g35_medium_L1EM22VHI || HLT_g35_medium_L1EM24VHI || HLT_g10_loose_L1EM3 || HLT_g10_medium_L1EM3 || HLT_g140_tight_L1EM24VHIM || HLT_g200_loose_L1EM24VHIM || HLT_g20_tight_L1EM15VHI || HLT_g20_tight_icalovloose_L1EM15VHI || HLT_g20_tight_icalotight_L1EM15VHI || HLT_g22_tight_L1EM15VHI || HLT_g22_tight_icalovloose_L1EM15VHI || HLT_g22_tight_icalotight_L1EM15VHI || HLT_g25_loose_L1EM20VH || HLT_g12_loose || HLT_g12_medium || HLT_g70_loose_L1EN24VHIM || HLT_g80_loose_L1EM24VHIM || HLT_g80_loose_icalovloose_L1EM24VHIM || HLT_g60_loose_L1EM24VHIM || HLT_g100_loose_L1EM24VHIM || HLT_g120_loose_L1EM24VHIM || HLT_g140_loose_L1EM24VHIM || HLT_g160_loose_L1EM24VHIM || HLT_g180_loose_L1EM24VHIM || HLT_g35_loose_L1EM24VHIM || HLT_g35_tight_icalotight_L1EM24VHIM || HLT_g40_tight_icalotight_L1EM24VHIM || HLT_g85_tight_L1EM24VHIM || HLT_g85_tight_icalovloose_L1EM24VHIM || HLT_g100_tight_L1EM24VHIM || HLT_g100_tight_icalovloose_L1EM24 || HLT_g45_tight_L1EM24VHI || HLT_g300_etcut_L1EM24VHI || HLT_g85_tight_L1EM24VHI || HLT_g100_tight || HLT_g100_tight_L1EM24VHI || HLT_g100_tight_icalovloose_L1EM24VHIM || HLT_g70_loose_L1EM24VHIM || HLT_g85_tight )'


diPhotonTriggerRequirements = '( HLT_2g20_loose_L12EM15 || HLT_2g20_loose || HLT_2g20_tight || HLT_2g22_tight || HLT_2g25_tight || HLT_g35_loose_g25_loose || HLT_g35_medium_HLTCalo_g25_medium_HLTCalo || HLT_g35_loose_L1EM15_g25_loose_L1EM15 || HLT_g35_loose_L1EM15VH_g25_loose_L1EM15VH || HLT_g35_medium_g25_medium || HLT_2g50_loose || HLT_2g60_loose_L12EM15VH  || HLT_2g10_loose || HLT_2g50_loose_L12EM18VH || HLT_2g60_loose_L12EM18VH || HLT_2g50_loose_L12EM20VH || HLT_g50_loose_L12EM18VH || HLT_g60_loose_L12EM18VH ||  HLT_g50_loose_L12EM20VH || HLT_g60_loose_L12EM20VH || HLT_2g25_tight_L12EM20VH || HLT_g35_loose_g25_loose_L12EM18VH || HLT_g35_loose_g25_loose_L12EM20VH  || HLT_g35_medium_g25_medium_L12EM18VH || HLT_g35_medium_g25_medium_L12EM20VH || HLT_2g20_tight_L12EM15VHI || HLT_2g20_tight_icalovloose_L12EM15VHI || HLT_2g20_tight_icalotight_L12EM15VHI || HLT_2g22_tight_L12EM15VHI || HLT_2g22_tight_icalovloose_L12EM15VHI || HLT_2g22_tight_icalotight_L12EM15VHI || HLT_2g60_loose_L12EM20VH )'


triPhotonTriggerRequirements = '( HLT_3g15_loose || HLT_g20_loose_2g15_loose_L12EM13VH || HLT_2g20_loose_g15_loose || HLT_3g20_loose || HLT_3g20_loose_L12EM18VH || HLT_2g24_loose_g15_loose || HLT_2g24_g20_loose || HLT_3g24_loose_L12EM20VH || HLT_2g25_loose_g15_loose || HLT_2g25_loose_g20_loose || HLT_3g25_loose )'



electronPhotonTriggerRequirements = '( HLT_e24_medium_L1EM15VH_g25_medium || HLT_e20_medium_g35_loose || HLT_e24_lhmedium_L1EM15VH_g25_medium || HLT_e20_lhmedium_g35_loose || HLT_e20_lhmedium_2g10_loose || HLT_e24_lhmedium_L1EM15VH_g25_medium || HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium || HLT_e20_lhmedium_nod0_g35_loose || HLT_e20_lhmedium_nod0_2g10_loose || HLT_e20_lhmedium_nod0_2g10_medium || HLT_e24_lhmedium_nod0_L1EM18VH_g25_medium || HLT_e34_lhmedium_nod0_L1EM20VH_g25_medium || HLT_e20_lhmedium_nod0_g35_loose_L12EM18VH || HLT_e20_lhmedium_nod0_2g10_loose_L1EM15VH_3EM8VH || HLT_e20_lhmedium_nod0_2g10_loose_L1EM18VH_3EM8VH || HLT_e20_lhmedium_nod0_2g10_medium_L1EM15VH_3EM8VH || HLT_e25_mergedtight_g35_medium || HLT_e30_mergedtight_g35_medium || HLT_e24_lhmedium_nod0_2g12_loose || HLT_e24_lhmedium_nod0_2g12_medium || HLT_e24_lhmedium_nod0_L1EM20VH_g25_medium || HLT_e30_mergedtight_g35_medium_Heg )'

firstDataTriggerRequirements = '( HLT_2g10_loose || HLT_g20_loose_L1EM15 )'
# 2016 data triggers (preliminary)
# full list avaiable here: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/TriggerMenu/trunk/python/menu/Physics_pp_v6.py (good luck)
singleElectronTriggerRequirements_2016 = '( HLT_e24_lhtight_iloose || HLT_e26_lhtight_iloose || HLT_e28_lhtight_iloose || HLT_e60_lhmedium || HLT_e140_lhloose || HLT_e24_lhmedium_ivarloose || HLT_e24_lhmedium_nod0_ivarloose || HLT_e24_lhtight_ivarloose || HLT_e24_lhtight_nod0_ivarloose || HLT_e26_lhtight_ivarloose || HLT_e26_lhtight_nod0_ivarloose || HLT_e26_lhtight_smooth_ivarloose || HLT_e60_medium || HLT_e60_lhmedium || HLT_e60_lhmedium_nod0 )'


diElectronTriggerRequirements_2016 = '( HLT_2e15_lhvloose_L12EM13VH || HLT_2e17_lhvloose || HLT_2e12_lhvloose_L12EM10VH || HLT_2e12_lhvloose_nod0_L12EM10VH || HLT_2e17_lhloose || HLT_2e15_lhvloose_nod0_L12EM13VH || HLT_2e17_lhvloose_nod0 )'

triElectronTriggerRequirements_2016 = '( HLT_e17_loose_2e9_loose || HLT_e17_medium_2e9_medium )'

singlePhotonTriggerRequirements_2016 = '( HLT_g140_loose || HLT_g160_loose || HLT_g200_loose || HLT_g140_tight || HLT_g250_etcut || HLT_g300_etcut )'

diPhotonTriggerRequirements_2016 = '( HLT_g35_medium_g25_medium || HLT_2g50_loose || HLT_2g60_loose_L12EM15VH || HLT_2g20_tight || HLT_2g22_tight )'

triPhotonTriggerRequirements_2016 = '( HLT_2g20_loose_g15_loose || HLT_3g20_loose )'

noalgTriggers = '(HLT_noalg_L1EM12 || HLT_noalg_L1EM15 || HLT_noalg_L1EM18VH || HLT_noalg_L1EM20VH || HLT_noalg_L1EM10 || HLT_noalg_L1EM10VH || HLT_noalg_L1EM13VH || HLT_noalg_L1EM20VHI || HLT_noalg_L1EM22VHI || HLT_noalg_L1EM8VH || HLT_noalg_L1EM15VH || HLT_noalg_L12EM7 || HLT_noalg_L12EM15)'
 
expression = ' ( ' + photonSelection + ' || ' + electronSelection + ' ) && ( ' + singlePhotonTriggerRequirements + ' || ' + diPhotonTriggerRequirements + ' || ' + triPhotonTriggerRequirements + ' || ' + electronPhotonTriggerRequirements + ' || ' + firstDataTriggerRequirements + ' || ' + singlePhotonTriggerRequirements_2016 + ' || ' + diPhotonTriggerRequirements_2016 + ' || ' + triPhotonTriggerRequirements_2016 +' || '+ noalgTriggers +') '

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM2SkimmingTool1",
                                                                 expression = expression)
ToolSvc += STDM2SkimmingTool


#====================================================================
# CP group tool : PhotonPointing
#====================================================================
from PhotonVertexSelection.PhotonVertexSelectionConf import CP__PhotonPointingTool
from RecExConfig.RecFlags  import rec

STDM2PhotonPointingTool = CP__PhotonPointingTool(name = "STDM2PhotonPointingTool",
                                                 isSimulation = DerivationFrameworkIsMonteCarlo)
ToolSvc += STDM2PhotonPointingTool


#====================================================================
# PhotonVertexSelectionWrapper decoration tool
#====================================================================
from DerivationFrameworkSM.DerivationFrameworkSMConf import DerivationFramework__PhotonVertexSelectionWrapper
STDM2_PhotonVertexSelectionWrapper = DerivationFramework__PhotonVertexSelectionWrapper( 
                                                             name = "STDM2_PhotonVertexSelectionWrapper",
                                                             PointingTool = STDM2PhotonPointingTool,
                                                             DecorationPrefix = "STDM2",
                                                             PhotonContainerName = "Photons",
                                                             VertexContainerName = "PrimaryVertices")
ToolSvc += STDM2_PhotonVertexSelectionWrapper


#====================================================================
# THINNING TOOLS
#====================================================================

# All Track within a cone DeltaR=0.6 around Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2ElectronTPThinningTool",
                                                             ThinningService        = "STDM2ThinningSvc",
                                                             SGKey                  = "Electrons",
                                                             GSFTrackParticlesKey   = "GSFTrackParticles",
                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                             SelectionString        = electronRequirements,
                                                             BestMatchOnly          = True,
                                                             ConeSize               = 0.6,
                                                             ApplyAnd               = False)
ToolSvc += STDM2ElectronTPThinningTool

# Track associated to all Electrons for ambiguity resolver tool
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2ElectronTPThinningToolAR = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2ElectronTPThinningToolAR",
                                                             ThinningService        = "STDM2ThinningSvc",
                                                             SGKey                  = "Electrons",
                                                             GSFTrackParticlesKey   = "GSFTrackParticles",
                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                             BestMatchOnly          = True,
                                                             ApplyAnd               = False)
ToolSvc += STDM2ElectronTPThinningToolAR

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
STDM2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "STDM2PhotonTPThinningTool",
                                                           ThinningService        = "STDM2ThinningSvc",
                                                           SGKey                  = "Photons",
                                                           GSFTrackParticlesKey   = "GSFTrackParticles",
                                                           InDetTrackParticlesKey = "InDetTrackParticles",
                                                           SelectionString        = photonRequirements,
                                                           BestMatchOnly          = True,
                                                           ConeSize               = 0.6,
                                                           ApplyAnd               = False)
ToolSvc += STDM2PhotonTPThinningTool

# Possibility to thin CaloCalTopoClusters (UE/PU iso corrections not recomputable then)
# see https://indico.cern.ch/event/532191/contributions/2167754/attachments/1273075/1887639/ArthurLesage_ASGMeeting_20160513.pdf, S6-7

#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
STDM2_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "STDM2_MaxCellDecoratorTool",
                                                                    SGKey_electrons = "Electrons",
                                                                    SGKey_photons   = "Photons"
                                                                  )
ToolSvc += STDM2_MaxCellDecoratorTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE
STDM2Sequence = CfgMgr.AthSequencer("STDM2Sequence")

# ADD KERNEL
STDM2Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM2Kernel",
                              SkimmingTools = [STDM2SkimmingTool],
                              AugmentationTools = [STDM2_PhotonVertexSelectionWrapper,STDM2_MaxCellDecoratorTool],
                              ThinningTools = [STDM2ElectronTPThinningTool,STDM2ElectronTPThinningToolAR,STDM2PhotonTPThinningTool])



isMC = globalflags.DataSource()=='geant4'
# JET REBUILDING
# New method to replace the jet collections removed in the AOD size reduction
reducedJetList = ["AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt4PV0TrackJets"]
replaceAODReducedJets(reducedJetList, STDM2Sequence, "STDM2");

# ADD SEQUENCE TO JOB
DerivationFrameworkJob += STDM2Sequence
        
        
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM2Stream )
STDM2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM2Stream.AcceptAlgs(["STDM2Kernel"])


# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="STDM2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM2SlimmingHelper = SlimmingHelper("STDM2SlimmingHelper")
STDM2SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4LCTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

STDM2SlimmingHelper.IncludeEGammaTriggerContent = True

STDM2SlimmingHelper.ExtraVariables = ExtraContentElectrons+ExtraContentPhotons+ExtraContentBtag+ExtraContentVtx+ExtraContentTrk+ExtraContentJets+["Electrons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]+["Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"]+["PrimaryVertices.STDM2_sumPt.STDM2_sumPt2.STDM2_pt.STDM2_eta.STDM2_phi"]+ExtraVariablesEventShape
STDM2SlimmingHelper.ExtraVariables += ["AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"]
STDM2SlimmingHelper.AllVariables = ExtraContainersJets + ["CaloCalTopoClusters"] #+ExtraContainers6Jets #do not exist for now

# # btagging variables
from  DerivationFrameworkFlavourTag.BTaggingContent import *

STDM2SlimmingHelper.ExtraVariables += BTaggingStandardContent("AntiKt4EMTopoJets")
STDM2SlimmingHelper.AppendToDictionary.update({
        "BTagging_AntiKt4EMTopo":    "xAOD::BTaggingContainer",
        "BTagging_AntiKt4EMTopoAux": "xAOD::BTaggingAuxContainer"})

if DerivationFrameworkIsMonteCarlo:
    STDM2SlimmingHelper.ExtraVariables += ExtraElectronsTruth+ExtraPhotonsTruth#+ExtraVariablesTruthEventShape
    STDM2SlimmingHelper.AllVariables   += ExtraContainersTruth+ExtraContainersTruthPhotons#["AntiKt6TruthJets", "BTagging_AntiKt6TruthWZ","AntiKt6TruthWZJets"] do not exist for now
    STDM2SlimmingHelper.AllVariables   += ["TruthIsoCentralEventShape", "TruthIsoForwardEventShape"]
   # STDM2SlimmingHelper.AppendToDictionary = ExtraDictionary
    #STDM2SlimmingHelper.AppendToDictionary = ExtraDictionaryTruthEventShape

    STDM2SlimmingHelper.AppendToDictionary.update({
    "STDMTruthMuons":"xAOD::TruthParticleContainer" ,
    "STDMTruthMuonsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthElectrons":"xAOD::TruthParticleContainer" ,
    "STDMTruthElectronsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthPhotons":"xAOD::TruthParticleContainer" ,
    "STDMTruthPhotonsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthNeutrinos":"xAOD::TruthParticleContainer" ,
    "STDMTruthNeutrinosAux":"xAOD::TruthParticleAuxContainer", 
    "STDMTruthTaus":"xAOD::TruthParticleContainer" ,
    "STDMTruthTausAux":"xAOD::TruthParticleAuxContainer",
    "TruthIsoCentralEventShape":"xAOD::EventShape",
    "TruthIsoCentralEventShapeAux":"xAOD::EventShapeAuxInfo",
    "TruthIsoForwardEventShape":"xAOD::EventShape",
    "TruthIsoForwardEventShapeAux":"xAOD::EventShapeAuxInfo"})

STDM2SlimmingHelper.AppendContentToStream(STDM2Stream)
