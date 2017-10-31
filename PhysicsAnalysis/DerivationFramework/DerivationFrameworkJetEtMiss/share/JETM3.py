m#====================================================================
# JETM3.py
# reductionConf flag JETM3 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.PFlowCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#
#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss.TriggerLists import *
electronTriggers = singleElTriggers+multiElTriggers
muonTriggers = singleMuTriggers+multiMuTriggers

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium)) >= 2'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 2'])
muonSelection = '( (' + mutrigsel + ') && (' + muofflinesel + ') )'
expression = '( ' + electronSelection + ' || ' + muonSelection + ' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM3SkimmingTool

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
                                                                    ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3MuonTPThinningTool
thinningTools.append(JETM3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3ElectronTPThinningTool",
                                                                               ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3ElectronTPThinningTool
thinningTools.append(JETM3ElectronTPThinningTool)

# TrackParticles associated with photons
JETM3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM3PhotonTPThinningTool",
                                                                             ThinningService         = JETM3ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3PhotonTPThinningTool
thinningTools.append(JETM3PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM3TauTPThinningTool",
                                                                        ThinningService = JETM3ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM3TauTPThinningTool
thinningTools.append(JETM3TauTPThinningTool)

thinning_expression = "( abs(InDetTrackParticles.d0) < 2 ) && ( abs(DFCommonInDetTrackZ0AtPV*sin(InDetTrackParticles.theta)) < 3 )"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM3TPThinningTool = DerivationFramework__TrackParticleThinning( name                = "JETM3TPThinningTool",
                                                                  ThinningService         = JETM3ThinningHelper.ThinningSvc(),
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
                                                                        ThinningService        = JETM3ThinningHelper.ThinningSvc(),
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
                                                                SkimmingTools = [JETM3SkimmingTool],
                                                                ThinningTools = thinningTools)
# PFlow augmentation
applyPFOAugmentation(jetm3Seq)

OutputJets["JETM3"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,jetm3Seq,"JETM3")
addDefaultTrimmedJets(jetm3Seq,"JETM3")

#=======================================
# SCHEDULE SMALL-R JETS WITH LOW PT CUT
#=======================================

addAntiKt4LowPtJets(jetm3Seq,"JETM3")

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
    addMETTruthMap('AntiKt4EMTopo',"JETMX")
    addMETTruthMap('AntiKt4LCTopo',"JETMX")
    addMETTruthMap('AntiKt4EMPFlow',"JETMX")
    scheduleMETAssocAlg(jetm3Seq,"JETMX")
    ## Add GhostTruthAssociation information ##
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4LCTopo",  truthjetalg="AntiKt4TruthJets", sequence=jetm3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow", truthjetalg="AntiKt4TruthJets", sequence=jetm3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMTopoLowPt",  truthjetalg="AntiKt4TruthJets", sequence=jetm3Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4LCTopoLowPt",  truthjetalg="AntiKt4TruthJets", sequence=jetm3Seq, algname="JetPtAssociationAlg")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM3SlimmingHelper = SlimmingHelper("JETM3SlimmingHelper")
JETM3SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt2LCTopoJets", "AntiKt6LCTopoJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "BTagging_AntiKt4EMTopo",
					]
JETM3SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "JetETMissChargedParticleFlowObjects",
                                    "JetETMissNeutralParticleFlowObjects",
                                    "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape",
                                    ]
JETM3SlimmingHelper.ExtraVariables = ["Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus",
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
addJetOutputs(JETM3SlimmingHelper,["SmallR","JETM3"],["AntiKt4TruthWZJets","AntiKt2LCTopoJets","AntiKt6LCTopoJets"])
# Add the MET containers to the stream
addMETOutputs(JETM3SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track","JETM3"])

JETM3SlimmingHelper.AppendContentToStream(JETM3Stream)
