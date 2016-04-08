#********************************************************************
# HIGG3D2.py 
# reductionConf flag HIGG3D2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
#from DerivationFrameworkInDet.InDetCommon import *
#from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *
#from DerivationFrameworkEGamma.EGammaCommon import *
#from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

#================
# THINNING
#================
thinningTools=[]

# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && (TruthParticles.pt > 10*GeV))" # Electron
truth_cond3 = "((abs(TruthParticles.pdgId) >= 1) && (abs(TruthParticles.pdgId) <= 6))"  # Partons
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D2TruthParticlesThinningTool = DerivationFramework__GenericTruthThinning( name                    = "HIGG3D2TruthParticlesThinningTool", 
                                                                               ThinningService         = "HIGG3D2ThinningSvc",
                                                                               ParticlesKey            = "TruthParticles",
                                                                               ParticleSelectionString = truth_expression,
                                                                               PreserveDescendants     = False,
                                                                               PreserveAncestors       = False
                                                                               )

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D2TruthParticlesThinningTool
    thinningTools.append(HIGG3D2TruthParticlesThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel( "HIGG3D2Kernel",
                                                                        ThinningTools = thinningTools
                                                                        )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D2Stream )
HIGG3D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D2Stream.AcceptAlgs(["HIGG3D2Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG3D2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D2SlimmingHelper = SlimmingHelper("HIGG3D2SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D2ExtraContent import *

HIGG3D2SlimmingHelper.ExtraVariables = HIGG3D2ExtraVariables

if globalflags.DataSource()=='geant4':
    HIGG3D2SlimmingHelper.AllVariables = HIGG3D2ExtraTruthContainers

HIGG3D2SlimmingHelper.AppendContentToStream(HIGG3D2Stream)
