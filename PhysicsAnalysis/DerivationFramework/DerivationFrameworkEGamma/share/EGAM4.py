#********************************************************************
# EGAM4.py
# Z->mumugamma reduction for photon studies
# reductionConf flag EGAM4 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *


#====================================================================
# SKIMMING TOOLS
#====================================================================


#====================================================================
# mumugamma selection for photon studies, single & di-muon triggers
# two opposite-sign muons, pT>15 GeV, |eta|<2.5, mmumu>40 GeV
# gamma: reco, ET>10 GeV, |eta|<2.5
#====================================================================

#requirement = 'Muons.pt>9.5*GeV && abs(Muons.eta)<2.7 && Muons.DFCommonGoodMuon'
requirement = 'Muons.pt>9.5*GeV && abs(Muons.eta)<2.7 && Muons.DFCommonMuonsPreselection'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM4_MuMuMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM4_MuMuMassTool",
                                                               Object1Requirements = requirement,
                                                               Object2Requirements = requirement,
                                                               StoreGateEntryName = "EGAM4_DiMuonMass",
                                                               Mass1Hypothesis = 105*MeV,
                                                               Mass2Hypothesis = 105*MeV,
                                                               Container1Name = "Muons",
                                                               Container2Name = "Muons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM4_MuMuMassTool
print EGAM4_MuMuMassTool


#====================================================================
# SKIMMING TOOL
#====================================================================

expression = 'count(DFCommonPhotons_et>9.5*GeV)>=1 && count(EGAM4_DiMuonMass > 40.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM4_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM4_SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM4_SkimmingTool
print "EGAM4 skimming tool:", EGAM4_SkimmingTool


#====================================================================
# DECORATION TOOLS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM4_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM4_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM4_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]


#================
# THINNING TOOLS
#================
thinningTools=[]
# TO BE ADDED


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam4Seq = CfgMgr.AthSequencer("EGAM4Sequence")
DerivationFrameworkJob += egam4Seq


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
egam4Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM4Kernel",
                                                         #AugmentationTools = [EGAM4_MuMuMassTool,EGAM4_CellDecoratorTool],
                                                         AugmentationTools = [EGAM4_MuMuMassTool,EGAM4_GainDecoratorTool] +  EGAM4_ClusterEnergyPerLayerDecorators,
                                                         SkimmingTools = [EGAM4_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )


#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam4Seq,"EGAM4")


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM4Stream )
EGAM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM4Stream.AcceptAlgs(["EGAM4Kernel"])

#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EGAM4ThinningSvc", outStreams=[evtStream] )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkEGamma.EGAM4ExtraContent import *

EGAM4SlimmingHelper = SlimmingHelper("EGAM4SlimmingHelper")
EGAM4SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM4SlimmingHelper.IncludeEGammaTriggerContent = True
EGAM4SlimmingHelper.IncludeMuonTriggerContent = True

# Extra variables
EGAM4SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM4SlimmingHelper.AllVariables = ExtraContainersPhotons
EGAM4SlimmingHelper.AllVariables += ExtraContainersTrigger
if globalflags.DataSource()!='geant4':
    EGAM4SlimmingHelper.AllVariables += ExtraContainersTriggerDataOnly

if globalflags.DataSource()=='geant4':
    EGAM4SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM4SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM4_ClusterEnergyPerLayerDecorators:
    EGAM4SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM4SlimmingHelper
EGAM4SlimmingHelper.AppendContentToStream(EGAM4Stream)

# Add AODCellContainer (have to find how to keep only cells belonging to e/gamma objects)
EGAM4Stream.AddItem("CaloCellContainer#AODCellContainer")
