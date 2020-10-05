#********************************************************************
# BPHY9.py
# reductionConf flag BPHY9 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkHiggs.TruthCategories import *
from AthenaCommon.GlobalFlags import globalflags

is_MC = globalflags.DataSource()=='geant4'
print 'is_MC = ',is_MC

if is_MC:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
    from DerivationFrameworkMCTruth.HFHadronsCommon import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName  = derivationFlags.WriteDAOD_BPHY9Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_BPHY9Stream )
BPHY9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
BPHY9Stream.AcceptAlgs(['BPHY9Kernel'])

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []

#--------------------------------------------------------------------
# Jpsi vertexing Tool
#--------------------------------------------------------------------
# 1/ setup vertexing tools and services
include('DerivationFrameworkBPhys/configureVertexing.py')
BPHY9_VertexTools = BPHYVertexTools('BPHY9')

# 2/ Setup the vertex fitter tools (e.g. JpsiFinder, JpsiPlus1Track, etc).
#    These are general tools independent of DerivationFramework that do the
#    actual vertex fitting and some pre-selection.
from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
BPHY9JpsiFinder = Analysis__JpsiFinder(name                       = 'BPHY9JpsiFinder',
                                       OutputLevel                = INFO,
                                       muAndMu                    = True,
                                       muAndTrack                 = False,
                                       TrackAndTrack              = False,
                                       assumeDiMuons              = True, # If true, will assume dimu hypothesis and use PDG value for mu mass
                                       invMassUpper               = 100000.0,
                                       invMassLower               = 0.0,
                                       Chi2Cut                    = 200.,
                                       oppChargesOnly             = True,
                                       atLeastOneComb             = True,
                                       useCombinedMeasurement     = False, # Only takes effect if combOnly=True
                                       muonCollectionKey          = 'Muons',
                                       TrackParticleCollection    = 'InDetTrackParticles',
                                       V0VertexFitterTool         = BPHY9_VertexTools.TrkV0Fitter, # V0 vertex fitter
                                       useV0Fitter                = False, # if False a TrkVertexFitterTool will be used
                                       TrkVertexFitterTool        = BPHY9_VertexTools.TrkVKalVrtFitter, # VKalVrt vertex fitter
                                       TrackSelectorTool          = BPHY9_VertexTools.InDetTrackSelectorTool,
                                       ConversionFinderHelperTool = BPHY9_VertexTools.InDetConversionHelper,
                                       VertexPointEstimator       = BPHY9_VertexTools.VtxPointEstimator,
                                       useMCPCuts                 = False)
ToolSvc += BPHY9JpsiFinder
print      BPHY9JpsiFinder

# 3/ Setup the vertex reconstruction 'call' tool(s). They are part of the derivation framework. These Augmentation tools add 
#    output vertex collection(s) into the StoreGate and add basic decorations which do not depend on the vertex mass hypothesis 
#    (e.g. lxy, ptError, etc). There should be one tool per topology, i.e. Jpsi and Psi(2S) do not need two instance of the
#    Reco tool is the JpsiFinder mass window is wide enough.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_mumu
BPHY9_Reco_mumu = DerivationFramework__Reco_mumu(name                   = 'BPHY9_Reco_mumu',
                                                 JpsiFinder             = BPHY9JpsiFinder,
                                                 OutputVtxContainerName = 'BPHY9OniaCandidates',
                                                 PVContainerName        = 'PrimaryVertices',
                                                 RefPVContainerName     = 'BPHY9RefittedPrimaryVertices',
                                                 RefitPV                = True,
                                                 MaxPVrefit             = 100000,
                                                 DoVertexType           = 7)
ToolSvc += BPHY9_Reco_mumu
print BPHY9_Reco_mumu

# 4/ Setup the vertex selection and augmentation tool(s). These tools decorate the vertices with variables that depend 
#    on the vertex mass hypothesis, e.g. invariant mass, proper decay time, etc. Property HypothesisName is used as a 
#    prefix for these decorations. They also perform tighter selection, flagging the vertecis that passed. The flag is 
#    a Char_t branch named 'passed_'+HypothesisName. It is used later by the 'SelectEvent' and 'Thin_vtxTrk' tools to 
#    determine which events and candidates should be kept in the output stream. Multiple instances of the Select_* tools 
#    can be used on a single input collection as long as they use different 'HypothesisName' flags.
from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Select_onia2mumu

# 4a/ augment and select Jpsi->mumu candidates
BPHY9_Select_Jpsi2mumu = DerivationFramework__Select_onia2mumu(name                  = 'BPHY9_Select_Jpsi2mumu',
                                                               HypothesisName        = 'Jpsi',
                                                               InputVtxContainerName = 'BPHY9OniaCandidates',
                                                               VtxMassHypo           = 3096.916,
                                                               MassMin               = 2000.0,
                                                               MassMax               = 3600.0,
                                                               Chi2Max               = 200,
                                                               DoVertexType          = 7)
ToolSvc += BPHY9_Select_Jpsi2mumu
print BPHY9_Select_Jpsi2mumu

# 4b/ augment and select Psi(2S)->mumu candidates
BPHY9_Select_Psi2mumu = DerivationFramework__Select_onia2mumu(name                  = 'BPHY9_Select_Psi2mumu',
                                                              HypothesisName        = 'Psi',
                                                              InputVtxContainerName = 'BPHY9OniaCandidates',
                                                              VtxMassHypo           = 3686.09,
                                                              MassMin               = 3300.0,
                                                              MassMax               = 4500.0,
                                                              Chi2Max               = 200,
                                                              DoVertexType          = 7)
ToolSvc += BPHY9_Select_Psi2mumu
print BPHY9_Select_Psi2mumu

# 5/ select the event. We only want to keep events that contain certain vertices which passed certain selection.
#    This is specified by the 'SelectionExpression' property, which contains the expression in the following format:
#       'ContainerName.passed_HypoName > count'
#    where 'ContainerName' is output container form some Reco_* tool, 'HypoName' is the hypothesis name setup in some 'Select_*'
#    tool and 'count' is the number of candidates passing the selection you want to keep.

# Skimming
# a/ High pt lepton
ptSelection = '( count(Electrons.pt > 20*GeV) > 0 || count(Muons.pt > 20*GeV) > 0 )'
# b/ >3 total leptons
threelepSelection = '( count(Muons.pt > 0) + count(Electrons.pt > 0) >= 3 )'
# c/ di-muon vertex near Onia peak
oniaSelection = '( count(BPHY9OniaCandidates.passed_Jpsi) > 0 || count(BPHY9OniaCandidates.passed_Psi) > 0 )'
# &
expression = oniaSelection + ' && ' + threelepSelection + ' && ' + ptSelection
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
BPHY9_SelectEvent = DerivationFramework__xAODStringSkimmingTool(name       = 'BPHY9_SelectEvent',
                                                                expression = expression)
ToolSvc += BPHY9_SelectEvent

# 6/ Track and vertex thinning. We want to remove all reconstructed secondary vertices which hasn't passed any of the 
#    selections defined by (Select_*) tools. We also want to keep only tracks which are associates with either muons or 
#    any of the vertices that passed the selection. Multiple thinning tools can perform the selection. The final thinning 
#    decision is based OR of all the decisions (by default, although it can be changed by the JO).

# 6a/ Thining out vertices that didn't pass any selection and idetifying tracks associated with selected vertices. The 
#     'VertexContainerNames' is a list of the vertex containers, and 'PassFlags' contains all pass flags for Select_* 
#     tools that must be satisfied. The vertex is kept is it satisfy any of the listed selections.

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Thin_vtxTrk
BPHY9Thin_vtxTrk = DerivationFramework__Thin_vtxTrk(name                       = 'BPHY9Thin_vtxTrk',
                                                    ThinningService            = 'BPHY9ThinningSvc',
                                                    TrackParticleContainerName = 'InDetTrackParticles',
                                                    VertexContainerNames       = ['BPHY9OniaCandidates'],
                                                    PassFlags                  = ['passed_Jpsi', 'passed_Psi'] )
ToolSvc += BPHY9Thin_vtxTrk

# 6b/ thinning out tracks that are not attached to muons. The final thinning decision is based on the OR operation
#    between decision from this and the previous tools.
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = 'BPHY9MuonTPThinningTool',
                                                                         ThinningService        = 'BPHY9ThinningSvc',
                                                                         MuonKey                = 'Muons',
                                                                         InDetTrackParticlesKey = 'InDetTrackParticles')
ToolSvc += BPHY9MuonTPThinningTool

# 6c/ Only save truth informtion directly associated with Onia
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
BPHY9TruthThinTool = DerivationFramework__GenericTruthThinning(name                    = 'BPHY9TruthThinTool',
                                                               ThinningService         = 'BPHY9ThinningSvc',
                                                               ParticleSelectionString = 'TruthParticles.pdgId==443 || TruthParticles.pdgId==100443',
                                                               PreserveDescendants     = True,
                                                               PreserveAncestors       = True)
ToolSvc += BPHY9TruthThinTool
print BPHY9TruthThinTool

#==============================================================================
# BACKGROUND ELECTRON DECORATION TYPE/ORIGIN
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/trunk/src/BkgElectronClassification.cxx

if is_MC:
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    BPHY9BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = 'BkgElectronClassificationTool',
                                                                                         MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BPHY9BkgElectronClassificationTool
    augmentationTools.append(BPHY9BkgElectronClassificationTool)
    print 'BkgElectronClassificationTool: ', BPHY9BkgElectronClassificationTool

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
BPHY9ThinningHelper = ThinningHelper( 'BPHY9ThinningHelper' )

# Trigger Thinning Tool
elTrig = '^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhloose.*'\
       +'|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhmedium.*'\
       +'|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhtight.*'\
       +'|^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*lhvloose.*'
muTrig = '^(?!.*_[0-9]*(e|j|xe|tau|ht|xs|te))(?!HLT_mu.*_[0-9]*mu.*)HLT_mu.*'
BPHY9ThinningHelper.TriggerChains = elTrig + '|' + muTrig
BPHY9ThinningHelper.AppendToStream( BPHY9Stream )

# Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
BPHY9JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = 'BPHY9JetTPThinningTool',
                                                                       ThinningService        = BPHY9ThinningHelper.ThinningSvc(),
                                                                       JetKey                 = 'AntiKt4EMTopoJets',
                                                                       InDetTrackParticlesKey = 'InDetTrackParticles',
                                                                       ApplyAnd               = True)
ToolSvc += BPHY9JetTPThinningTool
thinningTools.append(BPHY9JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
BPHY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = 'BPHY9MuonTPThinningTool',
                                                                         ThinningService        = BPHY9ThinningHelper.ThinningSvc(),
                                                                         MuonKey                = 'Muons',
                                                                         InDetTrackParticlesKey = 'InDetTrackParticles')
ToolSvc += BPHY9MuonTPThinningTool
thinningTools.append(BPHY9MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
BPHY9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = 'BPHY9ElectronTPThinningTool',
                                                                               ThinningService        = BPHY9ThinningHelper.ThinningSvc(),
                                                                               SGKey                  = 'Electrons',
                                                                               InDetTrackParticlesKey = 'InDetTrackParticles')
ToolSvc += BPHY9ElectronTPThinningTool
thinningTools.append(BPHY9ElectronTPThinningTool)

#====================================================================
# Truth Thinning
#====================================================================
# Truth selection strings
truth_cond_lep_list = [
'(abs(TruthParticles.pdgId)>=11 && abs(TruthParticles.pdgId)<=14)',
'(TruthParticles.pt > 4.0*GeV)',
'(TruthParticles.status == 1)',
'(TruthParticles.barcode<200000)']
truth_cond_lep = ' && '.join(truth_cond_lep_list)
truth_cond_photon = '(abs(TruthParticles.pdgId)==22) && (TruthParticles.pt>1*GeV)'
truth_cond_comb = '('+truth_cond_lep+') || ('+truth_cond_photon+')'

# PreserveGeneratorDescendants only keeps particles that came directly from the event generator
# PreserveDescendants keeps all particles including those that come from Geant processes
BPHY9TruthTool = DerivationFramework__GenericTruthThinning(name                         = 'BPHY9TruthTool',
                                                           ThinningService              = BPHY9ThinningHelper.ThinningSvc(),
                                                           ParticleSelectionString      = truth_cond_comb,
                                                           PreserveDescendants          = True,
                                                           PreserveGeneratorDescendants = False,
                                                           PreserveAncestors            = True,
                                                           TauHandling                  = False)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
BPHY9TruthToolMenu = DerivationFramework__MenuTruthThinning(name                = 'BPHY9TruthToolMenu',
                                                            ThinningService     = BPHY9ThinningHelper.ThinningSvc(),
                                                            WritePartons        = False,
                                                            WriteHadrons        = False,
                                                            WriteBHadrons       = False,
                                                            WriteGeant          = False,
                                                            GeantPhotonPtThresh = -1.0,
                                                            WriteTauHad         = False,
                                                            PartonPtThresh      = -1.0,
                                                            WriteBSM            = False,
                                                            WriteBosons         = True,
                                                            WriteBSMProducts    = False,
                                                            WriteBosonProducts  = True,
                                                            WriteTopAndDecays   = True,
                                                            WriteEverything     = False,
                                                            WriteAllLeptons     = True,
                                                            WriteStatus3        = False,
                                                            WriteFirstN         = -1)

if is_MC:
    ToolSvc += BPHY9TruthTool
    thinningTools.append(BPHY9TruthTool)
    ToolSvc += BPHY9TruthToolMenu
    thinningTools.append(BPHY9TruthToolMenu)

#=======================================
# CREATE PRIVATE SEQUENCE  
#=======================================
BPHY9Seq = CfgMgr.AthSequencer('BPHY9Sequence')
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections=['AntiKt4EMPFlowJets'], Sequencer=BPHY9Seq)
#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
BPHY9ThinningTools = [BPHY9Thin_vtxTrk, BPHY9MuonTPThinningTool]
if is_MC:
    BPHY9ThinningTools.append(BPHY9TruthThinTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
    'BPHY9Kernel',
    AugmentationTools = [BPHY9_Reco_mumu, BPHY9_Select_Jpsi2mumu, BPHY9_Select_Psi2mumu],
    SkimmingTools = [BPHY9_SelectEvent])

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(BPHY9Seq, 'BPHY9')
# Add BDT decoration algs
BPHY9Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()
DerivationFrameworkJob += BPHY9Seq

#====================================================================
# SLIMMING TOOL
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
BPHY9SlimmingHelper = SlimmingHelper('BPHY9SlimmingHelper')

# Slimming for recontruction content
BPHY9SlimmingHelper.AllVariables = []

BPHY9SlimmingHelper.SmartCollections = ['AntiKt4EMPFlowJets',
                                        'AntiKt4EMPFlowJets_BTagging201903',
                                        'AntiKt4EMTopoJets',
                                        'AntiKt4EMTopoJets_BTagging201810',
                                        'BTagging_AntiKt4EMPFlow_201903',
                                        'BTagging_AntiKt4EMTopo_201810',
                                        'Electrons',
                                        'InDetTrackParticles',
                                        'MET_Reference_AntiKt4EMPFlow',
                                        'MET_Reference_AntiKt4EMTopo',
                                        'Muons',
                                        'PrimaryVertices']

extraJetVariables = '.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m'\
                   +'.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt'\
                   +'.HECFrac.LArQuality.HECQuality.NegativeE.AverageLArQF'

BPHY9SlimmingHelper.ExtraVariables = ['AntiKt4EMPFlowJets'+extraJetVariables,
                                      'AntiKt4EMTopoJets'+extraJetVariables,
                                      'CombinedMuonTrackParticles'+'.z0'
                                                                  +'.vz'
                                                                  +'.definingParametersCovMatrix',
                                      'Electrons'+'.author'
                                                 +'.charge',
                                      'ExtrapolatedMuonTrackParticles'+'.z0'
                                                                      +'.vz'
                                                                      +'.definingParametersCovMatrix',
                                      'GSFTrackParticles'+'.z0'
                                                         +'.vz'
                                                         +'.definingParametersCovMatrix',
                                      'Muons'+'.clusterLink'
                                             +'.allAuthors'
                                             +'.charge'
                                             +'.extrapolatedMuonSpectrometerTrackParticleLink'
                                             +'.scatteringCurvatureSignificance'
                                             +'.scatteringNeighbourSignificance',
                                      'PrimaryVertices'+'.x'
                                                       +'.y']

BPHY9Stream.StaticContent = []

# Slimming for truth content
if is_MC:
  BPHY9SlimmingHelper.AllVariables += ['TruthParticles',
                                       'TruthEvents',
                                       'TruthVertices']

  BPHY9SlimmingHelper.SmartCollections += ['AntiKt4TruthJets',
                                           'AntiKt4TruthWZJets']

  BPHY9SlimmingHelper.ExtraVariables += ['CombinedMuonTrackParticles'+'.truthOrigin'
                                                                     +'.truthType'
                                                                     +'.truthParticleLink',
                                         'Electrons'+'.truthOrigin'
                                                    +'.truthType'
                                                    +'.truthParticleLink'
                                                    +'.bkgTruthType'
                                                    +'.bkgTruthOrigin'
                                                    +'.bkgTruthParticleLink'
                                                    +'.bkgMotherPdgId'
                                                    +'.deltaPhi1',
                                         'InDetTrackParticles'+'.truthOrigin'
                                                              +'.truthType'
                                                              +'.truthParticleLink',
                                         'MuonTruthParticles'+'.truthOrigin'
                                                             +'.truthType'
                                                             +'.truthParticleLink']

  BPHY9SlimmingHelper.StaticContent += ['xAOD::TruthParticleContainer#TruthMuons',
                                        'xAOD::TruthParticleAuxContainer#TruthMuonsAux.',
                                        'xAOD::TruthParticleContainer#TruthElectrons',
                                        'xAOD::TruthParticleAuxContainer#TruthElectronsAux.',
                                        'xAOD::TruthParticleContainer#TruthNeutrinos',
                                        'xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.']

# Slimming for trigger content
BPHY9SlimmingHelper.IncludeEGammaTriggerContent = True
BPHY9SlimmingHelper.IncludeMuonTriggerContent = True

# Slimming for charmonia content
BPHY9Stream.AddItem('xAOD::VertexContainer#BPHY9OniaCandidates')
BPHY9Stream.AddItem('xAOD::VertexAuxContainer#BPHY9OniaCandidatesAux.')
BPHY9Stream.AddItem('xAOD::VertexAuxContainer#BPHY9OniaCandidatesAux.-vxTrackAtVertex')

BPHY9SlimmingHelper.AppendContentToStream(BPHY9Stream)