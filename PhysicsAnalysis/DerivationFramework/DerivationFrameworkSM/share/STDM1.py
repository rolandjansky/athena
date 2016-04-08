7#********************************************************************
# STDM1.py 
# reductionConf flag STDM1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

#=======================================
# SKIMMING TOOL   
#=======================================
skimmingTools = []

expression = '(EventInfo.eventTypeBitmask==1) || (HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_noalg_L1J50)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
STDM1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "STDM1SkimmingTool1",
                                                                 expression = expression)
ToolSvc += STDM1SkimmingTool
skimmingTools.append(STDM1SkimmingTool)

##====================================================================
## THINNING TOOLS
##====================================================================
#
#thinningTools=[]
#if globalflags.DataSource()=='geant4':
#    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
#    STDM1TruthThinningTool = DerivationFramework__MenuTruthThinning(name               = "STDM1TruthThinningTool",
#                                                            ThinningService            = "STDM1ThinningSvc",
#                                                            WritePartons               = False,
#                                                            WriteHadrons               = True,
#                                                            WriteBHadrons              = True,
#                                                            WriteGeant                 = False,
#                                                            GeantPhotonPtThresh        = -1.0,
#                                                            WriteTauHad                = True,
#                                                            PartonPtThresh             = -1.0,
#                                                            WriteBSM                   = False,
#                                                            WriteBosons                = True,
#                                                            WriteBSMProducts           = False,
#                                                            WriteBosonProducts         = True,
#                                                            WriteTopAndDecays          = True,
#                                                            WriteEverything            = False,
#                                                            WriteAllLeptons            = True,
#                                                            WriteStatus3               = False, 
#                                                            WriteFirstN                = -1)
#    ToolSvc += STDM1TruthThinningTool
#    thinningTools.append(STDM1TruthThinningTool)

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"           # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))"           # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  6))"                                                # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.))"                # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    STDM1TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "STDM1TruthThinningTool",
                                                                        ThinningService        = "STDM1ThinningSvc",
                                                                        ParticlesKey = "TruthParticles",
                                                                        VerticesKey = "TruthVertices",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += STDM1TruthThinningTool
    thinningTools.append(STDM1TruthThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

stdm1Seq = CfgMgr.AthSequencer("STDM1Sequence")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
stdm1Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM1Kernel",SkimmingTools=skimmingTools,ThinningTools=thinningTools)

#=======================================
# ATTACH TO THE MAIN SEQUENCE
#=======================================

DerivationFrameworkJob += stdm1Seq

#====================================================================
# Jets for R-scan
#====================================================================

from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm


def addRscanJets(jetalg,radius,inputtype,sequence,outputlist):
    jetname = "{0}{1}{2}Jets".format(jetalg,int(radius*10),inputtype)
    algname = "jetalg"+jetname
    if not hasattr(sequence,algname):
        if inputtype == "Truth":
            addStandardJets(jetalg, radius, "Truth", ptmin=5000, algseq=sequence, outputGroup=outputlist)
        elif inputtype == "LCTopo":
            addStandardJets(jetalg, radius, "LCTopo", mods="calib",
                            ghostArea=0.01, ptmin=2000, ptminFilter=7000, calibOpt="none", algseq=sequence, outputGroup=outputlist)

OutputJets["STDM1"] = []
for radius in [0.2, 0.3, 0.5, 0.6, 0.7, 0.8]:
    if jetFlags.useTruth:
        addRscanJets("AntiKt",radius,"Truth",stdm1Seq,"STDM1")
    addRscanJets("AntiKt",radius,"LCTopo",stdm1Seq,"STDM1")

#====================================================================
# Special jets
#====================================================================

addDefaultTrimmedJets(stdm1Seq,"STDM1")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_STDM1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM1Stream )
STDM1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM1Stream.AcceptAlgs(["STDM1Kernel"]) 

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="STDM1ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM1SlimmingHelper = SlimmingHelper("STDM1SlimmingHelper")
STDM1SlimmingHelper.SmartCollections =["AntiKt4EMTopoJets",
                                       "AntiKt4LCTopoJets",
                                       #"BTagging_AntiKt4LCTopo",
                                       #"BTagging_AntiKt4EMTopo",
                                       "PrimaryVertices" ]


# Trigger content
STDM1SlimmingHelper.IncludeJetTriggerContent = True
#STDM1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True

STDM1SlimmingHelper.AllVariables = ExtraContainersJets
if globalflags.DataSource()=='geant4':
    STDM1SlimmingHelper.AllVariables = [ "TruthVertices", "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#    STDM1SlimmingHelper.AllVariables += ExtraContainersTruth # This list defined in python/STDMExtraContent

# Add the jet containers to the stream
addJetOutputs(STDM1SlimmingHelper,["STDM1"])

STDM1SlimmingHelper.AppendContentToStream(STDM1Stream)
STDM1Stream.AddItem("xAOD::EventShape#*")
STDM1Stream.AddItem("xAOD::EventShapeAuxInfo#*")    
