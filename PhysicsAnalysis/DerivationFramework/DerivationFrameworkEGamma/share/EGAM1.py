#********************************************************************
# EGAM1.py
# Z->ee reduction for central electrons - for electron ID and calibration
# reductionConf flag EGAM1 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM1ExtraContent import *

# read common DFEGamma settings from egammaDFFlags
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")

# this could also go in egammaDFFlags
RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

#OutputLevel = DEBUG

DoCellReweighting = jobproperties.egammaDFFlags.doEGammaCellReweighting
DoCellReweightingVariations = jobproperties.egammaDFFlags.doEGammaCellReweightingVariations
#override if needed (do at your own risk..)
#DoCellReweighting = False
#DoCellReweighting = True
#DoCellReweightingVariations = True


# check if we run on data or MC (DataSource = geant4)
from AthenaCommon.GlobalFlags import globalflags
print "EGAM1 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()!='geant4':
    DoCellReweighting = False
    DoCellReweightingVariations = False


#====================================================================
# SET UP STREAM (to be done early in the game to set up thinning Svc
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM1Stream )
EGAM1Stream = MSMgr.NewPoolRootStream( streamName, fileName )


augmentationTools = []

#====================================================================
# SET UP SKIMMING
#====================================================================

# 1. SELECTION FOR CALIBRATION

#====================================================================
# Z->ee selection based on single e trigger:
# 1 tight e, central, pT>25 GeV
# 1 medium e, pT>20 GeV
# OS, mee>50 GeV
# it would be nice if we could add a tag-bit to flag
# events passing the T&P selection, perhaps using an augmentation
# tool?
#====================================================================

if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
    requirement_probe = '(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.5*GeV)'
else :
    requirement_tag = '(Electrons.LHTight) && (Electrons.pt > 24.5*GeV)'
    requirement_probe = '(Electrons.LHMedium) && (Electrons.pt > 19.5*GeV)'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM1_ZEEMassTool1 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool1",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool1
augmentationTools += [EGAM1_ZEEMassTool1]
print EGAM1_ZEEMassTool1

#====================================================================
# Z->ee selection based on di-electron trigger
# 2 medium e, central, pT>20 GeV
# OS, mee>50 GeV
#====================================================================

if RecomputeElectronSelectors:
    requirement = '(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 19.5*GeV)'
else:
    requirement = '(Electrons.LHMedium) && (Electrons.pt > 19.5*GeV)'
EGAM1_ZEEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool2",
                                                               Object1Requirements = requirement,
                                                               Object2Requirements = requirement,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass2",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = True,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool2
augmentationTools += [EGAM1_ZEEMassTool2]
print EGAM1_ZEEMassTool2


# 2. SELECTION FOR T&P

#====================================================================
# Z->ee selection based on single e trigger, for reco (central) and ID SF(central)
# 1 tight e, central, pT>25 GeV
# 1 e, central, pT>4 GeV
# OS+SS, mee>50 GeV
#====================================================================

if RecomputeElectronSelectors :
    requirement_tag = '(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 24.5*GeV)'
else :
    requirement_tag = '(Electrons.LHMedium) && (Electrons.pt > 24.5*GeV)'
requirement_probe = 'Electrons.pt > 4*GeV'
EGAM1_ZEEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEEMassTool3",
                                                               Object1Requirements = requirement_tag,
                                                               Object2Requirements = requirement_probe,
                                                               StoreGateEntryName = "EGAM1_DiElectronMass3",
                                                               Mass1Hypothesis = 0.511*MeV,
                                                               Mass2Hypothesis = 0.511*MeV,
                                                               Container1Name = "Electrons",
                                                               Container2Name = "Electrons",
                                                               CheckCharge = False,
                                                               DoTransverseMass = False,
                                                               MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEEMassTool3
augmentationTools += [EGAM1_ZEEMassTool3]
print EGAM1_ZEEMassTool3



#====================================================================
# Z->eg selection based on single e trigger, for reco SF (central)
# 1 tight e, central, pT>25 GeV
# 1 gamma, pT>15 GeV, central
# OS+SS, mee>50 GeV
#====================================================================

if RecomputeElectronSelectors:
#    use medium for early data upon electron group request
    requirement_tag = '(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 24.5*GeV)'
else:
    requirement_tag = '(Electrons.LHMedium) && (Electrons.pt > 24.5*GeV)'
requirement_probe = 'DFCommonPhotons_et > 14.5*GeV'
EGAM1_ZEGMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM1_ZEGMassTool",
                                                              Object1Requirements = requirement_tag,
                                                              Object2Requirements = requirement_probe,
                                                              StoreGateEntryName = "EGAM1_ElectronPhotonMass",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "Photons",
                                                              Pt2BranchName = "DFCommonPhotons_pt",
                                                              Eta2BranchName = "DFCommonPhotons_eta",
                                                              Phi2BranchName = "DFCommonPhotons_phi",
                                                              CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)
ToolSvc += EGAM1_ZEGMassTool
augmentationTools += [EGAM1_ZEGMassTool]
print EGAM1_ZEGMassTool


# Skimming criteria
expression = 'count(EGAM1_DiElectronMass > 50.0*GeV)>=1 || count(EGAM1_DiElectronMass2 > 50.0*GeV)>=1 || count(EGAM1_DiElectronMass3 > 50.0*GeV)>=1 ||  count (EGAM1_ElectronPhotonMass > 50.0*GeV)>=1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM1SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM1SkimmingTool
print "EGAM1 skimming tool: ", EGAM1SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM1_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM1_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons" )
ToolSvc += EGAM1_MaxCellDecoratorTool
augmentationTools += [EGAM1_MaxCellDecoratorTool]


#====================================================================
# Cell reweighter
#====================================================================
if DoCellReweighting:
    from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import NewCellTool, ClusterDecoratorWithNewCells, EGammaReweightTool

    # first, create the container with the new cells (after reweighting)
    EGAM1_NewCellTool = NewCellTool("EGAM1_NewCellTool",
                                    #OutputLevel = DEBUG,
                                    CellContainerName = "AODCellContainer",
                                    ReweightCellContainerName = "NewCellContainer",
                                    SGKey_electrons = "Electrons",
                                    SGKey_photons = "Photons",
                                    ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs10.root" )

    print EGAM1_NewCellTool
    ToolSvc += EGAM1_NewCellTool
    augmentationTools += [EGAM1_NewCellTool]

    # second, run a tool that creates the clusters and objects from these new cells
    EGAM1_ClusterDecoratorTool = ClusterDecoratorWithNewCells("EGAM1_ClusterDecoratorTool",
                                                              #OutputLevel=DEBUG,
                                                              OutputClusterSGKey = "EGammaSwClusterWithNewCells",
                                                              OutputClusterLink = "NewSwClusterLink",
                                                              SGKey_caloCells = "NewCellContainer",
                                                              SGKey_electrons = "Electrons",
                                                              SGKey_photons = "Photons")
    print EGAM1_ClusterDecoratorTool
    ToolSvc += EGAM1_ClusterDecoratorTool
    augmentationTools += [EGAM1_ClusterDecoratorTool]

    
    # third, schedule a tool that will be invoked by the EGammaReweightTool to create on-the-fly the shower shapes with the new cells
    from egammaTools.egammaToolsFactories import EMShowerBuilder
    EGAM1_EMShowerBuilderTool = EMShowerBuilder("EGAM1_EMShowerBuilderTool", 
                                                CellsName="NewCellContainer" )

    print EGAM1_EMShowerBuilderTool
    ToolSvc += EGAM1_EMShowerBuilderTool
    
    # fourth, decorate the new objects with their shower shapes computed from the new clusters
    EGAM1_EGammaReweightTool = EGammaReweightTool("EGAM1_EGammaReweightTool",
                                                  #OutputLevel=DEBUG,
                                                  SGKey_electrons = "Electrons",
                                                  SGKey_photons = "Photons",
                                                  NewCellContainerName = "NewCellContainer",
                                                  NewElectronContainer = "NewSwElectrons",
                                                  NewPhotonContainer = "NewSwPhotons",
                                                  EMShowerBuilderTool = EGAM1_EMShowerBuilderTool,
                                                  ClusterCorrectionToolName = "DFEgammaSWToolWithNewCells",
                                                  CaloClusterLinkName = "NewSwClusterLink",
                                                  DecorateEGammaObjects = False,
                                                  DecorationPrefix = "RW_",
                                                  SaveReweightedContainer = True)

    print EGAM1_EGammaReweightTool
    ToolSvc += EGAM1_EGammaReweightTool
    augmentationTools += [EGAM1_EGammaReweightTool]

    if DoCellReweightingVariations:
        
        ###########################################  REWEIGHTING VARIATIONS - MAX ######################################################
      
        from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import MaxVarCellTool, ClusterDecoratorWithMaxVarCells, EGammaMaxVarReweightTool
        
        # first, create the container with the new cells (after reweighting)      
        EGAM1_MaxVarCellTool = MaxVarCellTool ("EGAM1_MaxVarCellTool",
                                               #OutputLevel = DEBUG,
                                               CellContainerName="AODCellContainer",
                                               ReweightCellContainerName="MaxVarCellContainer",
                                               SGKey_electrons = "Electrons",
                                               SGKey_photons = "Photons", 
                                               ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs11.root")
        
        print EGAM1_MaxVarCellTool
        ToolSvc += EGAM1_MaxVarCellTool

        # second, run a tool that creates the clusters and objects from these new cells
        EGAM1_MaxVarClusterDecoratorTool = ClusterDecoratorWithMaxVarCells("EGAM1_MaxVarClusterDecoratorTool",
                                                                           OutputClusterSGKey="EGammaSwClusterWithMaxVarCells",
                                                                           OutputClusterLink="MaxVarSwClusterLink",
                                                                           SGKey_caloCells = "MaxVarCellContainer", 
                                                                           SGKey_electrons = "Electrons",
                                                                           SGKey_photons = "Photons")
        print EGAM1_MaxVarClusterDecoratorTool
        ToolSvc += EGAM1_MaxVarClusterDecoratorTool

        # third, schedule a tool that will be invoked by the EGammaReweightTool to create on-the-fly the shower shapes with the new cells
        EGAM1_EMMaxVarShowerBuilderTool = EMShowerBuilder("EGAM1_EMMaxVarShowerBuilderTool", 
                                                          CellsName="MaxVarCellContainer")
        print EGAM1_EMMaxVarShowerBuilderTool
        ToolSvc += EGAM1_EMMaxVarShowerBuilderTool
        
        # fourth, decorate the new objects with their shower shapes computed from the new clusters
        EGAM1_EGammaMaxVarReweightTool = EGammaReweightTool("EGAM1_EGammaMaxVarReweightTool",
                                                            #OutputLevel = DEBUG,
                                                            SGKey_electrons = "Electrons",
                                                            SGKey_photons="Photons",
                                                            NewCellContainerName="MaxVarCellContainer",
                                                            NewElectronContainer = "MaxVarSwElectrons",
                                                            NewPhotonContainer = "MaxVarSwPhotons",
                                                            EMShowerBuilderTool = EGAM1_EMMaxVarShowerBuilderTool,
                                                            ClusterCorrectionToolName = "DFEgammaSWToolWithMaxVarCells",
                                                            CaloClusterLinkName="MaxVarSwClusterLink",
                                                            DecorateEGammaObjects = False,
                                                            DecorationPrefix = "Max_",
                                                            SaveReweightedContainer = True)
        print EGAM1_EGammaMaxVarReweightTool
        ToolSvc += EGAM1_EGammaMaxVarReweightTool


        ###########################################  REWEIGHTING VARIATIONS - MIN ######################################################
        
        from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import MinVarCellTool, ClusterDecoratorWithMinVarCells, EGammaMinVarReweightTool

        # first, create the container with the new cells (after reweighting)            
        EGAM1_MinVarCellTool = MinVarCellTool ("EGAM1_MinVarCellTol",
                                               #OutputLevel = DEBUG,
                                               CellContainerName="AODCellContainer",
                                               ReweightCellContainerName="MinVarCellContainer",
                                               SGKey_electrons = "Electrons",
                                               SGKey_photons = "Photons",
                                               ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs00.root")

      
        print EGAM1_MinVarCellTool
        ToolSvc += EGAM1_MinVarCellTool

        # second, run a tool that creates the clusters and objects from these new cells
        EGAM1_MinVarClusterDecoratorTool = ClusterDecoratorWithMinVarCells("EGAM1_MinVarClusterDecoratorTool",
                                                                           OutputClusterSGKey="EGammaSwClusterWithMinVarCells",
                                                                           OutputClusterLink="MinVarSwClusterLink",
                                                                           SGKey_caloCells = "MinVarCellContainer", 
                                                                           SGKey_electrons = "Electrons",
                                                                           SGKey_photons = "Photons")
        print EGAM1_MinVarClusterDecoratorTool
        ToolSvc += EGAM1_MinVarClusterDecoratorTool

        # third, schedule a tool that will be invoked by the EGammaReweightTool to create on-the-fly the shower shapes with the new cells      
        EGAM1_EMMinVarShowerBuilderTool = EMShowerBuilder("EGAM1_EMMinVarShowerBuilderTool", 
                                                          CellsName="MinVarCellContainer")
        print EGAM1_EMMinVarShowerBuilderTool
        ToolSvc += EGAM1_EMMinVarShowerBuilderTool

        # fourth, decorate the new objects with their shower shapes computed from the new clusters
        EGAM1_EGammaMinVarReweightTool = EGammaReweightTool("EGAM1_EGammaMinVarReweightTool",
                                                            #OutputLevel = DEBUG,
                                                            SGKey_electrons = "Electrons",
                                                            SGKey_photons="Photons",
                                                            NewCellContainerName="MinVarCellContainer",
                                                            NewElectronContainer = "MinVarSwElectrons",
                                                            NewPhotonContainer = "MinVarSwPhotons",
                                                            EMShowerBuilderTool = EGAM1_EMMinVarShowerBuilderTool,
                                                            ClusterCorrectionToolName = "DFEgammaSWToolWithMinVarCells",
                                                            CaloClusterLinkName="MinVarSwClusterLink",
                                                            DecorateEGammaObjects = False,
                                                            DecorationPrefix = "Min_",
                                                            SaveReweightedContainer = True)

        print EGAM1_EGammaMinVarReweightTool
        ToolSvc += EGAM1_EGammaMinVarReweightTool

        augmentationTools += [EGAM1_MaxVarCellTool, EGAM1_MaxVarClusterDecoratorTool, EGAM1_EGammaMaxVarReweightTool, EGAM1_MinVarCellTool, EGAM1_MinVarClusterDecoratorTool, EGAM1_EGammaMinVarReweightTool]
        

#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
# GM: do we really need new, different tools: getClusterEnergyPerLayerDecoratorNew, getClusterEnergyPerLayerDecoratorMaxVar, getClusterEnergyPerLayerDecoratorMinVar?
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations, getClusterEnergyPerLayerDecoratorNew, getClusterEnergyPerLayerDecoratorMaxVar, getClusterEnergyPerLayerDecoratorMinVar
EGAM1_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM1_GainDecoratorTool
augmentationTools += [EGAM1_GainDecoratorTool]

cluster_sizes = (3,7), (5,5), (7,11)
EGAM1_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
if DoCellReweighting:
    EGAM1_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorNew(neta, nphi)() for neta, nphi in cluster_sizes]
    if DoCellReweightingVariations:
        EGAM1_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorMaxVar(neta, nphi)() for neta, nphi in cluster_sizes]
        EGAM1_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorMinVar(neta, nphi)() for neta, nphi in cluster_sizes]
augmentationTools += EGAM1_ClusterEnergyPerLayerDecorators


#====================================================================
# SET UP THINNING
#====================================================================
        
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EGAM1ThinningHelper = ThinningHelper( "EGAM1ThinningHelper" )
EGAM1ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))|HLT_e.*_Zee.*'
EGAM1ThinningHelper.AppendToStream( EGAM1Stream, ExtraContainersTrigger )


thinningTools=[]

if jobproperties.egammaDFFlags.doEGammaDAODTrackThinning:

    TrackThinningKeepElectronTracks = True
    TrackThinningKeepPhotonTracks = True
    TrackThinningKeepAllElectronTracks = True
    TrackThinningKeepJetTracks = False
    TrackThinningKeepMuonTracks = False
    TrackThinningKeepTauTracks = False
    TrackThinningKeepPVTracks = True

    # Tracks associated with Electrons
    if (TrackThinningKeepElectronTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM1ElectronTPThinningTool",
                                                                                        StreamName              = streamName,
                                                                                        SGKey                   = "Electrons",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 0*GeV",
                                                                                        BestMatchOnly = True,
                                                                                        ConeSize = 0.3)
        ToolSvc += EGAM1ElectronTPThinningTool
        print EGAM1ElectronTPThinningTool
        thinningTools.append(EGAM1ElectronTPThinningTool)

    # Tracks associated with Electrons (all tracks, large cone, for track isolation studies of the selected electrons)
    if (TrackThinningKeepAllElectronTracks) :
        EGAM1ElectronTPThinningTool2 = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM1ElectronTPThinningTool2",
                                                                                         StreamName              = streamName,
                                                                                         SGKey                   = "Electrons",
                                                                                         GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                         SelectionString         = "Electrons.pt > 4*GeV",
                                                                                         BestMatchOnly = False,
                                                                                         ConeSize = 0.6)

        ToolSvc += EGAM1ElectronTPThinningTool2
        print EGAM1ElectronTPThinningTool2
        thinningTools.append(EGAM1ElectronTPThinningTool2)

    # Tracks associated with Photons
    if (TrackThinningKeepPhotonTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM1PhotonTPThinningTool",
                                                                                      StreamName              = streamName,
                                                                                      SGKey                   = "Photons",
                                                                                      GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      SelectionString         = "Photons.pt > 0*GeV",
                                                                                      BestMatchOnly = True,
                                                                                      ConeSize = 0.3)
        ToolSvc += EGAM1PhotonTPThinningTool
        print EGAM1PhotonTPThinningTool
        thinningTools.append(EGAM1PhotonTPThinningTool)

    # Tracks associated with Jets
    if (TrackThinningKeepJetTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
        EGAM1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM1JetTPThinningTool",
                                                                                StreamName              = streamName,
                                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM1JetTPThinningTool
        print EGAM1JetTPThinningTool
        thinningTools.append(EGAM1JetTPThinningTool)


    # Tracks associated with Muons
    if (TrackThinningKeepMuonTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
        EGAM1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM1MuonTPThinningTool",
                                                                                  StreamName              = streamName,
                                                                                  MuonKey                 = "Muons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM1MuonTPThinningTool
        print EGAM1MuonTPThinningTool
        thinningTools.append(EGAM1MuonTPThinningTool)
        

    # Tracks associated with Taus
    if (TrackThinningKeepTauTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
        EGAM1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM1TauTPThinningTool",
                                                                                StreamName              = streamName,
                                                                                TauKey                  = "TauJets",
                                                                                ConeSize                = 0.6,
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM1TauTPThinningTool
        print EGAM1TauTPThinningTool
        thinningTools.append(EGAM1TauTPThinningTool)

    # Tracks from primary vertex
    if (TrackThinningKeepPVTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
        EGAM1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM1TPThinningTool",
                                                                          StreamName              = streamName,
                                                                          SelectionString         = "InDetTrackParticles.DFCommonTightPrimary && abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0*mm",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM1TPThinningTool
        print EGAM1TPThinningTool
        thinningTools.append(EGAM1TPThinningTool)


# keep topoclusters around electrons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EGAM1CCTCThinningTool = DerivationFramework__CaloClusterThinning(name                    = "EGAM1CCTCThinningTool",
                                                                 StreamName              = streamName,
                                                                 SGKey                   = "Electrons",
                                                                 SelectionString         = "Electrons.pt>4*GeV",
                                                                 TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                 ConeSize                = 0.5)
ToolSvc += EGAM1CCTCThinningTool
print EGAM1CCTCThinningTool
thinningTools.append(EGAM1CCTCThinningTool)


# Truth thinning
if globalflags.DataSource()=='geant4':
    truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
    truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
    truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    EGAM1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM1TruthThinningTool",
                                                                       StreamName              = streamName,
                                                                       ParticleSelectionString = truth_expression,
                                                                       PreserveDescendants     = False,
                                                                       PreserveGeneratorDescendants     = True,
                                                                       PreserveAncestors      = True)
    
    ToolSvc += EGAM1TruthThinningTool
    thinningTools.append(EGAM1TruthThinningTool)

print "EGAM1 thinningTools: ", thinningTools



#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam1Seq = CfgMgr.AthSequencer("EGAM1Sequence")
DerivationFrameworkJob += egam1Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

print "EGAM1 skimming tools", [EGAM1SkimmingTool]
print "EGAM1 thinning tools", thinningTools
print "EGAM1 augmentation tools", augmentationTools
egam1Seq += CfgMgr.DerivationFramework__DerivationKernel("EGAM1Kernel",
                                                         AugmentationTools = augmentationTools,
                                                         SkimmingTools = [EGAM1SkimmingTool],
                                                         ThinningTools = thinningTools
                                                         )

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
#reducedJetList = ["AntiKt4PV0TrackJets", "AntiKt4TruthJets"]
reducedJetList = ["AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,egam1Seq,"EGAM1")


#=======================================
# ADD NON-PROMPT LEPTON VETO ALGORITHMS 
#=======================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
JetTagConfig.ConfigureAntiKt4PV0TrackJets(egam1Seq, "EGAM1")
egam1Seq += JetTagConfig.GetDecoratePromptLeptonAlgs(name="Electrons")


#====================================================================
# SET UP STREAM SELECTION
#====================================================================
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM1Stream.AcceptAlgs(["EGAM1Kernel"])



#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM1SlimmingHelper = SlimmingHelper("EGAM1SlimmingHelper")
EGAM1SlimmingHelper.SmartCollections = ["Electrons",
					"Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM1SlimmingHelper.IncludeEGammaTriggerContent = True

# Append cell-reweighted collections to dictionary
if DoCellReweighting:
    EGAM1SlimmingHelper.AppendToDictionary = {"NewSwElectrons": "xAOD::ElectronContainer", "NewSwElectronsAux": "xAOD::ElectronAuxContainer"}
    if DoCellReweightingVariations:
        EGAM1SlimmingHelper.AppendToDictionary.update({ "MaxVarSwElectrons": "xAOD::ElectronContainer", "MaxVarSwElectronsAux": "xAOD::ElectronAuxContainer", "MinVarSwElectrons": "xAOD::ElectronContainer", "MinVarSwElectronsAux": "xAOD::ElectronAuxContainer" })
        
# Extra variables
EGAM1SlimmingHelper.ExtraVariables = ExtraContentAll
# the next line is not needed because we save all variables for electrons, including the prompt lepton decorations
# EGAM1SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
EGAM1SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM1SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM1SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM1SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM1SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM1_ClusterEnergyPerLayerDecorators:
    EGAM1SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# Add energy density variables
EGAM1SlimmingHelper.ExtraVariables += ExtraVariablesEventShape

# Add detailed shower shape variables (not needed for electrons because we save everything but it doesn't hurt..)
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
EGAM1SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EGAM1SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
EGAM1SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# This line must come after we have finished configuring EGAM1SlimmingHelper
EGAM1SlimmingHelper.AppendContentToStream(EGAM1Stream)

#Add full CellContainer
EGAM1Stream.AddItem("CaloCellContainer#AODCellContainer")
EGAM1Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")




