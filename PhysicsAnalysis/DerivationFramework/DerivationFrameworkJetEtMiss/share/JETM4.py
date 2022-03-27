#====================================================================
# JETM4.py
# reductionConf flag JETM4 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets, addJetOutputs

#
from DerivationFrameworkJetEtMiss.METCommon import addMETTruthMap, scheduleMETAssocAlg, addMETOutputs
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

from DerivationFrameworkPhys import PhysCommon

#====================================================================
# SKIMMING TOOL
#====================================================================
# NOTE: need to add isSimulation as OR with trigger

JETM4SkimmingTools = []
from DerivationFrameworkJetEtMiss import TriggerLists
if not DerivationFrameworkIsMonteCarlo:
  triggerlist = TriggerLists.single_photon_Trig()

  triggers = '||'.join(triggerlist)
  expression = triggers

  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
  JETM4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM4SkimmingTool1",
                                                                      expression = expression)
  ToolSvc += JETM4SkimmingTool
  JETM4SkimmingTools += [JETM4SkimmingTool]

#Trigger matching decorations
from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM4",
                                                 PhotonTriggers=TriggerLists.single_photon_Trig())

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM4Stream )
JETM4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM4Stream.AcceptAlgs(["JETM4Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM4ThinningHelper = ThinningHelper( "JETM4ThinningHelper" )
# JETM4ThinningHelper.TriggerChains = triggers
JETM4ThinningHelper.AppendToStream( JETM4Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM4MuonTPThinningTool",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4MuonTPThinningTool
thinningTools.append(JETM4MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM4ElectronTPThinningTool",
                                                                               StreamName              = streamName,
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4ElectronTPThinningTool
thinningTools.append(JETM4ElectronTPThinningTool)

# TrackParticles associated with electrons
JETM4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM4PhotonTPThinningTool",
                                                                             StreamName              = streamName,
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4PhotonTPThinningTool
thinningTools.append(JETM4PhotonTPThinningTool)

thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM4TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM4TPThinningTool",
                                                                  StreamName              = streamName,
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4TPThinningTool
thinningTools.append(JETM4TPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM4TauTPThinningTool",
                                                                        StreamName      = streamName,
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM4TauTPThinningTool
thinningTools.append(JETM4TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM4TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM4TruthThinningTool",
                                                                        StreamName              = streamName,
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)

    ToolSvc += JETM4TruthThinningTool
    thinningTools.append(JETM4TruthThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm4Seq = CfgMgr.AthSequencer("JETM4Sequence")
DerivationFrameworkJob += jetm4Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm4Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM4Kernel" ,
                                                         SkimmingTools = JETM4SkimmingTools,
                                                         ThinningTools = thinningTools,
                                                         AugmentationTools = [TrigMatchAug])
#====================================================================
# Special jets
#====================================================================

OutputJets["JETM4"] = []

#====================================================================
# ADD PFLOW AUG INFORMATION 
#====================================================================
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation
applyPFOAugmentation(DerivationFrameworkJob)

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

# To be restored after fixing the errors caused by these lines
#if DerivationFrameworkIsMonteCarlo:
    #addMETTruthMap('AntiKt4EMTopo',"JETMX")
    #addMETTruthMap('AntiKt4EMPFlow',"JETMX")
    #scheduleMETAssocAlg(jetm4Seq,"JETMX")


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM4SlimmingHelper = SlimmingHelper("JETM4SlimmingHelper")
JETM4SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt10TruthJets",
                                        "AntiKt10LCTopoJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track"
                                        ]

JETM4SlimmingHelper.AllVariables = [# "CaloCalTopoClusters",
                                    "CHSChargedParticleFlowObjects", "CHSNeutralParticleFlowObjects",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape"]
JETM4SlimmingHelper.AppendToDictionary = {'CHSChargedParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSChargedParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                          'CHSNeutralParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSNeutralParticleFlowObjectsAux':'xAOD::ShallowAuxContainer'}

JETM4SlimmingHelper.ExtraVariables = ["CaloCalTopoClusters.calE.calEta.calPhi.calM.rawE.rawEta.rawPhi.rawM","Photons."+NewTrigVars["Photons"],"JetETMissNeutralParticleFlowObjects.m.mEM.eflowRec_TIMING.eflowRec_AVG_LAR_Q.eflowRec_CENTER_LAMBDA.pt.ptEM.phi.eta",
"JetETMissChargedParticleFlowObjects.pt.eta.phi.m.eflowRec_tracksExpectedEnergyDeposit.charge.eflowRec_isInDenseEnvironment.pfo_TrackLinks.DFCommonPFlow_z0.DFCommonPFlow_vz.DFCommonPFlow_d0.DFCommonPFlow_theta.DFCommonPFlow_envWeight",
"TauJets.truthJetLink.truthParticleLink.IsTruthMatched"]

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM4SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM4SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM4SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(
    slimhelper = JETM4SlimmingHelper,
    contentlist = [
      "SmallR",
      "JETM4"
      ],
    smartlist = JETM4SlimmingHelper.SmartCollections,
    )

# Add the MET containers to the stream
addMETOutputs(JETM4SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track",])

JETM4SlimmingHelper.AppendContentToStream(JETM4Stream)
