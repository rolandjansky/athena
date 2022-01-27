#====================================================================
# JETM3.py
# reductionConf flag JETM3 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets, addJetOutputs
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addAntiKt4LowPtJets, addRscanJets, addJetPtAssociation
#
from DerivationFrameworkJetEtMiss.METCommon import addMETTruthMap, scheduleMETAssocAlg, addMETOutputs
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

from DerivationFrameworkPhys import PhysCommon
  
#
#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
electronTriggers = TriggerLists.single_el_Trig()
muonTriggers = TriggerLists.single_mu_Trig()

#Trigger matching decorations
from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM3",
                                                 ElectronTriggers=electronTriggers,MuonTriggers=muonTriggers)

JETM3SkimmingTools = []
orstr  = ' || '
andstr = ' && '

elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium)) >= 2'])
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonPassPreselection)) >= 2'])

electronSelection = '(' + elofflinesel + ')'
muonSelection = '(' + muofflinesel + ')'

if not DerivationFrameworkIsMonteCarlo:
  eltrigsel = orstr.join(electronTriggers)
  mutrigsel = orstr.join(muonTriggers)

  electronSelection += ' && (' + eltrigsel + ')'
  muonSelection += ' && (' + mutrigsel + ')'

expression = '( (' + electronSelection + ') || (' + muonSelection + ') )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM3SkimmingTool1",
                                                                 expression = expression)
ToolSvc += JETM3SkimmingTool
JETM3SkimmingTools += [JETM3SkimmingTool]

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM3Stream )
JETM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM3Stream.AcceptAlgs(["JETM3Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM3ThinningHelper = ThinningHelper( "JETM3ThinningHelper" )
# JETM3ThinningHelper.TriggerChains = orstr.join(electronTriggers+muonTriggers)
JETM3ThinningHelper.AppendToStream( JETM3Stream )

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM3MuonTPThinningTool",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3MuonTPThinningTool
thinningTools.append(JETM3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3ElectronTPThinningTool",
                                                                               StreamName              = streamName,
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3ElectronTPThinningTool
thinningTools.append(JETM3ElectronTPThinningTool)

# TrackParticles associated with photons
JETM3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3PhotonTPThinningTool",
                                                                             StreamName              = streamName,
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3PhotonTPThinningTool
thinningTools.append(JETM3PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM3TauTPThinningTool",
                                                                        StreamName              = streamName,
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3TauTPThinningTool
thinningTools.append(JETM3TauTPThinningTool)

thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM3TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM3TPThinningTool",
                                                                  StreamName              = streamName,
                                                                  SelectionString         = thinning_expression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3TPThinningTool
thinningTools.append(JETM3TPThinningTool)


# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5 && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM3TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM3TruthThinningTool",
                                                                        StreamName              = streamName,
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)

    ToolSvc += JETM3TruthThinningTool
    thinningTools.append(JETM3TruthThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm3Seq = CfgMgr.AthSequencer("JETM3Sequence")
DerivationFrameworkJob += jetm3Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm3Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM3Kernel",
                                                                SkimmingTools = JETM3SkimmingTools,
                                                                ThinningTools = thinningTools,
                                                                AugmentationTools = [TrigMatchAug])

OutputJets["JETM3"] = []

#=======================================
# Add. small-R jet stuff in derivations
#=======================================

#=======================================
# SCHEDULE SMALL-R JETS WITH LOW PT CUT
#=======================================

if DerivationFrameworkIsMonteCarlo:
  jetList_lowPt = ["AntiKt4EMTopoJets",
                   "AntiKt4EMPFlowJets"]
  addAntiKt4LowPtJets(jetList_lowPt,DerivationFrameworkJob,"JETM3")

#====================================================================
#Jets for R-scan
#====================================================================
for radius in [0.2, 0.6]:
    if jetFlags.useTruth:
        addRscanJets("AntiKt",radius,"Truth",jetm3Seq,"JETM3")
        addRscanJets("AntiKt",radius,"TruthWZ",jetm3Seq,"JETM3")
    addRscanJets("AntiKt",radius,"LCTopo",jetm3Seq,"JETM3")

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================
if DerivationFrameworkIsMonteCarlo:
    ###addMETTruthMap('AntiKt4EMTopo',"JETMX")
    ###addMETTruthMap('AntiKt4EMPFlow',"JETMX")
    ##scheduleMETAssocAlg(jetm3Seq,"JETMX")
    ## Add GhostTruthAssociation information ##
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=DerivationFrameworkJob)
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=DerivationFrameworkJob)
    #addJetPtAssociation(jetalg="AntiKt4EMTopoLowPt",  truthjetalg="AntiKt4TruthJets", sequence=DerivationFrameworkJob)

#====================================================================
# ADD PFLOW AUG INFORMATION 
#====================================================================
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation
applyPFOAugmentation(DerivationFrameworkJob)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM3SlimmingHelper = SlimmingHelper("JETM3SlimmingHelper")
JETM3SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt2LCTopoJets", "AntiKt6LCTopoJets",
                                        "AntiKt4TruthWZJets",
                                        "AntiKt10TruthJets",
                                        "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TruthSoftDropBeta100Zcut10Jets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "BTagging_AntiKt4EMTopo",
					]
JETM3SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "LVL1JetRoIs",
                                    "JetETMissChargedParticleFlowObjects",
                                    "JetETMissNeutralParticleFlowObjects",
                                    "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
                                    ]
JETM3SlimmingHelper.ExtraVariables = [
  'HLT_xAOD__JetContainer_a4tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
  'HLT_xAOD__JetContainer_a4tcemsubjesISFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
  "Electrons."+NewTrigVars["Electrons"],
  "Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus."+NewTrigVars["Muons"],
  "AntiKt4TruthWZJets.pt","AntiKt4TruthWZJets.eta", "AntiKt4TruthWZJets.phi", "AntiKt4TruthWZJets.m"]
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM3SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM3SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM3SlimmingHelper.IncludeMuonTriggerContent = True
JETM3SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(
    slimhelper = JETM3SlimmingHelper,
    contentlist = ["SmallR","JETM3"],
    smartlist = JETM3SlimmingHelper.SmartCollections
    )

# Add the MET containers to the stream
addMETOutputs(JETM3SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track","JETM3"])
