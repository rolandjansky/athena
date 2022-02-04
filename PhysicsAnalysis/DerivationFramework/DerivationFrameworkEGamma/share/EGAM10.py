#********************************************************************
# EGAM10.py
# Inclusive photon reduction - for e/gamma photon studies
# reductionConf flag EGAM10 in Reco_tf.py
# Migrated from r21 STDM2 format
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.EGAM10ExtraContent import *


#====================================================================
# read common DFEGamma settings from egammaDFFlags
#====================================================================
from DerivationFrameworkEGamma.egammaDFFlags import jobproperties
jobproperties.egammaDFFlags.print_JobProperties("full")


#====================================================================
# Set up sequence for this format and add to the top sequence
#====================================================================
EGAM10Sequence = CfgMgr.AthSequencer("EGAM10Sequence")
DerivationFrameworkJob += EGAM10Sequence


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM10Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM10Stream )
EGAM10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM10Stream.AcceptAlgs(["EGAM10Kernel"])


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
EGAM10_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name = "EGAM10_MaxCellDecoratorTool",
                                                                     SGKey_electrons = "Electrons",
                                                                     SGKey_photons   = "Photons")
ToolSvc += EGAM10_MaxCellDecoratorTool
augmentationTools += [EGAM10_MaxCellDecoratorTool]


#====================================================================
# PhotonVertexSelectionWrapper decoration tool - needs PhotonPointing tool
#====================================================================
from PhotonVertexSelection.PhotonVertexSelectionConf import CP__PhotonPointingTool
from RecExConfig.RecFlags  import rec

EGAM10_PhotonPointingTool = CP__PhotonPointingTool(name = "EGAM10_PhotonPointingTool",
                                                   isSimulation = DerivationFrameworkIsMonteCarlo)
ToolSvc += EGAM10_PhotonPointingTool

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__PhotonVertexSelectionWrapper
EGAM10_PhotonVertexSelectionWrapper = DerivationFramework__PhotonVertexSelectionWrapper(name = "EGAM10_PhotonVertexSelectionWrapper",
                                                                                        PhotonPointingTool = EGAM10_PhotonPointingTool,
                                                                                        DecorationPrefix = "EGAM10",
                                                                                        PhotonContainer = "Photons",
                                                                                        VertexContainer = "PrimaryVertices")
ToolSvc += EGAM10_PhotonVertexSelectionWrapper
augmentationTools += [EGAM10_PhotonVertexSelectionWrapper]


#====================================================================
# SET UP THINNING
#====================================================================

electronRequirements = '(Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.5) && (Electrons.DFCommonElectronsLHLoose)'
photonRequirements = '(DFCommonPhotons_et >= 15*GeV) && (abs(DFCommonPhotons_eta) < 2.5)' # && (Photons.DFCommonPhotonsLoose)'

# All Track within a cone DeltaR=0.6 around Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM10_ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "EGAM10_ElectronTPThinningTool",
                                                                                StreamName              = streamName,
                                                                                SGKey                  = "Electrons",
                                                                                GSFTrackParticlesKey   = "GSFTrackParticles",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                SelectionString        = electronRequirements,
                                                                                BestMatchOnly          = True,
                                                                                ConeSize               = 0.6)
ToolSvc += EGAM10_ElectronTPThinningTool
thinningTools.append(EGAM10_ElectronTPThinningTool)

# Track associated to all Electrons for ambiguity resolver tool
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM10_ElectronTPThinningToolAR = DerivationFramework__EgammaTrackParticleThinning(name = "EGAM10_ElectronTPThinningToolAR",
                                                                                   StreamName              = streamName,
                                                                                   SGKey                  = "Electrons",
                                                                                   GSFTrackParticlesKey   = "GSFTrackParticles",
                                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                   BestMatchOnly          = True)
ToolSvc += EGAM10_ElectronTPThinningToolAR
thinningTools.append(EGAM10_ElectronTPThinningToolAR)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EGAM10_PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name = "EGAM10_PhotonTPThinningTool",
                                                                               StreamName              = streamName,
                                                                               SGKey                  = "Photons",
                                                                               GSFTrackParticlesKey   = "GSFTrackParticles",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles",
                                                                               SelectionString        = photonRequirements,
                                                                               BestMatchOnly          = False,
                                                                               ConeSize               = 0.6)
ToolSvc += EGAM10_PhotonTPThinningTool
thinningTools.append(EGAM10_PhotonTPThinningTool)

# Possibility to thin CaloCalTopoClusters (UE/PU iso corrections not recomputable then)
# see https://indico.cern.ch/event/532191/contributions/2167754/attachments/1273075/1887639/ArthurLesage_ASGMeeting_20160513.pdf, S6-7


#====================================================================
# Setup the skimming criteria
#====================================================================

#====================================================================
# SKIMMING TOOL - OFFLINE
#====================================================================
photonSelection = '(count(' + photonRequirements + ') >= 1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM10_OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM10_OfflineSkimmingTool",
                                                                          expression = photonSelection)
ToolSvc += EGAM10_OfflineSkimmingTool
print("EGAM10 offline skimming tool:", EGAM10_OfflineSkimmingTool)


#====================================================================
# SKIMMING TOOL - trigger-based selection
#====================================================================
# 2015 data triggers. unprescaled list validated with https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled#Egamma_MET.
# full list avaiable here: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/TrigMenuRulebook/trunk/python/Physics_pp_v5_rules.py (good luck)
singlePhotonTriggers = ['HLT_g10_loose',
                        'HLT_g15_loose_L1EM7',
                        'HLT_g20_loose_L1EM12',
                        'HLT_g25_loose_L1EM15 ',
                        'HLT_g35_loose_L1EM15',
                        'HLT_g25_loose',
                        'HLT_g25_medium',
                        'HLT_g35_loose',
                        'HLT_g35_medium',
                        'HLT_g40_loose_L1EM15',
                        'HLT_g45_loose_L1EM15',
                        'HLT_g50_loose_L1EM15',
                        'HLT_g50_loose',
                        'HLT_g60_loose',
                        'HLT_g70_loose',
                        'HLT_g80_loose',
                        'HLT_g100_loose',
                        'HLT_g120_loose',
                        'HLT_g140_loose',
                        'HLT_g140_loose_HLTCalo',
                        'HLT_g200_etcut',
                        'HLT_g10_etcut',
                        'HLT_g20_etcut_L1EM12',
                        'HLT_g300_etcut_L1EM24VHIM',
                        'HLT_g22_tight',
                        'HLT_g25_medium_L1EM22VHI',
                        'HLT_g35_loose_L1EM22VHI',
                        'HLT_g45_tight_L1EM22VHI',
                        'HLT_g35_loose_L1EM24VHI',
                        'HLT_g35_loose_L1EM26VHI',
                        'HLT_g10_medium',
                        'HLT_g15_loose_L1EM3',
                        'HLT_g15_loose',
                        'HLT_g20_loose',
                        'HLT_g20_tight',
                        'HLT_g40_tight',
                        'HLT_g45_tight',
                        'HLT_g60_loose_L1EM15VH ',
                        'HLT_g180_loose',
                        'HLT_g60_loose_L1EM24VHI',
                        'HLT_g70_loose_L1EM24VHI',
                        'HLT_g80_loose_L1EM24VHI',
                        'HLT_g100_loose_L1EM24VHI',
                        'HLT_g120_loose_L1EM24VHI',
                        'HLT_g60_loose_L1EM26VHI',
                        'HLT_g70_loose_L1EM26VHI',
                        'HLT_g80_loose_L1EM26VHI',
                        'HLT_g100_loose_L1EM26VHI',
                        'HLT_g120_loose_L1EM26VHI',
                        'HLT_g140_loose_L1EM26VHI',
                        'HLT_g160_loose_L1EM26VHI',
                        'HLT_g180_loose_L1EM26VHI',
                        'HLT_200_loose_L1EM26VHI',
                        'HLT_g20_loose_L1EM18VH',
                        'HLT_g24_loose',
                        'HLT_g35_medium_L1EM22VHI',
                        'HLT_g35_medium_L1EM24VHI',
                        'HLT_g10_loose_L1EM3',
                        'HLT_g10_medium_L1EM3',
                        'HLT_g140_tight_L1EM24VHIM',
                        'HLT_g200_loose_L1EM24VHIM',
                        'HLT_g20_tight_L1EM15VHI',
                        'HLT_g20_tight_icalovloose_L1EM15VHI',
                        'HLT_g20_tight_icalotight_L1EM15VHI',
                        'HLT_g22_tight_L1EM15VHI',
                        'HLT_g22_tight_icalovloose_L1EM15VHI',
                        'HLT_g22_tight_icalotight_L1EM15VHI',
                        'HLT_g25_loose_L1EM20VH',
                        'HLT_g12_loose',
                        'HLT_g12_medium',
                        'HLT_g70_loose_L1EN24VHIM',
                        'HLT_g80_loose_L1EM24VHIM',
                        'HLT_g80_loose_icalovloose_L1EM24VHIM',
                        'HLT_g60_loose_L1EM24VHIM',
                        'HLT_g100_loose_L1EM24VHIM',
                        'HLT_g120_loose_L1EM24VHIM',
                        'HLT_g140_loose_L1EM24VHIM',
                        'HLT_g160_loose_L1EM24VHIM',
                        'HLT_g180_loose_L1EM24VHIM',
                        'HLT_g35_loose_L1EM24VHIM',
                        'HLT_g35_tight_icalotight_L1EM24VHIM',
                        'HLT_g40_tight_icalotight_L1EM24VHIM',
                        'HLT_g85_tight_L1EM24VHIM',
                        'HLT_g85_tight_icalovloose_L1EM24VHIM',
                        'HLT_g100_tight_L1EM24VHIM',
                        'HLT_g100_tight_icalovloose_L1EM24',
                        'HLT_g45_tight_L1EM24VHI',
                        'HLT_g300_etcut_L1EM24VHI',
                        'HLT_g85_tight_L1EM24VHI',
                        'HLT_g100_tight',
                        'HLT_g100_tight_L1EM24VHI',
                        'HLT_g100_tight_icalovloose_L1EM24VHIM',
                        'HLT_g70_loose_L1EM24VHIM',
                        'HLT_g85_tight',
                        'HLT_g6_loose',
                        'HLT_g6_tight_icalotight',
                        'HLT_g25_tight_L1EM20VH',
                        'HLT_g15_loose_L1EM8VH',
                        'HLT_g50_loose_L1EM20VH',
                        'HLT_g60_loose_L1EM20VH',
                        'HLT_g25_medium_L1EM20VH',
                        'HLT_g35_medium_L1EM20VH',
                        'HLT_g35_loose_L1EM20VH',
                        'HLT_g22_tight_icalovloose',
                        'HLT_g22_tight_icalotight',
                        'HLT_g35_tight_icalotight_L1EM24VHI',
                        'HLT_g40_tight_icalotight_L1EM24VHI',
                        'HLT_g85_tight_icalovloose_L1EM24VHI',
                        'HLT_g100_tight_icalovloose_L1EM24VHI',
                        'HLT_g35_medium_icalovloose',
                        'HLT_g35_medium_icalotight',
                        'HLT_g15_etcut_L1EM7',
                        'HLT_g20_medium_L1EM15',
                        'HLT_g20_tight_L1EM15',
                        'HLT_g20_etcut_L1EM15',
                        'HLT_g20_medium',
                        'HLT_g20_etcut',
                        'HLT_g25_medium_L1EM15',
                        'HLT_g25_tight_L1EM15',
                        'HLT_g25_etcut_L1EM15',
                        'HLT_g30_loose_L1EM15',
                        'HLT_g30_etcut_L1EM15']


diPhotonTriggers = ['HLT_2g20_loose_L12EM15',
                    'HLT_2g20_loose',
                    'HLT_2g20_tight',
                    'HLT_2g22_tight',
                    'HLT_2g25_tight',
                    'HLT_g35_loose_g25_loose',
                    'HLT_g35_medium_HLTCalo_g25_medium_HLTCalo',
                    'HLT_g35_loose_L1EM15_g25_loose_L1EM15',
                    'HLT_g35_loose_L1EM15VH_g25_loose_L1EM15VH',
                    'HLT_g35_medium_g25_medium',
                    'HLT_2g50_loose',
                    'HLT_2g60_loose_L12EM15VH ',
                    'HLT_2g10_loose',
                    'HLT_2g50_loose_L12EM18VH',
                    'HLT_2g60_loose_L12EM18VH',
                    'HLT_2g50_loose_L12EM20VH',
                    'HLT_g50_loose_L12EM18VH',
                    'HLT_g60_loose_L12EM18VH',
                    'HLT_g50_loose_L12EM20VH',
                    'HLT_g60_loose_L12EM20VH',
                    'HLT_2g25_tight_L12EM20VH',
                    'HLT_g35_loose_g25_loose_L12EM18VH',
                    'HLT_g35_loose_g25_loose_L12EM20VH ',
                    'HLT_g35_medium_g25_medium_L12EM18VH',
                    'HLT_g35_medium_g25_medium_L12EM20VH',
                    'HLT_2g20_tight_L12EM15VHI',
                    'HLT_2g20_tight_icalovloose_L12EM15VHI',
                    'HLT_2g20_tight_icalotight_L12EM15VHI',
                    'HLT_2g22_tight_L12EM15VHI',
                    'HLT_2g22_tight_icalovloose_L12EM15VHI',
                    'HLT_2g22_tight_icalotight_L12EM15VHI',
                    'HLT_2g60_loose_L12EM20VH',
                    'HLT_2g3_loose_dPhi15_L12EM3_VTE50',
                    'HLT_2g3_loose_L12EM3_VTE50',
                    'HLT_2g3_medium_dPhi15_L12EM3_VTE50',
                    'HLT_2g22_tight_icalovloose',
                    'HLT_2g22_tight_icalotight',
                    'HLT_2g10_loose_L12EM7',
                    'HLT_2g15_loose_L12EM7']


triPhotonTriggers = ['HLT_3g15_loose',
                     'HLT_g20_loose_2g15_loose_L12EM13VH',
                     'HLT_2g20_loose_g15_loose',
                     'HLT_3g20_loose',
                     'HLT_3g20_loose_L12EM18VH',
                     'HLT_2g24_loose_g15_loose',
                     'HLT_2g24_g20_loose',
                     'HLT_3g24_loose_L12EM20VH',
                     'HLT_2g25_loose_g15_loose',
                     'HLT_2g25_loose_g20_loose',
                     'HLT_3g25_loose']

firstDataTriggers = ['HLT_2g10_loose',
                     'HLT_g20_loose_L1EM15']

# 2016 data triggers (preliminary)
# full list avaiable here: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/TriggerMenu/trunk/python/menu/Physics_pp_v6.py (good luck)
singlePhotonTriggers_2016 = ['HLT_g140_loose',
                             'HLT_g160_loose',
                             'HLT_g200_loose',
                             'HLT_g140_tight',
                             'HLT_g250_etcut',
                             'HLT_g300_etcut']

diPhotonTriggers_2016 = ['HLT_g35_medium_g25_medium',
                         'HLT_2g50_loose',
                         'HLT_2g60_loose_L12EM15VH',
                         'HLT_2g20_tight',
                         'HLT_2g22_tight']

triPhotonTriggers_2016 = ['HLT_2g20_loose_g15_loose',
                          'HLT_3g20_loose']

noalgTriggers = ['HLT_noalg_L1EM12',
                 'HLT_noalg_L1EM15',
                 'HLT_noalg_L1EM18VH',
                 'HLT_noalg_L1EM20VH',
                 'HLT_noalg_L1EM10',
                 'HLT_noalg_L1EM10VH',
                 'HLT_noalg_L1EM13VH',
                 'HLT_noalg_L1EM20VHI',
                 'HLT_noalg_L1EM22VHI',
                 'HLT_noalg_L1EM8VH',
                 'HLT_noalg_L1EM15VH',
                 'HLT_noalg_L12EM7',
                 'HLT_noalg_L12EM15']

allTriggers = singlePhotonTriggers + diPhotonTriggers + triPhotonTriggers + firstDataTriggers + singlePhotonTriggers_2016 + diPhotonTriggers_2016 + triPhotonTriggers_2016 + noalgTriggers

#remove duplicates
allTriggers = list(set(allTriggers))

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EGAM10_TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name = "EGAM10_TriggerSkimmingTool", TriggerListOR = allTriggers)
print('EGAM10 list of triggers used for skimming:')
for trig in allTriggers: print(trig)

# can't use trigger API (https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI) because we also need prescaled triggers
# so we cannot use trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
# which gives only the unprescaled ones
# nor trig_g   = TriggerAPI.getActive(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
# which seems to return only single photon triggers but not multi-photon ones

ToolSvc += EGAM10_TriggerSkimmingTool
print("EGAM10 trigger skimming tool:", EGAM10_TriggerSkimmingTool)

#
# Make AND of trigger-based and offline-based selections
#
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
EGAM10_SkimmingTool = DerivationFramework__FilterCombinationAND(name="EGAM10_SkimmingTool", FilterList=[EGAM10_OfflineSkimmingTool,EGAM10_TriggerSkimmingTool] )
ToolSvc+=EGAM10_SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
print("EGAM10 skimming tools: ", [EGAM10_SkimmingTool])
print("EGAM10 thinning tools: ", thinningTools)
print("EGAM10 augmentation tools: ", augmentationTools)
EGAM10Sequence += CfgMgr.DerivationFramework__DerivationKernel("EGAM10Kernel",
                                                               SkimmingTools = [EGAM10_SkimmingTool],
                                                               AugmentationTools = augmentationTools,
                                                               ThinningTools = thinningTools
                                                               )


#====================================================================
# JET/MET
#====================================================================
from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
from JetRecConfig.StandardSmallRJets import AntiKt4Truth,AntiKt4TruthWZ,AntiKt4PV0Track
jetList = [AntiKt4Truth,AntiKt4TruthWZ,AntiKt4PV0Track]
addDAODJets(jetList, EGAM10Sequence)


#====================================================================
# SET UP SLIMMING
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EGAM10SlimmingHelper = SlimmingHelper("EGAM10SlimmingHelper")
EGAM10SlimmingHelper.SmartCollections = ["Electrons",
                                         "Photons",
                                         "AntiKt4EMPFlowJets",
                                         "InDetTrackParticles",
                                         "PrimaryVertices",
]
EGAM10SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM10SlimmingHelper.ExtraVariables = ExtraContentElectrons + ExtraContentPhotons + ExtraContentVtx + ExtraContentTrk + ExtraContentJets + ExtraContentEventShape

EGAM10SlimmingHelper.AllVariables += ExtraContainers

# Add event info
if jobproperties.egammaDFFlags.doEGammaEventInfoSlimming:
    EGAM10SlimmingHelper.SmartCollections.append("EventInfo")
else:
    EGAM10SlimmingHelper.AllVariables += ["EventInfo"]

# add detailed photon shower shape variables
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *
EGAM10SlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

# add new TTVA isolation variables
EGAM10SlimmingHelper.ExtraVariables += ["Photons.ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500"]

# additional truth-level variables
if DerivationFrameworkIsMonteCarlo:
    EGAM10SlimmingHelper.ExtraVariables += ExtraElectronsTruth+ExtraPhotonsTruth
    EGAM10SlimmingHelper.AllVariables   += ExtraContainersTruth+ExtraContainersTruthPhotons
    EGAM10SlimmingHelper.AllVariables   += ["TruthIsoCentralEventShape", "TruthIsoForwardEventShape"]
    EGAM10SlimmingHelper.AppendToDictionary = ExtraDictionary

# This line must come after we have finished configuring EGAM1SlimmingHelper
EGAM10SlimmingHelper.AppendContentToStream(EGAM10Stream)
