
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
RecomputeEGammaSelectors = True
#RecomputeEGammaSelectors = False

DoCellReweighting = jobproperties.egammaDFFlags.doEGammaCellReweighting
DoCellReweightingVariations = jobproperties.egammaDFFlags.doEGammaCellReweightingVariations
#override if needed (do at your own risk..)
#DoCellReweighting = False
#DoCellReweighting = True

# check if we run on data or MC (DataSource = geant4)
from AthenaCommon.GlobalFlags import globalflags
print "EGAM3 globalflags.DataSource(): ", globalflags.DataSource()
if globalflags.DataSource()!='geant4':
    DoCellReweighting = False
    DoCellReweightingVariations = False


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
# eegamma and eee selection for photon efficiency studies, di-electron triggers
# two opposite-sign medium el, pT>10 GeV, |eta|<2.5, mee>40 GeV
# eegamma: one reco photon, ET>10 GeV< |eta|<2.5
# eee: 3 electrons, pT>10 GeV, mee>40 GeV
#====================================================================

# if skim size too large either require tight electrons (at least one) or raise electron pT threshold (at least one)
if RecomputeEGammaSelectors :
    requirementElectrons = '(Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 9.5*GeV)'
else :
    requirementElectrons = '(Electrons.LHMedium) && (Electrons.pt > 9.5*GeV)'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM3_EEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool",
                                                             Object1Requirements = requirementElectrons,
                                                             Object2Requirements = requirementElectrons,
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
skimmingExpression1a = '(count(DFCommonPhotons_et>9.5*GeV)>=1 && count(EGAM3_DiElectronMass > 40.0*GeV)>=1)'
skimmingExpression1b = '(count(Electrons.pt>9.5*GeV)>=3 && count(EGAM3_DiElectronMass > 40.0*GeV)>=1)'

#====================================================================
# eegamma selection for low-pT electron studies with T&P
# tag e: tight, |eta|<2.5, pT>25 GeV
# probe e: reco, ET>7 GeV, central electron
# gamma: tight, ET>10 GeV
#====================================================================
# asymmetric electron cuts/single e trigger, low pT cut for subleading e (for e calibration studies at low pT)
if RecomputeEGammaSelectors :
    requirementElectron1 = '(Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirementElectron1 = '(Electrons.LHTight) && (Electrons.pt > 24.5*GeV)'
requirementElectron2 = '(Electrons.pt > 6.5*GeV)'
if RecomputeEGammaSelectors :
    requirementPhoton = 'Photons.DFCommonPhotonsIsEMTight'
else :
    requirementPhoton = 'Photons.Tight'


EGAM3_EEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool2",
                                                              Object1Requirements = requirementElectron1,
                                                              Object2Requirements = requirementElectron2,
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
skimmingExpression2 = '(count(DFCommonPhotons_et>9.5*GeV && '+ requirementPhoton + ')>=1 && count(EGAM3_DiElectronMass2 > 40.0*GeV)>=1)'


#====================================================================
# eegamma selection for low-pT electron studies with T&P
# tag e: tight, |eta|<2.5, pT>25 GeV
# probe e: reco, ET>7 GeV, forward electron
# gamma: tight, ET>10 GeV
#====================================================================

if RecomputeEGammaSelectors :
    requirementElectron1 = '(Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirementElectron1 = '(Electrons.LHTight) && (Electrons.pt > 24.5*GeV)'
requirementElectron2 = '(ForwardElectrons.pt > 6.5*GeV)'

EGAM3_EEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool3",
                                                              Object1Requirements = requirementElectron1,
                                                              Object2Requirements = requirementElectron2,
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
skimmingExpression3 = '(count(DFCommonPhotons_et>9.5*GeV && '+ requirementPhoton + ')>=1 && count(EGAM3_DiElectronMass3 > 40.0*GeV)>=1)'


#====================================================================
# SKIMMING TOOL
#====================================================================

skimmingExpression = skimmingExpression1a + ' || ' + skimmingExpression1b + ' || ' + skimmingExpression2 + ' || ' + skimmingExpression3
print "EGAM3 skimming expression: ", skimmingExpression


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM3_SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM3_SkimmingTool",
                                                                 expression = skimmingExpression)
ToolSvc += EGAM3_SkimmingTool
print "EGAM3 skimming tool: ", EGAM3_SkimmingTool



#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations, getClusterEnergyPerLayerDecoratorNew, getClusterEnergyPerLayerDecoratorMaxVar, getClusterEnergyPerLayerDecoratorMinVar
EGAM3_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM3_GainDecoratorTool

cluster_sizes = (3,7), (5,5), (7,11)
EGAM3_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
if DoCellReweighting:
    EGAM3_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorNew(neta, nphi)() for neta, nphi in cluster_sizes]
    if DoCellReweightingVariations:
        EGAM3_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorMaxVar(neta, nphi)() for neta, nphi in cluster_sizes]
        EGAM3_ClusterEnergyPerLayerDecorators += [getClusterEnergyPerLayerDecoratorMinVar(neta, nphi)() for neta, nphi in cluster_sizes]


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM3_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM3_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons" )
ToolSvc += EGAM3_MaxCellDecoratorTool


#====================================================================
# TopoCluster moments
#====================================================================
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGTopoClusterMomentDecorator
EGAM3_PhotonTopoClusterMomentDecorator = DerivationFramework__EGTopoClusterMomentDecorator(
	name = "EGAM3_PhotonTopoClusterMomentDecorator",
	ContainerName = "Photons")
print EGAM3_PhotonTopoClusterMomentDecorator
ToolSvc += EGAM3_PhotonTopoClusterMomentDecorator


#====================================================================
# Cell reweighter
#====================================================================
if DoCellReweighting:
    from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import NewCellTool, ClusterDecoratorWithNewCells, EGammaReweightTool

    # first, create the container with the new cells (after reweighting)
    EGAM3_NewCellTool = NewCellTool("EGAM3_NewCellTool",
                                    #OutputLevel = DEBUG
                                    CellContainerName = "AODCellContainer",
                                    ReweightCellContainerName = "NewCellContainer",
                                    SGKey_electrons = "Electrons",
                                    SGKey_photons = "Photons",
                                    ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs10.root" )
    print EGAM3_NewCellTool
    ToolSvc += EGAM3_NewCellTool

    # second, run a tool that creates the clusters and objects from these new cells
    EGAM3_ClusterDecoratorTool = ClusterDecoratorWithNewCells("EGAM3_ClusterDecoratorTool",
                                                              #OutputLevel=DEBUG,
                                                              OutputClusterSGKey = "EGammaSwClusterWithNewCells",
                                                              OutputClusterLink = "NewSwClusterLink",
                                                              SGKey_caloCells = "NewCellContainer",
                                                              SGKey_electrons = "Electrons",
                                                              SGKey_photons = "Photons")
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
                                                  #OutputLevel=DEBUG,
                                                  SGKey_electrons = "Electrons",
                                                  SGKey_photons="Photons",
                                                  NewCellContainerName="NewCellContainer",
                                                  NewElectronContainer = "NewSwElectrons",
                                                  #NewElectronContainer = "", # no container for electrons
                                                  NewPhotonContainer = "NewSwPhotons",
                                                  EMShowerBuilderTool = EGAM3_EMShowerBuilderTool,
                                                  ClusterCorrectionToolName = "DFEgammaSWToolWithNewCells",
                                                  CaloClusterLinkName="NewSwClusterLink",
                                                  DecorateEGammaObjects = False,
                                                  DecorationPrefix = "RW_",
                                                  SaveReweightedContainer = True)

    print EGAM3_EGammaReweightTool
    ToolSvc += EGAM3_EGammaReweightTool

    if DoCellReweightingVariations:
        
        ###########################################  REWEIGHTING VARIATIONS - MAX ######################################################
      
        from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import MaxVarCellTool, ClusterDecoratorWithMaxVarCells, EGammaMaxVarReweightTool
        
        # first, create the container with the new cells (after reweighting)      
        EGAM3_MaxVarCellTool = MaxVarCellTool ("EGAM3_MaxVarCellTool",
                                               #OutputLevel = DEBUG,
                                               CellContainerName="AODCellContainer",
                                               ReweightCellContainerName="MaxVarCellContainer",
                                               SGKey_electrons = "Electrons",
                                               SGKey_photons = "Photons", 
                                               ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs11.root")
        
        print EGAM3_MaxVarCellTool
        ToolSvc += EGAM3_MaxVarCellTool
        
        # second, run a tool that creates the clusters and objects from these new cells
        EGAM3_MaxVarClusterDecoratorTool = ClusterDecoratorWithMaxVarCells("EGAM3_MaxVarClusterDecoratorTool",
                                                                           OutputClusterSGKey="EGammaSwClusterWithMaxVarCells",
                                                                           OutputClusterLink="MaxVarSwClusterLink",
                                                                           SGKey_caloCells = "MaxVarCellContainer", 
                                                                           SGKey_electrons = "Electrons",
                                                                           SGKey_photons = "Photons")
        print EGAM3_MaxVarClusterDecoratorTool
        ToolSvc += EGAM3_MaxVarClusterDecoratorTool

        # third, schedule a tool that will be invoked by the EGammaReweightTool to create on-the-fly the shower shapes with the new cells
        EGAM3_EMMaxVarShowerBuilderTool = EMShowerBuilder("EGAM3_EMMaxVarShowerBuilderTool", 
                                                          CellsName="MaxVarCellContainer")
        print EGAM3_EMMaxVarShowerBuilderTool
        ToolSvc += EGAM3_EMMaxVarShowerBuilderTool
        
        # fourth, decorate the new objects with their shower shapes computed from the new clusters
        EGAM3_EGammaMaxVarReweightTool = EGammaReweightTool("EGAM3_EGammaMaxVarReweightTool",
                                                            #OutputLevel = DEBUG,
                                                            SGKey_electrons = "Electrons",
                                                            SGKey_photons="Photons",
                                                            NewCellContainerName="MaxVarCellContainer",
                                                            #NewElectronContainer = "MaxVarSwElectrons",
                                                            NewElectronContainer = "",
                                                            NewPhotonContainer = "MaxVarSwPhotons",
                                                            EMShowerBuilderTool = EGAM3_EMMaxVarShowerBuilderTool,
                                                            ClusterCorrectionToolName = "DFEgammaSWToolWithMaxVarCells",
                                                            CaloClusterLinkName="MaxVarSwClusterLink",
                                                            DecorateEGammaObjects = False,
                                                            DecorationPrefix = "Max_",
                                                            SaveReweightedContainer = True)
        print EGAM3_EGammaMaxVarReweightTool
        ToolSvc += EGAM3_EGammaMaxVarReweightTool


        ###########################################  REWEIGHTING VARIATIONS - MIN ######################################################
        
        from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import MinVarCellTool, ClusterDecoratorWithMinVarCells, EGammaMinVarReweightTool

        # first, create the container with the new cells (after reweighting)            
        EGAM3_MinVarCellTool = MinVarCellTool ("EGAM3_MinVarCellTol",
                                               #OutputLevel = DEBUG,
                                               CellContainerName="AODCellContainer",
                                               ReweightCellContainerName="MinVarCellContainer",
                                               SGKey_electrons = "Electrons",
                                               SGKey_photons = "Photons",
                                               ReweightCoefficients2DPath = "DerivationFrameworkCalo/CellReweight_v2d/rewCoeffs00.root")

      
        print EGAM3_MinVarCellTool
        ToolSvc += EGAM3_MinVarCellTool

        # second, run a tool that creates the clusters and objects from these new cells
        EGAM3_MinVarClusterDecoratorTool = ClusterDecoratorWithMinVarCells("EGAM3_MinVarClusterDecoratorTool",
                                                                           OutputClusterSGKey="EGammaSwClusterWithMinVarCells",
                                                                           OutputClusterLink="MinVarSwClusterLink",
                                                                           SGKey_caloCells = "MinVarCellContainer", 
                                                                           SGKey_electrons = "Electrons",
                                                                           SGKey_photons = "Photons")
        print EGAM3_MinVarClusterDecoratorTool
        ToolSvc += EGAM3_MinVarClusterDecoratorTool

        # third, schedule a tool that will be invoked by the EGammaReweightTool to create on-the-fly the shower shapes with the new cells      
        EGAM3_EMMinVarShowerBuilderTool = EMShowerBuilder("EGAM3_EMMinVarShowerBuilderTool", 
                                                          CellsName="MinVarCellContainer")
        print EGAM3_EMMinVarShowerBuilderTool
        ToolSvc += EGAM3_EMMinVarShowerBuilderTool

        # fourth, decorate the new objects with their shower shapes computed from the new clusters
        EGAM3_EGammaMinVarReweightTool = EGammaReweightTool("EGAM3_EGammaMinVarReweightTool",
                                                            #OutputLevel = DEBUG,
                                                            SGKey_electrons = "Electrons",
                                                            SGKey_photons="Photons",
                                                            NewCellContainerName="MinVarCellContainer",
                                                            #NewElectronContainer = "MinVarSwElectrons",
                                                            NewElectronContainer = "",
                                                            NewPhotonContainer = "MinVarSwPhotons",
                                                            EMShowerBuilderTool = EGAM3_EMMinVarShowerBuilderTool,
                                                            ClusterCorrectionToolName = "DFEgammaSWToolWithMinVarCells",
                                                            CaloClusterLinkName="MinVarSwClusterLink",
                                                            DecorateEGammaObjects = False,
                                                            DecorationPrefix = "Min_",
                                                            SaveReweightedContainer = True)

        print EGAM3_EGammaMinVarReweightTool
        ToolSvc += EGAM3_EGammaMinVarReweightTool


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

# Track thinning
if jobproperties.egammaDFFlags.doEGammaDAODTrackThinning:

    TrackThinningKeepElectronTracks = True
    TrackThinningKeepPhotonTracks = True
    TrackThinningKeepAllPhotonTracks = True
    TrackThinningKeepJetTracks = False
    TrackThinningKeepMuonTracks = False
    TrackThinningKeepTauTracks = False
    TrackThinningKeepPVTracks = False

    # Tracks associated with Jets
    if (TrackThinningKeepJetTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
        EGAM3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EGAM3JetTPThinningTool",
                                                                                ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ApplyAnd                = True)
        ToolSvc += EGAM3JetTPThinningTool
        print EGAM3JetTPThinningTool
        thinningTools.append(EGAM3JetTPThinningTool)
    
    # Tracks associated with Muons
    if (TrackThinningKeepMuonTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
        EGAM3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EGAM3MuonTPThinningTool",
                                                                                  ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                  MuonKey                 = "Muons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM3MuonTPThinningTool
        print EGAM3MuonTPThinningTool
        thinningTools.append(EGAM3MuonTPThinningTool)
    
    # Tracks associated with Electrons
    if (TrackThinningKeepElectronTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM3ElectronTPThinningTool",
                                                                                        ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 0*GeV",
                                                                                        BestMatchOnly = True,
                                                                                        ConeSize = 0.3,
                                                                                        ApplyAnd = False)
        ToolSvc += EGAM3ElectronTPThinningTool
        print EGAM3ElectronTPThinningTool
        thinningTools.append(EGAM3ElectronTPThinningTool)

    # Tracks associated with Photons
    if (TrackThinningKeepPhotonTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM3PhotonTPThinningTool",
                                                                                      ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      SelectionString         = "Photons.pt > 0*GeV",
                                                                                      BestMatchOnly = True,
                                                                                      ConeSize = 0.3,
                                                                                      ApplyAnd = False)
        
        ToolSvc += EGAM3PhotonTPThinningTool
        print EGAM3PhotonTPThinningTool
        thinningTools.append(EGAM3PhotonTPThinningTool)

    # Tracks associated with Photons (all tracks, large cone, for track isolation studies of the selected photon)
    if (TrackThinningKeepAllPhotonTracks) : 
        EGAM3PhotonTPThinningTool2 = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM3PhotonTPThinningTool2",
                                                                                       ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                       SGKey                   = "Photons",
                                                                                       GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                       SelectionString         = "Photons.pt > 9.5*GeV",
                                                                                       BestMatchOnly = False,
                                                                                       ConeSize = 0.6,
                                                                                       ApplyAnd = False)
        
        ToolSvc += EGAM3PhotonTPThinningTool2
        print EGAM3PhotonTPThinningTool2
        thinningTools.append(EGAM3PhotonTPThinningTool2)

    # Tracks associated with Taus
    if (TrackThinningKeepTauTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
        EGAM3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "EGAM3TauTPThinningTool",
                                                                                ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                                TauKey                  = "TauJets",
                                                                                ConeSize                = 0.6,
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM3TauTPThinningTool
        print EGAM3TauTPThinningTool
        thinningTools.append(EGAM3TauTPThinningTool)

    # Tracks from primary vertex
    if (TrackThinningKeepPVTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
        EGAM3TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM3TPThinningTool",
                                                                          ThinningService         = EGAM3ThinningHelper.ThinningSvc(),
                                                                          SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM3TPThinningTool
        print EGAM3TPThinningTool
        thinningTools.append(EGAM3TPThinningTool)


print "EGAM3 thinningTools: ", thinningTools


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
egam3Seq = CfgMgr.AthSequencer("EGAM3Sequence")
DerivationFrameworkJob += egam3Seq


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
augmentationTools = [EGAM3_EEMassTool,EGAM3_EEMassTool2,EGAM3_EEMassTool3,EGAM3_GainDecoratorTool,EGAM3_MaxCellDecoratorTool,EGAM3_PhotonTopoClusterMomentDecorator]
if DoCellReweighting:
    augmentationTools += [EGAM3_NewCellTool, EGAM3_ClusterDecoratorTool, EGAM3_EGammaReweightTool]
    if DoCellReweightingVariations:
        augmentationTools += [EGAM3_MaxVarCellTool, EGAM3_MaxVarClusterDecoratorTool, EGAM3_EGammaMaxVarReweightTool, EGAM3_MinVarCellTool, EGAM3_MinVarClusterDecoratorTool, EGAM3_EGammaMinVarReweightTool]

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
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM3SlimmingHelper.IncludeEGammaTriggerContent = True

# Append cell-reweighted collections to dictionary
if DoCellReweighting:
    EGAM3SlimmingHelper.AppendToDictionary = {"NewSwPhotons": "xAOD::PhotonContainer", "NewSwPhotonsAux": "xAOD::PhotonAuxContainer", "NewSwElectrons": "xAOD::ElectronContainer", "NewSwElectronsAux": "xAOD::ElectronAuxContainer" }
    if DoCellReweightingVariations:
        EGAM3SlimmingHelper.AppendToDictionary.update({ "MaxVarSwPhotons": "xAOD::PhotonContainer", "MaxVarSwPhotonsAux": "xAOD::PhotonAuxContainer", "MinVarSwPhotons": "xAOD::PhotonContainer", "MinVarSwPhotonsAux": "xAOD::PhotonAuxContainer" })
        EGAM3SlimmingHelper.AppendToDictionary.update({ "MaxVarSwElectrons": "xAOD::ElectronContainer", "MaxVarSwElectronsAux": "xAOD::ElectronAuxContainer", "MinVarSwElectrons": "xAOD::ElectronContainer", "MinVarSwElectronsAux": "xAOD::ElectronAuxContainer" })


# Extra variables
EGAM3SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM3SlimmingHelper.AllVariables = ExtraContainersPhotons
EGAM3SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM3SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM3SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM3_ClusterEnergyPerLayerDecorators:
    EGAM3SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )


# Add detailed shower shape variables
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
EGAM3SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EGAM3SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
EGAM3SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# This line must come after we have finished configuring EGAM3SlimmingHelper
EGAM3SlimmingHelper.AppendContentToStream(EGAM3Stream)

#Add full CellContainer
EGAM3Stream.AddItem("CaloCellContainer#AODCellContainer")
EGAM3Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")

