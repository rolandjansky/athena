#********************************************************************
# STDM6.py 
# reductionConf flag STDM6 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *


#====================================================================                                               
# SET UP STREAM 
#====================================================================                                               

streamName = derivationFlags.WriteDAOD_STDM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM6Stream )
STDM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
STDM6Stream.AcceptAlgs(["STDM6Kernel"])




#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# CREATE THE PRIVATE SEQUENCE 
STDM6Sequence = CfgMgr.AthSequencer("STDM6Sequence")

# ADD KERNEL 
STDM6Sequence += CfgMgr.DerivationFramework__DerivationKernel("STDM6Kernel",
                                                                 SkimmingTools = [],
                                                                 ThinningTools = [])
# FIX TRUTH JETS
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(STDM6Sequence,"STDM6")

#ADD MBTS Container
from xAODForwardCnv.xAODMBTSModuleCreator import xAODMaker__MBTSModuleCnvAlg
STDM6Sequence +=  xAODMaker__MBTSModuleCnvAlg()


# ADD SEQUENCE TO JOB  
DerivationFrameworkJob += STDM6Sequence


#====================================================================
# SET UP STREAM
#====================================================================
#streamName = derivationFlags.WriteDAOD_STDM6Stream.StreamName
#fileName   = buildFileName( derivationFlags.WriteDAOD_STDM6Stream )
#STDM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
#STDM6Stream.AcceptAlgs(["STDM6Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="STDM6ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSM.STDMExtraContent import *

STDM6SlimmingHelper = SlimmingHelper("STDM6SlimmingHelper")
STDM6SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4LCTopoJets",
                                        "BTagging_AntiKt4LCTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"  ]

STDM6SlimmingHelper.AllVariables=[ "CaloCalTopoClusters","ALFADataContainer","MBTSModules"]
STDM6SlimmingHelper.AppendToDictionary = {"MBTSModules":"xAOD::MBTSModuleContainer","MBTSModulesAux":"xAOD::MBTSModuleAuxContainer"}

STDM6SlimmingHelper.IncludeEGammaTriggerContent = True
STDM6SlimmingHelper.IncludeMuonTriggerContent = True


STDM6SlimmingHelper.ExtraVariables = ExtraContentAll
STDM6SlimmingHelper.AllVariables += ExtraContainersAll
if globalflags.DataSource()=='geant4':
    STDM6SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    STDM6SlimmingHelper.AllVariables += ExtraContainersTruth
    
STDM6SlimmingHelper.AppendContentToStream(STDM6Stream)





if globalflags.DataSource()=='geant4':
    STDM6Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
    STDM6Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
    STDM6Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
    STDM6Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
    STDM6Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
    STDM6Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
    STDM6Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
    STDM6Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
    STDM6Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
    STDM6Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )


