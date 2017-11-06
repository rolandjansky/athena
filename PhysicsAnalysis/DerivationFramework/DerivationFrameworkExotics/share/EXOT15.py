#********************************************************************
# EXOT15.py 
# reductionConf flag EXOT15 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot15Seq = CfgMgr.AthSequencer("EXOT15Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT15Stream )
EXOT15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT15Stream.AcceptAlgs(["EXOT15Kernel"])

#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT15ThinningSvc", outStreams=[evtStream] )

SkipTriggerRequirement=(globalflags.DataSource()=='geant4')  #apply triggers only to data

#====================================================================
# THINNING TOOLS
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT15ThinningHelper = ThinningHelper( "EXOT15ThinningHelper" )
EXOT15ThinningHelper.AppendToStream( EXOT15Stream )

thinningTools = []

thinning_expression = "(InDetTrackParticles.pt > 1.0*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 3)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT15TPThinningTool = DerivationFramework__TrackParticleThinning( name              = "EXOT15TPThinningTool",
                                                                ThinningService         = "EXOT15ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = False)
#ToolSvc += EXOT15TPThinningTool  # do not use track thinning
#thinningTools.append(EXOT15TPThinningTool)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT15TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT15TMCThinningTool",
                                                              ThinningService         = EXOT15ThinningHelper.ThinningSvc(),
                                                              WritePartons               = True,
                                                              WriteHadrons               = True,
                                                              WriteBHadrons              = True,
                                                              WriteGeant                 = False,
                                                              GeantPhotonPtThresh        = -1.0,
                                                              WriteTauHad                = False,
                                                              PartonPtThresh             = -1.0,
                                                              WriteBSM                   = True,
                                                              WriteBosons                = False,
                                                              WriteBosonProducts         = False,
                                                              WriteBSMProducts           = True,
                                                              WriteTopAndDecays          = False,
                                                              WriteEverything            = False,
                                                              WriteAllLeptons            = False,
                                                              WriteLeptonsNotFromHadrons = False,
                                                              WriteStatus3               = False,
                                                              WriteFirstN                = -1,
                                                              PreserveDescendants        = False,
                                                              PreserveGeneratorDescendants        = False,
                                                              PreserveAncestors        = False,
                                                              PreserveParentsSiblingsChildren         = True)

if SkipTriggerRequirement:
    ToolSvc += EXOT15TMCThinningTool
    thinningTools.append(EXOT15TMCThinningTool)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT15MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT15MCGenThinningTool",
                                                                   ThinningService         = EXOT15ThinningHelper.ThinningSvc(),
                                                                   ParticleSelectionString = "abs(TruthParticles.pdgId)==25 || abs(TruthParticles.pdgId)==35", 
                                                                   PreserveDescendants = True,
                                                                   PreserveAncestors   = True)


if SkipTriggerRequirement:
    ToolSvc += EXOT15MCGenThinningTool
    thinningTools.append(EXOT15MCGenThinningTool)


#=======================================
# SKIMMING
#=======================================
skimmingTools = []

if SkipTriggerRequirement: 
    topology_selection = "( (count (abs(AntiKt4EMTopoJets.eta) < 2.8 && AntiKt4EMTopoJets.pt > 20) > 0) || (count (abs(MSDisplacedVertex.z) >= 0) > 0) )"
else: 
    topology_selection = "((count (abs(MSDisplacedVertex.z) >= 0) > 0) )"

expression = topology_selection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT15SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT15SkimmingTool",
                                                                      expression = expression)

ToolSvc += EXOT15SkimmingTool
if not SkipTriggerRequirement and not rec.triggerStream() == 'ZeroBias': # add topology selection only to data. Keep all events in MC
    skimmingTools.append(EXOT15SkimmingTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool

if  SkipTriggerRequirement:
    print "trigger disabled"
elif rec.triggerStream() == 'ZeroBias':
    EXOT15TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT15TriggerSkimmingTool",
                                                                         TriggerListAND = [],
                                                                         TriggerListOR  = ["HLT_noalg_zb_L1ZB"])
    ToolSvc += EXOT15TriggerSkimmingTool
else:
    print "trigger enabled"
    EXOT15TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT15TriggerSkimmingTool",
                                                                         TriggerListAND = [],
                                                                         TriggerListOR  = [
                                                                             "HLT_j30_jes_PS_llp_L1TAU60",
                                                                             "HLT_j30_jes_PS_llp_noiso_L1TAU60",
                                                                             "HLT_j30_muvtx","HLT_j30_muvtx_noiso",
                                                                             #"HLT_j100",
                                                                             #"HLT_j400",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU30", # adding cleanLLP chains for the future
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU60", 
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU60", 
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH",    
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH", 
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO",
                                                                             "HLT_j30_muvtx_L1MU6_EMPTY",
                                                                             "HLT_j30_muvtx_noiso_L1MU6_EMPTY", # adding noiso in EMPTY, UNPAIRED for the future
                                                                             "HLT_j30_muvtx_L1MU4_UNPAIRED_ISO",
                                                                             "HLT_j30_muvtx_noiso_L1MU4_UNPAIRED_ISO",
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_L1TAU100", 
                                                                             "HLT_j30_jes_cleanLLP_PS_llp_noiso_L1TAU100"])

    ToolSvc += EXOT15TriggerSkimmingTool
    skimmingTools.append(EXOT15TriggerSkimmingTool)

if not SkipTriggerRequirement and not rec.triggerStream() == 'ZeroBias':
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
    EXOT15ORSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT15ORSkimmingTool", FilterList = skimmingTools )
    ToolSvc += EXOT15ORSkimmingTool


#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT15"] = []
reducedJetList = [
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot15Seq,"EXOT15")

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot15Seq
if SkipTriggerRequirement:
    exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", SkimmingTools = [EXOT15SkimmingTool], ThinningTools = thinningTools)
elif rec.triggerStream() == 'ZeroBias':
    exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", SkimmingTools = [EXOT15TriggerSkimmingTool])    
else:
    exot15Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT15Kernel", SkimmingTools = [EXOT15ORSkimmingTool])


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT15ContentList import *
EXOT15SlimmingHelper = SlimmingHelper("EXOT15SlimmingHelper")
EXOT15SlimmingHelper.SmartCollections = EXOT15SmartContent
EXOT15SlimmingHelper.AllVariables = EXOT15AllVariablesContent

#EXOT15SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT15SlimmingHelper.IncludeJetTriggerContent = True
EXOT15SlimmingHelper.IncludeBJetTriggerContent = True
EXOT15SlimmingHelper.IncludeTauTriggerContent = True
#EXOT15SlimmingHelper.IncludeMuonTriggerContent = True
EXOT15SlimmingHelper.ExtraVariables += ['HLT_xAOD__JetContainer_a4tcemsubjesFS.m.EMFrac','Electrons.LHMedium','PrimaryVertices.x.y']

EXOT15SlimmingHelper.AppendContentToStream(EXOT15Stream)
