#====================================================================
# EXOT23.py 
# reductionConf flag EXOT23 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *


#=======================================
# SET UP STREAM
#=======================================
streamName   = derivationFlags.WriteDAOD_EXOT23Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT23Stream )
EXOT23Stream = MSMgr.NewPoolRootStream( streamName, fileName )

# attach output stream to kernel
EXOT23Stream.AcceptAlgs(["EXOT23KernelSkim"])

# create sequence for specific stream
SeqEXOT23 = CfgMgr.AthSequencer("SeqEXOT23")
DerivationFrameworkJob += SeqEXOT23


#====================================================================
# THINNING TOOLS
#====================================================================
# initilize ThinningHelper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT23ThinningHelper = ThinningHelper( "EXOT23ThinningHelper" )
thinningTools = []

# adding unconfigured thinning helper
EXOT23ThinningHelper.AppendToStream( EXOT23Stream )

# TrackParticle thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# require pt > 0.5 GeV tracks
EXOT23TPThinningTool = DerivationFramework__TrackParticleThinning(
                         name                   = "EXOT23TPThinningTool",
                         ThinningService        = EXOT23ThinningHelper.ThinningSvc(),
                         SelectionString        = "InDetTrackParticles.pt > 0.5*GeV",
                         InDetTrackParticlesKey = "InDetTrackParticles" )

ToolSvc += EXOT23TPThinningTool
thinningTools.append(EXOT23TPThinningTool)


##====================================================================
## AUGMENTATION TOOLS
##====================================================================
AugmentationTools = []

if DerivationFrameworkHasTruth:
    # -- decorate truth particles with track parameters -- #
    # decorator tool
    from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValTruthDecoratorTool
    IDPV_TruthDecoratorTool = InDetPhysValTruthDecoratorTool()
    ToolSvc += IDPV_TruthDecoratorTool
    # augmentation tool
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles as TrkParam4TruthPart
    TrkParam4Truth = TrkParam4TruthPart( name = "TrkParam4Truth",
                                         OnlyDressPrimaryTracks = False )

    # WARNING !!! this crashes on R21: ERROR SG::ExcStoreLocked: Attempted to modify auxiliary data in a locked store: `::d0'
    # --> commented out until problem is fixed...
    #ToolSvc += TrkParam4Truth
    #AugmentationTools.append( TrkParam4Truth )


#====================================================================
# SKIMMING TOOLS
#====================================================================
# trigger skimming
triggers = [
    # single jet triggers for di-jet backgrounds
    "HLT_j15",  # 2015 support (support triggers are prescaled)
    "HLT_j25",  # 2016 support; 2017 support
    "HLT_j35",  # 2015 support; 2016 support; 2017 support
    "HLT_j45",  # 2015 support; 2016 support; 2017 support
    "HLT_j55",  # 2015 support; 2016 support; 
    "HLT_j60",  # 2015 support; 2016 support; 2017 support
    "HLT_j85",  # 2015 support; 2016 support; 2017 support
    "HLT_j100", # 2015 support
    "HLT_j110", # 2015 support; 2016 support; 2017 support
    "HLT_j150", # 2015 support
    "HLT_j175", # 2015 support; 2016 support; 2017 support
    "HLT_j200", # 2015 support
    "HLT_j260", # 2015 support; 2016 support; 2017 support
    "HLT_j300", # 2015 support
    "HLT_j320", # 2015 support; 2016 primary
    "HLT_j340", # 2016 primary; 2017 primary
    "HLT_j360", # 2015 primary; 2016 primary; 2017 support
    "HLT_j380", # 2015 primary; 2016 primary; 2017 primary
    "HLT_j400", # 2016 primary; 2017 primary
    "HLT_j420", # 2015 primary; 2016 primary; 2017 primary
    "HLT_j440", # 2015 primary; 2017 primary
    "HLT_j450", # 2017 primary
    "HLT_j460", # 2015 primary; 2016 primary; 2017 primary
    "HLT_j480", # 2017 primary
    "HLT_j500", # 2017 primary
    "HLT_j520", # 2017 primary
    # four-jet triggers for EJs signal
    "HLT_4j90",
    "HLT_4j100",
    "HLT_4j110",
    "HLT_4j120",
    "HLT_4j130",
    "HLT_4j140",
    "HLT_4j150"
    ]
expression = "(" + " || ".join(triggers) + ")"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT23SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT23SkimmingTool",
                                                                 expression = expression)

ToolSvc += EXOT23SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

SeqEXOT23 += CfgMgr.DerivationFramework__DerivationKernel(name = "EXOT23KernelSkim",
                                                          SkimmingTools = [EXOT23SkimmingTool])
SeqEXOT23 += CfgMgr.DerivationFramework__DerivationKernel(name = "EXOT23KernelAug",
                                                          AugmentationTools = AugmentationTools,
                                                          ThinningTools = thinningTools)


#====================================================================
# TRUTH DARK JETS
#====================================================================
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

OutputJets["EXOT23"] = []

if jetFlags.useTruth:
    # select truth dark hadrons for jet clustering
    from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
    from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
    from JetRec.JetRecStandardTools import truthClassifier

    jtm += CopyTruthJetParticles("truthpartcopydark",
                                OutputName = "JetInputTruthParticlesDarkHad",
                                MCTruthClassifier = truthClassifier,
                                IncludeSMParts = False,
                                IncludeDarkHads = True)

    # add JetToolRunner instance for new truthpartcopydark tool
    jtm += CfgMgr.JetToolRunner("jetrun_copytruthpartdark",
                                EventShapeTools = [],
                                Tools = [jtm.truthpartcopydark])

    # import pseudojet getter
    from JetRec.JetRecConf import PseudoJetGetter
    
    # build truth dark pseudojets
    jtm += PseudoJetGetter("truthdarkget",
                        Label = "Truth",
                        InputContainer = jtm.truthpartcopydark.OutputName,
                        OutputContainer = "PseudoJetTruthDark",
                        GhostScale = 0.0,
                        SkipNegativeEnergy = True)

    # add truth dark pseudojet getters
    truthdarkgetters = [jtm.truthdarkget]
    # add truth cone matching and truth flavor ghosts for truth dark jets
    from JetRec.JetRecStandardToolManager import flavorgetters
    truthdarkgetters += flavorgetters

    # add truth dark getters list to jtm
    jtm.gettersMap["truthdark"] = list(truthdarkgetters)

    # build list of truth dark jet modifiers
    jtm.modifiersMap["truthdarkmods"] = [jtm.truthpartondr,
                                        jtm.partontruthlabel]


# define truth dark jets helper
# analogous to addRscanJets in JETM9
# calls addStandardJets in JetCommon -- modified to include custom pseudojetgetter inputs
def addTruthDarkJets(jetalg, radius, sequence, outputlist):
    inputtype = "TruthDark"
    jetname = "{0}{1}{2}Jets".format(jetalg, int(radius*10), inputtype)
    algname = "jetalg" + jetname

    if not hasattr(sequence, algname):
        # add JetAlgorithm instance for truth dark jets before scheduling jet finder
        if not hasattr(sequence, "jetalg_copytruthpartdark"):
            sequence += CfgMgr.JetAlgorithm("jetalg_copytruthpartdark",
                                            Tools = [jtm.jetrun_copytruthpartdark])
        # call standard jet helper to schedule jet finder
        addStandardJets(jetalg, radius, "TruthDark", mods="truthdarkmods",
                        ptmin=5000., algseq=sequence, outputGroup=outputlist,
                        customGetters='truthdark')


# call truth dark helper
if jetFlags.useTruth:
    addTruthDarkJets("AntiKt", 0.4, SeqEXOT23, "EXOT23")
    addTruthDarkJets("AntiKt", 1.0, SeqEXOT23, "EXOT23")


#====================================================================
# RESTORE AOD-REDUCED TRUTH JETS
#====================================================================
reducedJetList = [ "AntiKt4TruthJets" ]
replaceAODReducedJets( reducedJetList, SeqEXOT23, "EXOT23" )


#====================================================================
# RE-TAG PFLOW JETS FOR B-TAGGING INFO
#====================================================================
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit( JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqEXOT23 )

    
#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT23SlimmingHelper = SlimmingHelper("EXOT23SlimmingHelper")

EXOT23SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                         "AntiKt4EMTopoJets_BTagging201810",
                                         "AntiKt4LCTopoJets",
                                         "AntiKt4EMPFlowJets",
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                         "BTagging_AntiKt4EMTopo_201810",
                                         "BTagging_AntiKt4EMPFlow_201810",
                                         "BTagging_AntiKt4EMPFlow_201903",
                                         "InDetTrackParticles",
                                         "PrimaryVertices"]
EXOT23SlimmingHelper.AllVariables = [ "AntiKt4EMTopoJets",
                                      "AntiKt4EMTopoJets_BTagging201810",
                                      "AntiKt4LCTopoJets",
                                      "AntiKt4EMPFlowJets",
                                      "AntiKt4EMPFlowJets_BTagging201810",
                                      "AntiKt4EMPFlowJets_BTagging201903",
                                      "BTagging_AntiKt4EMTopo_201810",
                                      "BTagging_AntiKt4EMPFlow_201810",
                                      "BTagging_AntiKt4EMPFlow_201903",
                                      "InDetTrackParticles",
                                      "PrimaryVertices",
                                      "VrtSecInclusive_SecondaryVertices",
                                      "VrtSecInclusive_SecondaryVertices_Leptons",
                                      "VrtSecInclusive_All2TrksVertices", # only filled for debug; off by default
                                      "TruthParticles",
                                      "TruthEvents",
                                      "TruthVertices",
                                      "AntiKt4TruthJets" ]
                                        

# Trigger content
EXOT23SlimmingHelper.IncludeJetTriggerContent = True

# Add origin corrected clusters -- for accessing jet constituents
addOriginCorrectedClusters(EXOT23SlimmingHelper, writeLC=True, writeEM=True)

# Add B-tagging collections to dictionary
EXOT23SlimmingHelper.AppendToDictionary = {
    'BTagging_AntiKt4EMTopo_201810':'xAOD::BTaggingContainer',
    'BTagging_AntiKt4EMPFlow_201810':'xAOD::BTaggingContainer',
    'BTagging_AntiKt4EMPFlow_201903':'xAOD::BTaggingContainer',
    'BTagging_AntiKt4EMTopoAux_201810':'xAOD:BTaggingAuxContainer',
    'BTagging_AntiKt4EMPFlowAux_201810':'xAOD:BTaggingAuxContainer',
    'BTagging_AntiKt4EMPFlowAux_201903':'xAOD:BTaggingAuxContainer'}

# Add the jet containers to the stream
addJetOutputs(EXOT23SlimmingHelper, ["EXOT23"])

EXOT23SlimmingHelper.AppendContentToStream(EXOT23Stream)
