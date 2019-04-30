#====================================================================
# JETM6.py
# reductionConf flag JETM6 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkJetEtMiss.METCommon import *
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
  import DerivationFrameworkCore.WeightMetadata
  import DerivationFrameworkCore.LHE3WeightMetadata

#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkJetEtMiss import TriggerLists
electronTriggers = TriggerLists.single_el_Trig()
muonTriggers = TriggerLists.single_mu_Trig()
photonTriggers = TriggerLists.single_photon_Trig()
jetTriggers = TriggerLists.jetTrig()

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 400.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10TrackCaloClusterJets.pt > 400.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >= 1)'
if DerivationFrameworkIsMonteCarlo:
  jetSelection = '(count( AntiKt10LCTopoJets.pt > 180.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1 || count( AntiKt10TrackCaloClusterJets.pt > 180.*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5 ) >= 1 )'

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHLoose)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1)'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1',
                            '(count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1)'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
gammatrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(photonTriggers)
gammaofflinesel = '(count(Photons.pt > 150*GeV) >= 1 && (count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1 || count(AntiKt10TrackCaloClusterJets.pt > 150*GeV && abs(AntiKt10TrackCaloClusterJets.eta) < 2.5) >= 1))'
photonSelection = ' ( (' + gammatrigsel + ') && (' + gammaofflinesel + ') ) '
# MET filter wanted? : MET_Reference_AntiKt4LCTopo > 20*GeV # should use a different container

lepSelection = '( ' + electronSelection + ' || ' + muonSelection + ' || ' + photonSelection + ' )'


expression = jetSelection + ' || '+ lepSelection


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM6TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM6TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers+electronTriggers+muonTriggers+photonTriggers )
                                                                    #TriggerListOR  = triggers )
ToolSvc += JETM6TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM6OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM6OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += JETM6OfflineSkimmingTool

#Trigger matching decorations
from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="JETM6",
                                   ElectronTriggers=electronTriggers,
                                   MuonTriggers=muonTriggers,
                                   PhotonTriggers=photonTriggers)

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4JetTPThinningTool",
                                                                        ThinningService         = "JETM6ThinningSvc",
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM6Akt4JetTPThinningTool
thinningTools.append(JETM6Akt4JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM6Akt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetTPThinningTool",
                                                                        ThinningService         = "JETM6ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM6Akt10JetTPThinningTool
thinningTools.append(JETM6Akt10JetTPThinningTool)

JETM6Akt10JetTPThinningToolTCC = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt10JetTPThinningToolTCC",
                                                                                ThinningService         = "JETM6ThinningSvc",
                                                                                JetKey                  = "AntiKt10TrackCaloClusterJets",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ApplyAnd                = False)
ToolSvc += JETM6Akt10JetTPThinningToolTCC
thinningTools.append(JETM6Akt10JetTPThinningToolTCC)


# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM6MuonTPThinningTool",
                                                                         ThinningService         = "JETM6ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6MuonTPThinningTool
thinningTools.append(JETM6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6ElectronTPThinningTool",
                                                                               ThinningService         = "JETM6ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6ElectronTPThinningTool
thinningTools.append(JETM6ElectronTPThinningTool)

# TrackParticles associated with photons
JETM6PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM6PhotonTPThinningTool",
                                                                             ThinningService         = "JETM6ThinningSvc",
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6PhotonTPThinningTool
thinningTools.append(JETM6PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM6TauTPThinningTool",
                                                                        ThinningService = "JETM6ThinningSvc",
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM6TauTPThinningTool
thinningTools.append(JETM6TauTPThinningTool)

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

  truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+' || '+truth_cond_Gluon+' || '+truth_cond_Photon+')'

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  JETM6TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM6TruthThinningTool",
                                                                      ThinningService        = "JETM6ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      #PreserveDescendants     = preserveAllDescendants,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                      #PreserveGeneratorDescendants = False,
                                                                      PreserveAncestors = True)

  ToolSvc += JETM6TruthThinningTool
  thinningTools.append(JETM6TruthThinningTool)

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  JETM6TruthTopThinningTool = DerivationFramework__MenuTruthThinning( name                       = "JETM6TruthTopThinningTool",
 	                                                           ThinningService            = "JETM6ThinningSvc",
 		                                                   WritePartons               = True,
 		                                                   WriteHadrons               = False,
 		                                                   WriteBHadrons              = True,
 		                                                   WriteGeant                 = False,
 		                                                   GeantPhotonPtThresh        = -1.0,
 		                                                   WriteTauHad                = False,
 		                                                   PartonPtThresh             = 10e3,
 		                                                   WriteBSM                   = False,
 		                                                   WriteBosons                = True,
 		                                                   PhotonPtThresh             = 10e3,
 		                                                   WriteBSMProducts           = False,
 		                                                   WriteTopAndDecays          = True,
 		                                                   WriteEverything            = False,
 		                                                   WriteAllLeptons            = False,
 		                                                   WriteLeptonsNotFromHadrons = True,
 		                                                   WriteStatus3               = False,
 		                                                   WriteFirstN                = 10,
 		                                                   PreserveDescendants        = preserveAllDescendants,
 		                                                   PreserveAncestors          = True)  

  ToolSvc += JETM6TruthTopThinningTool
  thinningTools.append(JETM6TruthTopThinningTool)    
#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm6Seq = CfgMgr.AthSequencer("JETM6Sequence")
DerivationFrameworkJob += jetm6Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM6TrigSkimKernel",
                                                            AugmentationTools = [] ,
                                                            SkimmingTools = [JETM6TrigSkimmingTool],
                                                            ThinningTools = [])

#======================================= 
# RECONSTRUCT TCC JETS
#======================================= 

from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(jetm6Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles")

#=======================================
# BTAGGING INFO FOR PFLOW JET
#=======================================
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'],Sequencer = jetm6Seq)


#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TrackCaloClusterJets"]
replaceAODReducedJets(reducedJetList,jetm6Seq,"JETM6")

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM6MainKernel",
                                                          AugmentationTools = [TrigMatchAug] ,
                                                          SkimmingTools = [JETM6OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM6"] = []

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm6Seq,"JETM6")
addTCCTrimmedJets(jetm6Seq,"JETM6")

addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=jetm6Seq, outputGroup="JETM6")

if DerivationFrameworkIsMonteCarlo:
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=jetm6Seq, outputGroup="JETM6", writeUngroomed=True)

addCSSKSoftDropJets(jetm6Seq, "JETM6")

addSoftDropJets("AntiKt", 1.0, "PV0Track", beta=1.0, zcut=0.1, algseq=jetm6Seq, outputGroup="JETM6")

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM6Stream )
JETM6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM6Stream.AcceptAlgs(["JETM6MainKernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM6ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM6SlimmingHelper = SlimmingHelper("JETM6SlimmingHelper")
JETM6SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        #
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
					"BTagging_AntiKt2Track",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt4EMPFlow"
                                        ]
JETM6SlimmingHelper.AllVariables = [
    "TruthParticles",  "TruthEvents", "TruthVertices",
    "MuonTruthParticles", "egammaTruthParticles",
    #"JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"
    "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
    ]

JETM6SlimmingHelper.ExtraVariables = [
    'CaloCalTopoClusters.calE.calEta.calM.calPhi.CENTER_MAG', NewTrigVars["Electrons"][0],NewTrigVars["Muons"][0],NewTrigVars["Photons"][0]]

JETM6SlimmingHelper.AppendToDictionary = {
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"   :   "xAOD::JetContainer"        ,  
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux":   "xAOD::JetAuxContainer"        ,
}
JETM6SlimmingHelper.AllVariables  += ["AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"]

if DerivationFrameworkIsMonteCarlo:
  JETM6SlimmingHelper.AppendToDictionary = {
    "AntiKt10TruthSoftDropBeta100Zcut10Jets"   :   "xAOD::JetContainer"        ,
    "AntiKt10TruthSoftDropBeta100Zcut10JetsAux":   "xAOD::JetAuxContainer"        ,
  }
  JETM6SlimmingHelper.AllVariables  += ["AntiKt10TruthSoftDropBeta100Zcut10Jets"]

addOriginCorrectedClusters(JETM6SlimmingHelper,writeLC=True,writeEM=True)

#JETM6SlimmingHelper.ExtraVariables = []
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
    "TruthNeutrinos"
    ]:
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM6SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

## # Trigger content
## from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent
JETM6SlimmingHelper.IncludeJetTriggerContent = True
JETM6SlimmingHelper.IncludeMuonTriggerContent = True
JETM6SlimmingHelper.IncludeEGammaTriggerContent = True
# We actually set the precise variable content  in ExtraVariables

# Add the jet containers to the stream
# explicitely add the container we want :
addJetOutputs(JETM6SlimmingHelper,[
        "AntiKt10LCTopoJets",    "AntiKt10TruthJets", "AntiKt10TrackCaloClusterJets",
        "JETM6", # jets defined in this file
        ])
# for other containers, w set the precise variable content  in ExtraVariables

addTopoJetVars = 'ActiveArea.AlgorithmType.AverageLArQF.BchCorrCell.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConstituentScale.EMFrac.HECFrac.HECQuality.HighestJVFVtx.InputType.IsoDelta2SumPt.IsoDelta3SumPt.JVF.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.JvtJvfcorr.LArBadHVEnergyFrac.LArBadHVNCell.LArQuality.LeadingClusterCenterLambda.LeadingClusterSecondLambda.LeadingClusterSecondR.Mu12.N90Constituents.NegativeE.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex.PileupCorrected.SizeParameter.SumPtTrkPt1000.TrackWidthPt500.Width.CentroidR'

JETM6SlimmingHelper.ExtraVariables += [
    'AntiKt4EMTopoJets.'+addTopoJetVars ,
    'AntiKt4TruthJets.AlgorithmType.Angularity.Aplanarity.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.ConstituentScale.HadronConeExclTruthLabelID.InputType.IsoDelta2SumPt.IsoDelta3SumPt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.Mu12.PartonTruthLabelID.SizeParameter.Width.eta.pt.phi.m',
    "AntiKt2PV0TrackJets.AlgorithmType.ConstituentScale.Width.eta.pt.phi.m.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.OriginVertex.SizeParameter",
    'HLT_xAOD__JetContainer_a4tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a10tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
    "Electrons."+NewTrigVars["Electrons"],
    "Muons."+NewTrigVars["Muons"],
    "Photons."+NewTrigVars["Photons"]
    ]

# Add the MET containers to the stream
addMETOutputs(JETM6SlimmingHelper,["Diagnostic","AntiKt4EMPFlow","Track"])

JETM6SlimmingHelper.AppendContentToStream(JETM6Stream)
#JETM6Stream.RemoveItem("xAOD::TrigNavigation#*")
#JETM6Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")



def removeVars(coll, vars):
    """Terrible hack to force removing a limited set of variables.
    Intended to remove definingParametersCovMatrix if possible.. """
    items = JETM6Stream.GetItems()
    origS = None
    for i in items:
        if coll in i and '.' in i:
            origS = i
            break
    if origS is None:
        return
    existV = origS.split('.')
    cleanedV = []
    for v in existV:
        if v in vars:
            continue
        cleanedV.append(v)
    newS = '.'.join(cleanedV)
    JETM6Stream.RemoveItem( origS )
    JETM6Stream.AddItem( newS )

#removeVars('InDetTrackParticles', ['definingParametersCovMatrix',])

