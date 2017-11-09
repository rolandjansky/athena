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

# read common DFEGamma settings from egammaDFFlags
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")

DoCellReweighting = jobproperties.egammaDFFlags.doEGammaCellReweighting
#override if needed (do at your own risk..)
#DoCellReweighting = False
#DoCellReweighting = True

# check if we run on data or MC (DataSource = geant4)
from AthenaCommon.GlobalFlags import globalflags
print "EGAM4 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()!='geant4':
    DoCellReweighting = False


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


#====================================================================
# Cell reweighter
#====================================================================
if DoCellReweighting:
    from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import NewCellTool, ClusterDecoratorWithNewCells, EGammaReweightTool

    # first, create the container with the new cells (after reweighting)
    EGAM4_NewCellTool = NewCellTool("EGAM4_NewCellTool")
    #                                OutputLevel = DEBUG)
    #                                ReweightCellContainerName="NewCellContainer",
    #                                SGKey_electrons = "Electrons", 
    #                                SGKey_photons = "Photons")
    print EGAM4_NewCellTool
    ToolSvc += EGAM4_NewCellTool

    # second, run a tool that creates the clusters and objects from these new cells
    EGAM4_ClusterDecoratorTool = ClusterDecoratorWithNewCells("EGAM4_ClusterDecoratorTool")
    #                                                          OutputClusterSGKey="EGammaSwClusterWithNewCells",
    #                                                          OutputClusterLink="NewSwClusterLink",
    #                                                          SGKey_caloCells = "NewCellContainer",
    #                                                          SGKey_electrons = "Electrons", 
    #                                                          SGKey_photons = "Photons")
    print EGAM4_ClusterDecoratorTool
    ToolSvc += EGAM4_ClusterDecoratorTool

    # third, run a tool that creates the shower shapes with the new cells
    from egammaTools.egammaToolsFactories import EMShowerBuilder
    EGAM4_EMShowerBuilderTool = EMShowerBuilder("EGAM4_EMShowerBuilderTool", 
                                                CellsName="NewCellContainer")
    print EGAM4_EMShowerBuilderTool
    ToolSvc += EGAM4_EMShowerBuilderTool

    # fourth, decorate the new objects with their shower shapes computed from the new clusters
    EGAM4_EGammaReweightTool = EGammaReweightTool("EGAM4_EGammaReweightTool",
                                                  SGKey_electrons = "Electrons",
                                                  SGKey_photons="Photons",
                                                  NewCellContainerName="NewCellContainer",
                                                  NewElectronContainer = "NewSwElectrons",
                                                  NewPhotonContainer = "NewSwPhotons",
                                                  EMShowerBuilderTool = EGAM4_EMShowerBuilderTool,
                                                  ClusterCorrectionToolName = "DFEgammaSWToolWithNewCells",
                                                  CaloClusterLinkName="NewSwClusterLink")
    #                                             OutputLevel=DEBUG)
    print EGAM4_EGammaReweightTool
    ToolSvc += EGAM4_EGammaReweightTool


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
augmentationTools = [EGAM4_MuMuMassTool,EGAM4_GainDecoratorTool]
if DoCellReweighting:
    augmentationTools += [EGAM4_NewCellTool, EGAM4_ClusterDecoratorTool, EGAM4_EGammaReweightTool]
augmentationTools += EGAM4_ClusterEnergyPerLayerDecorators
egam4Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM4Kernel",
                                                         AugmentationTools = augmentationTools,
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

# Append cell-reweighted collections to dictionary
if DoCellReweighting:
    EGAM4SlimmingHelper.AppendToDictionary = {"NewSwPhotons": "xAOD::PhotonContainer", "NewSwPhotonsAux": "xAOD::PhotonAuxContainer" }

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

#Add full CellContainer
EGAM4Stream.AddItem("CaloCellContainer#AODCellContainer")
EGAM4Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
