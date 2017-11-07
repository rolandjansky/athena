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

from DerivationFrameworkJetEtMiss.TriggerLists import *
electronTriggers = singleElTriggers
muonTriggers = singleMuTriggers
photonTriggers = singlePhotonTriggers

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 400.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1)'
if DerivationFrameworkIsMonteCarlo:
  jetSelection = '(count( AntiKt10LCTopoJets.pt > 180.*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5 ) >=1)'

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHLoose)) >= 1',
                            'count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1',
                            'count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
gammatrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(photonTriggers)
gammaofflinesel = '(count(Photons.pt > 150*GeV) >= 1 && count(AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.5) >=1)'
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

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools = []

# thinning_expression = "InDetTrackParticles.pt > 0.5*GeV"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# JETM6TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "JETM6ThinningTool",
#                                                                   ThinningService         = "JETM6ThinningSvc",
#                                                                   SelectionString         = thinning_expression,
#                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")

# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
# JETM6JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM6Akt4JetTPThinningTool",
#                                                                         ThinningService         = "JETM6ThinningSvc",
#                                                                         JetKey                  = "AntiKt4EMTopoJets",
#                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                         ApplyAnd                = True)
# ToolSvc += JETM6JetTPThinningTool
# thinningTools.append(JETM6JetTPThinningTool)

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
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,jetm6Seq,"JETM6")

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm6Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM6MainKernel",
                                                          AugmentationTools = [] ,
                                                          SkimmingTools = [JETM6OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM6"] = []

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm6Seq,"JETM6")

addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=jetm6Seq, outputGroup="JETM6")

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
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
					"BTagging_AntiKt2Track"
                                        ]
JETM6SlimmingHelper.AllVariables = [
    "TruthParticles",  "TruthEvents", "TruthVertices",
    "MuonTruthParticles", "egammaTruthParticles",
    #"JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"
    "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape",
    ]

JETM6SlimmingHelper.ExtraVariables = [
    'CaloCalTopoClusters.calE.calEta.calM.calPhi.CENTER_MAG',
    'BTagging_AntiKt4EMTopo.MSV_N2Tpair.MSV_badTracksIP.MSV_energyTrkInJet.MSV_normdist.MSV_nvsec.MSV_vertices.MV1_discriminant.MV2c00_discriminant.MV2c100_discriminant.MV2c10_discriminant.MV2c20_discriminant.MV2m_pb.MV2m_pc.MV2m_pu.MultiSVbb1_discriminant.MultiSVbb2_discriminant.SV0_N2Tpair.SV1_pb.SV1_pc.SV1_pu.IP3D_pb.IP3D_pc.IP3D_pu'
    ]

JETM6SlimmingHelper.AppendToDictionary.update({"LCOriginTopoClusters":"xAOD::CaloClusterContainer",
                                               "LCOriginTopoClustersAux":"xAOD::ShallowAuxContainer",
					       "EMOriginTopoClusters":"xAOD::CaloClusterContainer",
                                               "EMOriginTopoClustersAux":"xAOD::ShallowAuxContainer"})
JETM6SlimmingHelper.ExtraVariables +=['LCOriginTopoClusters.calEta.calPhi','EMOriginTopoClusters.calE.calEta.calPhi']

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
        "AntiKt10LCTopoJets",    "AntiKt10TruthJets",
        "JETM6", # jets defined in this file
        ])
# for other containers, w set the precise variable content  in ExtraVariables

topoJetVars = 'ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.HadronConeExclTruthLabelID.HighestJVFVtx.InputType.IsoDelta2SumPt.IsoDelta3SumPt.JVF.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetOriginConstitScaleMomentum_eta.JetOriginConstitScaleMomentum_m.JetOriginConstitScaleMomentum_phi.JetOriginConstitScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.Jvt.JvtJvfcorr.JvtRpt.LArBadHVEnergyFrac.LArBadHVNCell.LArQuality.LeadingClusterCenterLambda.LeadingClusterSecondLambda.LeadingClusterSecondR.Mu12.N90Constituents.NegativeE.NumTrkPt1000.NumTrkPt500.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex.PartonTruthLabelID.PileupCorrected.SizeParameter.SumPtTrkPt1000.SumPtTrkPt500.TrackWidthPt1000.TrackWidthPt500.Width.btaggingLink.eta.pt.phi.m.GhostMuonSegmentCount.CentroidR.Timing'

JETM6SlimmingHelper.ExtraVariables += [
    'AntiKt4LCTopoJets.'+topoJetVars ,
    'AntiKt4EMTopoJets.'+topoJetVars ,
    'AntiKt4TruthJets.AlgorithmType.Angularity.Aplanarity.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.ConstituentScale.HadronConeExclTruthLabelID.InputType.IsoDelta2SumPt.IsoDelta3SumPt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.Mu12.PartonTruthLabelID.SizeParameter.Width.eta.pt.phi.m',
    "AntiKt2PV0TrackJets.AlgorithmType.ConstituentScale.Width.eta.pt.phi.m.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.OriginVertex.SizeParameter",
    'HLT_xAOD__JetContainer_a4tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
    'HLT_xAOD__JetContainer_a10tcemsubjesFS.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AlgorithmType.AverageLArQF.BchCorrCell.CentroidR.ConstituentScale.DetectorEta.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.HECFrac.HECQuality.InputType.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_m.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_m.JetEMScaleMomentum_phi.JetEMScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.LArQuality.N90Constituents.NegativeE.OriginCorrected.PileupCorrected.SizeParameter.Timing.eta.kinematics.m.phi.pt',
    ]

#JETM6SlimmingHelper.StaticContent.append("xAOD::JetContainer#CamKt15LCTopoJets")
#JETM6SlimmingHelper.StaticContent.append("xAOD::JetAuxContainer#CamKt15LCTopoJetsAux.eta.pt.phi")

# Add the MET containers to the stream
addMETOutputs(JETM6SlimmingHelper,["Diagnostic","AntiKt4LCTopo","AntiKt4EMPFlow","Track"])

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

