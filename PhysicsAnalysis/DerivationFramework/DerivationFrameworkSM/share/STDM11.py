#********************************************************************
# STDM11.py 
# reductionConf flag STDM11 in Reco_tf.py   
# Format for inclusive jets and leading jets measurements
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import (
    derivationFlags, buildFileName, MSMgr, DerivationFrameworkJob)
from DerivationFrameworkJetEtMiss.JetCommon import addJetOutputs
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkCore.LHE3WeightMetadata import addLHE3Weights
from DerivationFrameworkInDet.InDetCommon import *

# Add Truth MetaData
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM11Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM11Stream )
STDM11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM11Stream.AcceptAlgs(["STDM11Kernel"])

isMC = globalflags.DataSource()=='geant4'

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

#=====================
# TRIGGER NAV THINNING
#=====================
# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
STDM11ThinningHelper = ThinningHelper( "STDM11ThinningHelper" )

#trigger navigation content
STDM11ThinningHelper.TriggerChains = 'HLT_j.*|HLT_ht.*'
STDM11ThinningHelper.AppendToStream( STDM11Stream )


#====================================================================
# Thin TruthParticles for truth jet constituents
#====================================================================


if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__ViewContainerThinning
  STDM11TruthJetInputThin = DerivationFramework__ViewContainerThinning( name = "STDM11ViewContThinning",
                                                                        ThinningService        = STDM11ThinningHelper.ThinningSvc(),
                                                                        SourceContainer = "TruthParticles",
                                                                        ViewContainer = "JetInputTruthParticles",
                                                                        ParticleType = 201, # truthParticles
                                                                        ApplyAnd = False)

  ToolSvc += STDM11TruthJetInputThin
  thinningTools.append(STDM11TruthJetInputThin)
  
#########################################
# Tracks associated with jets
#########################################
#from ThinningUtils.ThinningUtilsConf import DeltaRThinningTool

from DerivationFrameworkInDet.InDetCommon import *
STDM11BaselineTrack = "(InDetTrackParticles.DFCommonTightPrimary) && (InDetTrackParticles.pt > 0.1*GeV) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
STDM11TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name            = "STDM11TrackParticleThinningTool",
                                                                             ThinningService = STDM11ThinningHelper.ThinningSvc(),
                                                                             SelectionString = STDM11BaselineTrack,
                                                                             InDetTrackParticlesKey = "InDetTrackParticles",
                                                                             ApplyAnd        = True)
ToolSvc += STDM11TrackParticleThinningTool
thinningTools.append(STDM11TrackParticleThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM11Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM11Akt4PFlowJetTPThinningTool",
                                                                                  ThinningService         = STDM11ThinningHelper.ThinningSvc(),
                                                                                  JetKey                  = "AntiKt4EMPFlowJets",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                  ApplyAnd                = False)
ToolSvc += STDM11Akt4PFlowJetTPThinningTool
thinningTools.append(STDM11Akt4PFlowJetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM11Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM11Akt4JetTPThinningTool",
                                                                            ThinningService         = STDM11ThinningHelper.ThinningSvc(),
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = False)
ToolSvc += STDM11Akt4JetTPThinningTool
thinningTools.append(STDM11Akt4JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
STDM11Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "STDM11Akt4PFlowJetTPThinningTool",
                                                                                  ThinningService         = STDM11ThinningHelper.ThinningSvc(),
                                                                                  JetKey                  = "AntiKt4EMPFlowJets",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                  ApplyAnd                = False)
ToolSvc += STDM11Akt4PFlowJetTPThinningTool
thinningTools.append(STDM11Akt4PFlowJetTPThinningTool)



#====================================================================
# SKIMMING TOOL 
#====================================================================
triggers = ["HLT_noalg_mb_L1J12","HLT_mb_sptrk","HLT_j0_perf_L1RD0_FILLED","HLT_j15","HLT_j25","HLT_j35","HLT_j45","HLT_j55","HLT_j60","HLT_j85","HLT_j100","HLT_j110","HLT_j150","HLT_j175","HLT_j200","HLT_j260","HLT_j300","HLT_j320","HLT_j360","HLT_j380","HLT_j400","HLT_j420","HLT_j440","HLT_j450","HLT_j460","HLT_j420_320eta490","HLT_j400_320eta490","HLT_j380_320eta490","HLT_j260_320eta490","HLT_j200_320eta490","HLT_j175_320eta490","HLT_j150_320eta490","HLT_j110_320eta490","HLT_j85_320eta490","HLT_j60_320eta490","HLT_j55_320eta490","HLT_j45_320eta490","HLT_j35_320eta490","HLT_j25_320eta490","HLT_j15_320eta490"]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
STDM11TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "STDM11TriggerSkimmingTool",
                                                                     TriggerListAND = [],
                                                                     TriggerListOR  = triggers)
ToolSvc += STDM11TriggerSkimmingTool

STDM11ANDSkimmingTool = DerivationFramework__FilterCombinationAND(name = "STDM11ANDSkimmingTool", FilterList = [STDM11TriggerSkimmingTool])
ToolSvc += STDM11ANDSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM11Sequence = CfgMgr.AthSequencer("STDM11Sequence")

# ADD KERNEL 
STDM11Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM11Kernel",
                                                                 SkimmingTools = [STDM11ANDSkimmingTool],
                                                                 ThinningTools = thinningTools)

# JET REBUILDING
reducedJetList = ["AntiKt4TruthWZJets", "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList, STDM11Sequence, "STDM11Jets")

# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM11Sequence

#====================================================================
# Jet reconstruction/retagging
#====================================================================s
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = STDM11Sequence)

#improved fJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation,getPFlowfJVT
# MVfJvt #
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=STDM11Sequence, algname='JetForwardJvtToolBDTAlg')
# PFlow fJvt #
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=STDM11Sequence, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM11SlimmingHelper = SlimmingHelper("STDM11SlimmingHelper")
STDM11SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                         "AntiKt4EMPFlowJets", 
                                         "InDetTrackParticles",					
                                         "PrimaryVertices"]

STDM11SlimmingHelper.ExtraVariables = ExtraContentJets_STDM11

STDM11SlimmingHelper.ExtraVariables += ExtraContentTrk

STDM11SlimmingHelper.ExtraVariables += ["InDetTrackParticles.truthMatchProbability"]

#STDM11SlimmingHelper.ExtraVariables += ["TruthParticles.px.py.pz.e.m.phi.pdgId.status.barcode"]

STDM11SlimmingHelper.AllVariables = ["TruthEvents", "TruthParticles"]


addJetOutputs(STDM11SlimmingHelper,["STDM11","STDM11Jets"])

STDM11SlimmingHelper.IncludeJetTriggerContent = True

STDM11SlimmingHelper.AppendContentToStream(STDM11Stream)
