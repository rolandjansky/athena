#********************************************************************
# EGAM9.py - keep events passing or of photon triggers used for
#            boostrap efficiency measurement of photon triggers
# reductionConf flag EGAM9 in Reco_tf.py
# author: fernando.monticelli@cern.ch
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM9ExtraContent import *


#====================================================================
# read common DFEGamma settings from egammaDFFlags
#====================================================================
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")

# additional settings for this derivation
thinCells = False
keepCells = False
RecomputeEGammaSelectors = True

#====================================================================
# check if we run on data or MC (DataSource = geant4)
#====================================================================
from AthenaCommon.GlobalFlags import globalflags
print("EGAM9 globalflags.DataSource(): ", globalflags.DataSource())
if globalflags.DataSource()!='geant4':
    ExtraContainersTrigger += ExtraContainersTriggerDataOnly


#====================================================================
# Set up sequence for this format and add to the top sequence
#====================================================================
EGAM9Sequence = CfgMgr.AthSequencer("EGAM9Sequence")
DerivationFrameworkJob += EGAM9Sequence


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM9Stream )
EGAM9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM9Stream.AcceptAlgs(["EGAM9Kernel"])


### Thinning and augmentation tools lists
augmentationTools = []
thinningTools=[]


#====================================================================
# SET UP AUGMENTATIONS
#====================================================================


#====================================================================
# Max Cell sum decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
EGAM9_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "EGAM9_MaxCellDecoratorTool",
                                                                    SGKey_electrons         = "Electrons",
                                                                    SGKey_photons           = "Photons")
ToolSvc += EGAM9_MaxCellDecoratorTool
augmentationTools += [EGAM9_MaxCellDecoratorTool]


#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM9_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM9_GainDecoratorTool
augmentationTools += [EGAM9_GainDecoratorTool]

cluster_sizes = (3,7), (5,5), (7,11)
EGAM9_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]
augmentationTools += EGAM9_ClusterEnergyPerLayerDecorators


#====================================================================
# SET UP THINNING
#====================================================================

print('WARNING, Thinning of trigger navigation has to be properly implemented in R22')
#from DerivationFrameworkCore.ThinningHelper import ThinningHelper
#EGAM9ThinningHelper = ThinningHelper( "EGAM9ThinningHelper" )
#EGAM9ThinningHelper.TriggerChains = '(^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_[eg].*_[0-9]*[eg][0-9].*)(?!HLT_eb.*)(?!.*larpeb.*)(?!HLT_.*_AFP_.*)(HLT_[eg].*))'
#EGAM9ThinningHelper.AppendToStream( EGAM9Stream, ExtraContainersTrigger )


# Truth thinning
if globalflags.DataSource()=='geant4':
    truth_cond_WZH = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_top = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
    truth_cond_gam = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
    truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)'   # stable particles
    truth_expression = '(' + truth_cond_WZH + ' ||  ' + truth_cond_lep +' || '+truth_cond_top +' || '+truth_cond_gam + ') || (' + truth_cond_finalState+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    EGAM9TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EGAM9TruthThinningTool",
                                                                       StreamName              = streamName,
                                                                       ParticleSelectionString = truth_expression,
                                                                       PreserveDescendants     = False,
                                                                       PreserveGeneratorDescendants     = True,
                                                                       PreserveAncestors      = True)
    
    
    ToolSvc += EGAM9TruthThinningTool
    thinningTools.append(EGAM9TruthThinningTool)
    

# Track thinning
if jobproperties.egammaDFFlags.doEGammaDAODTrackThinning:

    TrackThinningKeepElectronTracks = False
    TrackThinningKeepPhotonTracks = True
    TrackThinningKeepPVTracks = False

    # tracks associated with Electrons
    if (TrackThinningKeepElectronTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM9ElectronTPThinningTool",
                                                                                        StreamName              = streamName,
                                                                                        SGKey                   = "Electrons",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 0*GeV",
                                                                                        BestMatchOnly = True,
                                                                                        ConeSize = 0.3)
        ToolSvc += EGAM9ElectronTPThinningTool
        print(EGAM9ElectronTPThinningTool)
        thinningTools.append(EGAM9ElectronTPThinningTool)
    
    # tracks associated with Photons
    if (TrackThinningKeepPhotonTracks) : 
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
        EGAM9PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EGAM9PhotonTPThinningTool",
                                                                                      StreamName              = streamName,
                                                                                      SGKey                   = "Photons",
                                                                                      GSFTrackParticlesKey    = "GSFTrackParticles",        
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      SelectionString         = "Photons.pt > 0*GeV",
                                                                                      BestMatchOnly = True,
                                                                                      ConeSize = 0.3)
        
        ToolSvc += EGAM9PhotonTPThinningTool
        print(EGAM9PhotonTPThinningTool)
        thinningTools.append(EGAM9PhotonTPThinningTool)
        
    # Tracks from primary vertex
    thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
    if (TrackThinningKeepPVTracks) :
        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
        EGAM9TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "EGAM9TPThinningTool",
                                                                          StreamName              = streamName,
                                                                          SelectionString         = thinning_expression,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
        ToolSvc += EGAM9TPThinningTool
        print(EGAM9TPThinningTool)
        thinningTools.append(EGAM9TPThinningTool)


#============ Thin cells for EGAM9 ==================================
# Keep only calo cells associated with the egammaClusters collection
#====================================================================
if thinCells:
    from DerivationFrameworkCalo.CaloCellDFGetter import thinCaloCellsForDF
    thinCaloCellsForDF (inputClusterKeys=["egammaClusters"],
                        streamName = EGAM9Stream.Name,
                        outputCellKey = "DFEGAMCellContainer")


#====================================================================
# Setup the skimming criteria
#====================================================================

#====================================================================
# offline based selection
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
photon_selection = ''
electron_selection = ''
if RecomputeEGammaSelectors:
    photon_selection = 'count(Photons.pt > 9.5*GeV && Photons.DFCommonPhotonsIsEMLoose)>0'
    electron_selection = ' count(Electrons.pt > 9.5*GeV && Electrons.DFCommonElectronsLHMedium)>0 '
else:
    photon_selection = 'count(Photons.pt > 9.5*GeV && Photons.Loose)>0'
    electron_selection = ' count(Electrons.pt > 9.5*GeV && Electrons.LHMedium>0)>0 '


objectSelection = '( (' + photon_selection + ') || ('+ electron_selection +' ) )'
EGAM9_OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM9_OfflineSkimmingTool",
                                                                         expression = objectSelection)
ToolSvc += EGAM9_OfflineSkimmingTool
print("EGAM9 offline skimming tool:", EGAM9_OfflineSkimmingTool)


#====================================================================
# trigger based selection
#====================================================================
triggers =[]

# HLT_noalg_ triggers
triggers += ['HLT_noalg_L1EM15VH']
triggers += ['HLT_noalg_L1EM12']
triggers += ['HLT_noalg_L1EM15']
triggers += ['HLT_noalg_L1EM18VH']
triggers += ['HLT_noalg_L1EM20VH']
triggers += ['HLT_noalg_L1EM10']
triggers += ['HLT_noalg_L1EM10VH']
triggers += ['HLT_noalg_L1EM13VH']
triggers += ['HLT_noalg_L1EM20VHI']
triggers += ['HLT_noalg_L1EM22VHI']
triggers += ['HLT_noalg_L1EM8VH']

# pt_cut triggers
triggers += ['HLT_g20_etcut_L1EM12']

# Passed through triggers for bootstrapping
triggers += ['HLT_g10_loose']
triggers += ['HLT_g15_loose_L1EM7']
triggers += ['HLT_g20_loose_L1EM12']
triggers += ['HLT_g20_loose']
triggers += ['HLT_g25_loose_L1EM15']
triggers += ['HLT_g60_loose']
triggers += ['HLT_g100_loose']
triggers += ['HLT_g120_loose']
triggers += ['HLT_g160_loose']
triggers += ['HLT_g160_loose_L1EM24VHIM']
triggers += ['HLT_g180_loose']
triggers += ['HLT_g180_loose_L1EM24VHIM']
triggers += ['HLT_g35_loose_L1EM15']
triggers += ['HLT_g40_loose_L1EM15']
triggers += ['HLT_g45_loose_L1EM15']
triggers += ['HLT_g50_loose_L1EM15']
triggers += ['HLT_g70_loose']
triggers += ['HLT_g80_loose']
triggers += ['HLT_g140_loose']
triggers += ['HLT_g200_loose']

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EGAM9_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "EGAM9_TriggerSkimmingTool", TriggerListOR = triggers)
ToolSvc += EGAM9_TriggerSkimmingTool
print("EGAM9 trigger skimming tool:", EGAM9_TriggerSkimmingTool)

#====================================================================
# make the AND of offline and trigger-based selections
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
EGAM9_SkimmingTool = DerivationFramework__FilterCombinationAND(name="EGAM9SkimmingTool", FilterList=[EGAM9_OfflineSkimmingTool,EGAM9_TriggerSkimmingTool] )
ToolSvc+=EGAM9_SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
print("EGAM9 skimming tools: ", [EGAM9_SkimmingTool])
print("EGAM9 thinning tools: ", thinningTools)
print("EGAM9 augmentation tools: ", augmentationTools)
EGAM9Sequence += CfgMgr.DerivationFramework__DerivationKernel("EGAM9Kernel",
                                                              AugmentationTools = augmentationTools,
                                                              SkimmingTools = [EGAM9_SkimmingTool],
                                                              ThinningTools = thinningTools
                                                              )


#====================================================================
# JET/MET
#====================================================================
#from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
#from JetRecConfig.StandardSmallRJets import AntiKt4Truth
#addDAODJets([AntiKt4Truth], EGAM9Sequence)


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM9SlimmingHelper = SlimmingHelper("EGAM9SlimmingHelper")
EGAM9SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM9SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM9SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM9SlimmingHelper.AllVariables = ExtraContainersElectrons
EGAM9SlimmingHelper.AllVariables += ExtraContainersPhotons
EGAM9SlimmingHelper.AllVariables += ExtraContainersTrigger

if globalflags.DataSource()=='geant4':
    EGAM9SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM9SlimmingHelper.AllVariables += ExtraContainersTruth
else:
    EGAM9SlimmingHelper.ExtraVariables += ExtraContainersTriggerDataOnly

for tool in EGAM9_ClusterEnergyPerLayerDecorators:
    EGAM9SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# Add event info
if jobproperties.egammaDFFlags.doEGammaEventInfoSlimming:
    EGAM9SlimmingHelper.SmartCollections.append("EventInfo")
else:
    EGAM9SlimmingHelper.AllVariables += ["EventInfo"]

# Add detailed shower shape variables
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
EGAM9SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EGAM9SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
EGAM9SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# This line must come after we have finished configuring EGAM9SlimmingHelper
EGAM9SlimmingHelper.AppendContentToStream(EGAM9Stream)

#Add CellContainer
if keepCells:
    if thinCells:
        EGAM9Stream.AddItem("CaloCellContainer#DFEGAMCellContainer")
    else:
        EGAM9Stream.AddItem("CaloCellContainer#AllCalo")
    # Add the cluster->cells links
    EGAM9Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
