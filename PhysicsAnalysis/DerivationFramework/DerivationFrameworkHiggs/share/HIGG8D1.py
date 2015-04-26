#********************************************************************
# HIGG8D1.py 
# reductionConf flag HIGG8D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

# testing globalflags
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC


#====================================================================
# TAU SELECTOR TOOL 
#====================================================================

from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauSelectionWrapper
HIGG8D1TauWrapper = DerivationFramework__TauSelectionWrapper(name = "HIGG8D1TauSelectionWrapper",
                                                             IsTauFlag			= 19,
                                                             CollectionName		= "TauJets",
                                                             StoreGateEntryName	= "HIGG8D1JetBDTSigLoose")
ToolSvc += HIGG8D1TauWrapper


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG8D1MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG8D1MetTPThinningTool",
                                                                      ThinningService        = "HIGG8D1ThinningSvc",
                                                                      SelectionString        = thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG8D1MetTPThinningTool
thinningTools.append(HIGG8D1MetTPThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG8D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG8D1JetTPThinningTool",
                                                                ThinningService         = "HIGG8D1ThinningSvc",
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG8D1JetTPThinningTool
thinningTools.append(HIGG8D1JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG8D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG8D1MuonTPThinningTool",
                                                                           ThinningService         = "HIGG8D1ThinningSvc",
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1MuonTPThinningTool
thinningTools.append(HIGG8D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG8D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG8D1ElectronTPThinningTool",
                                                                                 ThinningService         = "HIGG8D1ThinningSvc",
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG8D1ElectronTPThinningTool
thinningTools.append(HIGG8D1ElectronTPThinningTool)

# Electrons - calo cluster thinning
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__CaloClusterThinning
HIGG8D1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "HIGG8D1ElectronCCThinningTool",
                                                                          ThinningService         = "HIGG8D1ThinningSvc",
                                                                          SGKey                   = "Electrons",
                                                                          CaloClCollectionSGKey   = "egammaClusters",
                                                                          SelectionString         = "Electrons.pt >= 5*GeV",
                                                                          ConeSize                = 0.4)
ToolSvc += HIGG8D1ElectronCCThinningTool
thinningTools.append(HIGG8D1ElectronCCThinningTool)


# Truth Thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
HIGG8D1TruthTool = DerivationFramework__MenuTruthThinning(name                       = "HIGG8D1TruthTool",
                                                          ThinningService            = "HIGG8D1ThinningSvc",
                                                          WritePartons               = False,
                                                          WriteHadrons               = False,
                                                          WriteBHadrons              = False,
                                                          WriteGeant                 = False,
                                                          GeantPhotonPtThresh        = -1.0,
                                                          WriteTauHad                = True,
                                                          PartonPtThresh             = -1.0,
                                                          WriteBSM                   = False,
                                                          WriteBosons                = True,
                                                          WriteBSMProducts           = False,
                                                          WriteBosonProducts         = True,
                                                          WriteTopAndDecays          = True,
                                                          WriteEverything            = False,
                                                          WriteAllLeptons            = True,
                                                          WriteStatus3               = False,
                                                          WriteFirstN                = -1)
if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG8D1TruthTool
    thinningTools.append(HIGG8D1TruthTool)


#====================================================================
# SKIMMING TOOL 
#====================================================================

#to add: MC event info requirements

eleRequirements = '(DFCommonElectrons_pt > 5*GeV) && (abs(DFCommonElectrons_eta) < 2.6) && (Electrons.Loose || Electrons.DFCommonElectronsLHLoose)'
leadElectron = eleRequirements + ' && (Electrons.pt > 15*GeV)'
muRequirements = '(Muons.DFCommonGoodMuon) && (Muons.muonType == 0) && (Muons.pt > 5*GeV) && (abs(Muons.eta) < 2.6)'
leadMuon = muRequirements + ' && (Muons.pt > 15*GeV)'
tauRequirements = '(TauJets.pt > 15*GeV) && HIGG8D1JetBDTSigLoose && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3))'

eeSelection = '((count('+eleRequirements+') >= 2) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muRequirements+') >= 2) && (count('+leadMuon+') >= 1))'
emSelection = '(((count('+eleRequirements+') >= 1) && (count('+muRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'

lepSelection = eeSelection+' || '+mmSelection+' || '+emSelection
tauSelection = '((count('+leadElectron+')+count('+leadMuon+') >= 1) && (count('+tauRequirements+') >= 2))'

expression = lepSelection+ ' || ' +tauSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG8D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG8D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG8D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG8D1Kernel",
									ThinningTools = thinningTools,
									AugmentationTools = [HIGG8D1TauWrapper],
									SkimmingTools = [HIGG8D1SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG8D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG8D1Stream )
HIGG8D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG8D1Stream.AcceptAlgs(["HIGG8D1Kernel"])
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG8D1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG8D1SlimmingHelper = SlimmingHelper("HIGG8D1SlimmingHelper")

## Smart Slimming
HIGG8D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Muons",
                                          "TauJets",
                                          #"MET_Reference_AntiKt4LCTopo",
                                          #"AntiKt4LCTopoJets",
                                          #"BTagging_AntiKt4LCTopo",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG8D1SlimmingHelper.ExtraVariables = ["Muons.clusterLink.allAuthors.charge.extrapolatedMuonSpectrometerTrackParticleLink",
                                        "Electrons.author.charge",
                                        "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID.SumPtTrkPt1000.Jvt.JvtJvfcorr.JvtRpt",
                                        "GSFTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix"
                                        ]

HIGG8D1SlimmingHelper.AllVariables = ["egammaClusters"]
#HIGG8D1SlimmingHelper.AllVariables = ["egammaClusters","CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles",
#"GSFTrackParticles"]

if globalflags.DataSource()=='geant4':
    HIGG8D1SlimmingHelper.AllVariables += ["TruthParticles", "TruthEvents","TruthVertices","AntiKt4TruthJets"]
    HIGG8D1SlimmingHelper.ExtraVariables += ["Electrons.truthOrigin.truthType.truthParticleLink",
                                             "MuonTruthParticles.truthOrigin.truthType"]

#HIGG8D1SlimmingHelper.IncludeMuonTriggerContent=True
#HIGG8D1SlimmingHelper.IncludeEGammaTriggerContent=True
 
HIGG8D1SlimmingHelper.AppendContentToStream(HIGG8D1Stream)

# Add MET_RefFinalFix
#addMETOutputs(HIGG8D1Stream)

# Add TOP dedicated MET (LHElectrons)
#from DerivationFrameworkJetEtMiss.TopCustomMET import *
#addMETOutputs(HIGG8D1Stream,["Top"])
