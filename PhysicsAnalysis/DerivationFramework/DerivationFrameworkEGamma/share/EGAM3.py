#********************************************************************
# EGAM3.py
# Z->eegamma reduction for low-pT electron and photon studies
# reductionConf flag EGAM3 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM3ExtraContent import *

# read common DFEGamma settings from egammaDFFlags
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")

# this could also go in egammaDFFlags
RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

DoCellReweighting = jobproperties.egammaDFFlags.doEGammaCellReweighting
#override if needed (do at your own risk..)
#DoCellReweighting = False
#DoCellReweighting = True

# check if we run on data or MC (DataSource = geant4)
from AthenaCommon.GlobalFlags import globalflags
print "EGAM3 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()!='geant4':
    DoCellReweighting = False


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM3Stream )
EGAM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )


#====================================================================
# SET UP SKIMMING
#====================================================================


#====================================================================
# eegamma selection for photon studies, di-electron triggers
# two opposite-sign medium el, pT>10 GeV, |eta|<2.5, mee>40 GeV
# gamma: reco, ET>10 GeV< |eta|<2.5
#====================================================================

# if skim size too large either require tight electrons (at least one) or raise electron pT threshold (at least one)
#requirement = 'DFCommonElectronsLHMedium && (DFCommonElectrons_pt > 9.5*GeV)'
if RecomputeElectronSelectors :
    requirement = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 9.5*GeV)'
else :
    requirement = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 9.5*GeV)'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM3_EEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool",
                                                             Object1Requirements = requirement,
                                                             Object2Requirements = requirement,
                                                             StoreGateEntryName = "EGAM3_DiElectronMass",
                                                             Mass1Hypothesis = 0.511*MeV,
                                                             Mass2Hypothesis = 0.511*MeV,
                                                             Container1Name = "Electrons",
                                                             Container2Name = "Electrons",
                                                             CheckCharge = True,
                                                             DoTransverseMass = False,
                                                             MinDeltaR = 0.0)
ToolSvc += EGAM3_EEMassTool
print EGAM3_EEMassTool


#====================================================================
# eegamma selection for low-pT electron studies with T&P
# tag e: tight, |eta|<2.5, pT>25 GeV
# probe e: reco, ET>7 GeV, no eta cut 
# gamma: tight, ET>10 GeV
#====================================================================
# asymmetric electron cuts/single e trigger, low pT cut for subleading e (for e calibration studies at low pT)
#requirement1 = 'DFCommonElectronsLHTight && (DFCommonElectrons_pt > 24.5*GeV)'
if RecomputeElectronSelectors :
    requirement1 = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirement1 = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
requirement2 = '(Electrons.pt > 6.5*GeV)'

EGAM3_EEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool2",
                                                              Object1Requirements = requirement1,
                                                              Object2Requirements = requirement2,
                                                              StoreGateEntryName = "EGAM3_DiElectronMass2",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "Electrons",
                                                              CheckCharge = True,
                                                              #CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)

ToolSvc += EGAM3_EEMassTool2
print EGAM3_EEMassTool2

if RecomputeElectronSelectors :
    requirement1 = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirement1 = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
requirement2 = '(ForwardElectrons.pt > 6.5*GeV)'
#requirement2 = '(ForwardElectrons.pt > 9.5*GeV)'

EGAM3_EEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool3",
                                                              Object1Requirements = requirement1,
                                                              Object2Requirements = requirement2,
                                                              StoreGateEntryName = "EGAM3_DiElectronMass3",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "ForwardElectrons",
                                                              CheckCharge = True,
                                                              #CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)
ToolSvc += EGAM3_EEMassTool3
print EGAM3_EEMassTool3



#====================================================================
# SKIMMING TOOL
#====================================================================

if RecomputeElectronSelectors :
    photon_quality = 'Photons.DFCommonPhotonsIsEMTight'
else :
    photon_quality = 'Photons.Tight'

expression = '(count(DFCommonPhotons_et>9.5*GeV)>=1 && count(EGAM3_DiElectronMass > 40.0*GeV)>=1) || (count(DFCommonPhotons_et>9.5*GeV && '+ photon_quality + ')>=1 && (count(EGAM3_DiElectronMass2 > 40.0*GeV)>=1 || count(EGAM3_DiElectronMass3 > 40.0*GeV)>=1))'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM3_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM3_SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM3_SkimmingTool
print "EGAM3 skimming tool:", EGAM3_SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM3_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM3_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM3_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]

#====================================================================
# Cell reweighter
#====================================================================
if DoCellReweighting:
    from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import NewCellTool, ClusterDecoratorWithNewCells, EGammaReweightTool

    # first, create the container with the new cells (after reweighting)
    EGAM3_NewCellTool = NewCellTool("EGAM3_NewCellTool")
    #                                OutputLevel = DEBUG)
    #                                ReweightCellContainerName="NewCellContainer",
    #                                SGKey_electrons = "Electrons", 
    #                                SGKey_photons = "Photons")
    print EGAM3_NewCellTool
    ToolSvc += EGAM3_NewCellTool

    # second, run a tool that creates the clusters and objects from these new cells
    EGAM3_ClusterDecoratorTool = ClusterDecoratorWithNewCells("EGAM3_ClusterDecoratorTool")
    #                                                          OutputClusterSGKey="EGammaSwClusterWithNewCells",
    #                                                          OutputClusterLink="NewSwClusterLink",
    #                                                          SGKey_caloCells = "NewCellContainer",
    #                                                          SGKey_electrons = "Electrons",
    #                                                          SGKey_photons = "Photons")
    print EGAM3_ClusterDecoratorTool
    ToolSvc += EGAM3_ClusterDecoratorTool

    # third, run a tool that creates the shower shapes with the new cells
    from egammaTools.egammaToolsFactories import EMShowerBuilder
    EGAM3_EMShowerBuilderTool = EMShowerBuilder("EGAM3_EMShowerBuilderTool", 
                                                CellsName="NewCellContainer")
    print EGAM3_EMShowerBuilderTool
    ToolSvc += EGAM3_EMShowerBuilderTool

    # fourth, decorate the new objects with their shower shapes computed from the new clusters
    EGAM3_EGammaReweightTool = EGammaReweightTool("EGAM3_EGammaReweightTool",
                                                  SGKey_electrons = "Electrons",
                                                  SGKey_photons="Photons",
                                                  NewCellContainerName="NewCellContainer",
                                                  NewElectronContainer = "NewSwElectrons",
                                                  NewPhotonContainer = "NewSwPhotons",
                                                  EMShowerBuilderTool = EGAM3_EMShowerBuilderTool,
                                                  ClusterCorrectionToolName = "DFEgammaSWToolWithNewCells",
                                                  CaloClusterLinkName="NewSwClusterLink")
    #                                             OutputLevel=DEBUG)
    print EGAM3_EGammaReweightTool
    ToolSvc += EGAM3_EGammaReweightTool


#====================================================================
# SET UP THINNING
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM3ThinningHelper = ThinningHelper( "EGAM3ThinningHelper" )
EGAM3ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))'
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly
EGAM3ThinningHelper.AppendToStream( EGAM3Stream, ExtraContainersTrigger )

thinningTools=[]
# TO BE ADDED


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam3Seq = CfgMgr.AthSequencer("EGAM3Sequence")
DerivationFrameworkJob += egam3Seq


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
augmentationTools = [EGAM3_EEMassTool,EGAM3_EEMassTool2,EGAM3_EEMassTool3,EGAM3_GainDecoratorTool]
if DoCellReweighting:
    augmentationTools += [EGAM3_NewCellTool, EGAM3_ClusterDecoratorTool, EGAM3_EGammaReweightTool]
augmentationTools += EGAM3_ClusterEnergyPerLayerDecorators
print "EGAM3 augmentationTools", augmentationTools
egam3Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM3Kernel",
                                                         AugmentationTools = augmentationTools,
                                                         SkimmingTools = [EGAM3_SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam3Seq,"EGAM3")


#====================================================================
# SET UP STREAM SELECTION
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM3Stream.AcceptAlgs(["EGAM3Kernel"])


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

EGAM3SlimmingHelper = SlimmingHelper("EGAM3SlimmingHelper")
EGAM3SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM3SlimmingHelper.IncludeEGammaTriggerContent = True

# Append cell-reweighted collections to dictionary
if DoCellReweighting:
    EGAM3SlimmingHelper.AppendToDictionary = {"NewSwPhotons": "xAOD::PhotonContainer", "NewSwPhotonsAux": "xAOD::PhotonAuxContainer", "NewSwElectrons": "xAOD::ElectronContainer", "NewSwElectronsAux": "xAOD::ElectronAuxContainer" }


# Extra variables
EGAM3SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM3SlimmingHelper.AllVariables = ExtraContainersPhotons

if globalflags.DataSource()=='geant4':
    EGAM3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM3SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM3_ClusterEnergyPerLayerDecorators:
    EGAM3SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM3SlimmingHelper
EGAM3SlimmingHelper.AppendContentToStream(EGAM3Stream)

#Add full CellContainer
EGAM3Stream.AddItem("CaloCellContainer#AODCellContainer")
EGAM3Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")

