#********************************************************************
# EXOT3.py 
# reductionConf flag EXOT3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from JetRec.JetRecStandard import jtm

from DerivationFrameworkCore.WeightMetadata import *

exot3Seq = CfgMgr.AthSequencer("EXOT3Sequence")

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

#====================================================================
# THINNING TOOL 
#====================================================================

thinningTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# MET/Jet tracks
#met_thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
#EXOT3MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                = "EXOT3MetTPThinningTool",
#                                                                ThinningService         = "EXOT3ThinningSvc",
#                                                                SelectionString         = met_thinning_expression,
#                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT3MetTPThinningTool
#
#EXOT3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "EXOT3JetTPThinningTool",
#                                                                ThinningService         = "EXOT3ThinningSvc",
#                                                                JetKey                  = "AntiKt4LCTopoJets",
#                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += EXOT3JetTPThinningTool



thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT3TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT3TPThinningTool",
                                                                 ThinningService         = "EXOT3ThinningSvc",
                                                                 SelectionString         = thinExpression,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3TPThinningTool
thinningTools.append(EXOT3TPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT3MuonTPThinningTool",
                                                                            ThinningService         = "EXOT3ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3MuonTPThinningTool
thinningTools.append(EXOT3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT3ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                                        SGKey             	= "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3ElectronTPThinningTool
thinningTools.append(EXOT3ElectronTPThinningTool)

EXOT3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT3PhotonTPThinningTool",
                                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                                        SGKey             	= "Photons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3PhotonTPThinningTool
thinningTools.append(EXOT3PhotonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT3AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                = "EXOT3AKt4JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "AntiKt4LCTopoJets",
                                                                        SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt4JetTPThinningTool
thinningTools.append(EXOT3AKt4JetTPThinningTool)

EXOT3AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "EXOT3AKt10JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt10JetTPThinningTool
thinningTools.append(EXOT3AKt10JetTPThinningTool)

EXOT3AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "EXOT3Cam12JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "CamKt12LCTopoJets",
                                                                        SelectionString         = "CamKt12LCTopoJets.pt > 150*GeV && abs(CamKt12LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt10JetTPThinningTool
thinningTools.append(EXOT3AKt10JetTPThinningTool)


# Calo-cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT3AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "EXOT3Ak10CCThinningTool",
                                                                      ThinningService       = "EXOT3ThinningSvc",
                                                                      SGKey                 = "AntiKt10LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                      ConeSize              = 0)
ToolSvc += EXOT3AKt10CCThinningTool
thinningTools.append(EXOT3AKt10CCThinningTool)

EXOT3CA12CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "EXOT3CA12CCThinningTool",
                                                                      ThinningService       = "EXOT3ThinningSvc",
                                                                      SGKey                 = "CamKt12LCTopoJets",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "CamKt12LCTopoJets.pt > 150*GeV && abs(CamKt12LCTopoJets.eta) < 2.8",
                                                                      ConeSize              = 0)
ToolSvc += EXOT3CA12CCThinningTool
thinningTools.append(EXOT3CA12CCThinningTool)

#Truth Thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT3MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                = "EXOT3MCThinningTool",
        ThinningService     = "EXOT3ThinningSvc",
        WriteEverything     = False,
        WritePartons        = False,
        WriteHadrons        = True,
        WriteBHadrons       = True,
        WriteGeant          = False,
        WriteTauHad         = False,
        WriteBSM            = True,
        WriteBosons         = True,
        WriteBosonProducts  = True,
        WriteBSMProducts    = True,
        WriteTopAndDecays   = True,
        WriteAllLeptons     = True,
        WriteStatus3        = False,
        WriteFirstN         = -1)

if isMC:
  ToolSvc += EXOT3MCThinningTool
  thinningTools.append(EXOT3MCThinningTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================

triggers = ["HLT_j200",
            "HLT_j260",
            "HLT_j300",
            "HLT_j320",
            "HLT_j360",
            "HLT_j400",
            "HLT_j400_sub",
            "HLT_j400_jes",
            "HLT_j400_lcw",
            "HLT_j400_lcw_jes",
            "HLT_ht700",
            "HLT_ht850",
            "HLT_ht1000",
            "HLT_ht1000_L1J100",
            "HLT_j360_a10_nojcalib",
            "HLT_j360_a10_nojcalib_L1HT150-J20.ETA31",
            "HLT_j360_a10_nojcalib_L1J100",
            "HLT_j360_a10_lcw_nojcalib",
            "HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31",
            "HLT_j360_a10_sub",
            "HLT_j360_a10_sub_L1HT150-J20.ETA31",
            "HLT_j360_a10_sub_L1J100",
            "HLT_j360_a10_lcw_sub",
            "HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31",
            "HLT_j360_a10_lcw_sub_L1J100",
            "HLT_j360_a10r",
            "HLT_j360_a10r_L1J100",
            "HLT_j460_a10_nojcalib",
            "HLT_j460_a10_nojcalib_L1HT190-J15.ETA21",
            "HLT_j460_a10_nojcalib_L1J100",
            "HLT_j460_a10_lcw_nojcalib",
            "HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21",
            "HLT_j460_a10_lcw_nojcalib_L1J100",
            "HLT_j460_a10_sub",
            "HLT_j460_a10_sub_L1HT190-J15.ETA21",
            "HLT_j460_a10_sub_L1J100",
            "HLT_j460_a10_lcw_sub",
            "HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21",
            "HLT_j460_a10_lcw_sub_L1J100",
            "HLT_j460_a10r",
            "HLT_j460_a10r_L1HT190-J15.ETA21",
            "HLT_j460_a10r_L1J100", "EF_J360_a10tcem",
            'HLT_j380',
            'HLT_j380_jes',
            'HLT_j380_lcw',
            'HLT_j380_lcw_jes',
            'HLT_j380_lcw_nojcalib',
            'HLT_j380_nojcalib']


topology_selection_1jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 1)"
topology_selection_1jet += " || (count (abs(CamKt12LCTopoBDRSFilteredMU100Y4Jets.eta) < 2.8 && CamKt12LCTopoBDRSFilteredMU100Y4Jets.pt > 100*GeV && CamKt12LCTopoBDRSFilteredMU100Y4Jets.m > 30*GeV)  >= 1))"
topology_selection_2jet = "((count (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)  >= 2)"
topology_selection_2jet += " || (count (abs(CamKt12LCTopoBDRSFilteredMU100Y4Jets.eta) < 2.8 && CamKt12LCTopoBDRSFilteredMU100Y4Jets.pt > 100*GeV && CamKt12LCTopoBDRSFilteredMU100Y4Jets.m > 30*GeV)  >= 2))"
EXOT6_trigger1 = "(EF_g120_loose || EF_xe80_tclcw_tight)"
EXOT6_trigger2 = "(HLT_g120_loose || HLT_g140_loose || HLT_xe100)"
EXOT6_selection = "((count(Photons.pt > 100*GeV) > 0) || (count(Electrons.pt > 100*GeV) > 0))"

beamEnergy = jobproperties.Beam.energy()
expression = ''
expression2 = ''
if (beamEnergy < 4.1e+06):
    expression = '(((EventInfo.eventTypeBitmask==1) || ' + EXOT6_trigger1 + ') && ' + EXOT6_selection + ' && ' + topology_selection_1jet + ' ) '#|| ( ((EventInfo.eventTypeBitmask==1) || ' + r1_trigger_selection + ' || ' + r2_trigger_selection + ' ) && ' + topology_selection_2jet + ' ) '
    expression2 = topology_selection_2jet
if (beamEnergy > 6.0e+06):
#    expression = '(HLT_g120_loose || HLT_g140_loose || HLT_xe100 || ' + r2_trigger_selection + ' ) && (((count(Photons.Loose && Photons.pt > 100*GeV) > 0) || (count(Electrons.Medium && Electrons.pt > 100*GeV) > 0)) || ' + topology_selection + ')'
    expression = '( ' + EXOT6_trigger2 + ' && ' + EXOT6_selection + ' && ' + topology_selection_1jet + ' )'# || ( ' + r2_trigger_selection + ' && ' + topology_selection_2jet + ' ) '
    expression2 = topology_selection_2jet

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT3SkimmingTool1 = DerivationFramework__xAODStringSkimmingTool( name = "EXOT3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOT3SkimmingTool1

EXOT3SkimmingTool2 = DerivationFramework__xAODStringSkimmingTool( name = "EXOT3SkimmingTool2",
                                                                    expression = expression2)
ToolSvc += EXOT3SkimmingTool2

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT3TriggerSkimmingTool",
                                                                     TriggerListAND = [],
                                                                     TriggerListOR  = triggers)
ToolSvc += EXOT3TriggerSkimmingTool

# topology_selection_2jet && triggers
EXOT3ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "EXOT3ANDSkimmingTool", FilterList = [EXOT3SkimmingTool2, EXOT3TriggerSkimmingTool])
ToolSvc += EXOT3ANDSkimmingTool

# expression || (topology_selection_2jet && triggers)
EXOT3FinalSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT3ORSkimmingTool", FilterList = [EXOT3ANDSkimmingTool, EXOT3SkimmingTool1])
ToolSvc += EXOT3FinalSkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot3Seq


#=======================================
# Augmentation: Jet Collections
#=======================================

from JetRec.JetRecConf import JetAlgorithm

#SplitFiltered
if isMC:
  addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.04, algseq=exot3Seq, outputGroup="EXOT3")
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.04, algseq=exot3Seq, outputGroup="EXOT3")
addFilteredJets("CamKt", 1.2, "PV0Track", mumax=1.0, ymin=0.04, algseq=exot3Seq, outputGroup="EXOT3")

# Trimmed jets
if isMC:
  addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05, algseq=exot3Seq, outputGroup="EXOT3")

#addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, algseq=exot3Seq, outputGroup="EXOT3")
addDefaultTrimmedJets(exot3Seq, "EXOT3")
addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=exot3Seq, outputGroup="EXOT3")

applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot3Seq)

exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel_skim", SkimmingTools = [EXOT3FinalSkimmingTool])
exot3Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT3Kernel",
#									ThinningTools = [EXOT3TPThinningTool,EXOT3MuonTPThinningTool,EXOT3ElectronTPThinningTool, EXOT3PhotonTPThinningTool]
									ThinningTools = thinningTools
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT3Stream )
EXOT3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT3Stream.AcceptAlgs(["EXOT3Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT3ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT3ContentList import *

EXOT3SlimmingHelper = SlimmingHelper("EXOT3SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT3SlimmingHelper.SmartCollections = EXOT3SmartContent
EXOT3SlimmingHelper.ExtraVariables = EXOT3ExtraVariables

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT3SlimmingHelper.AllVariables = EXOT3AllVariablesContent

# Add jet collections created by derivation job
EXOT3SlimmingHelper.StaticContent = EXOT3StaticContent

# (Dont) Add jet triger content
#EXOT3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT3SlimmingHelper.IncludeJetTriggerContent = True
EXOT3SlimmingHelper.IncludeEGammaTriggerContent = True

EXOT3SlimmingHelper.AppendContentToStream(EXOT3Stream)
