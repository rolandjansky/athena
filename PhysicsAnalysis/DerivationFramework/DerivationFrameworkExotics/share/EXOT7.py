#********************************************************************
# EXOT7.py 
# reductionConf flag EXOT7 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from JetRec.JetRecStandard import jtm
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#from DerivationFrameworkExotics.EXOTMuonsCommon import *

augTools = []
if DerivationFrameworkIsMonteCarlo:
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
   addStandardTruthContents()
   from DerivationFrameworkMCTruth.HFHadronsCommon import *
   from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
   EXOT7Classifier = MCTruthClassifier( name                      = "EXOT7Classifier",
                                       ParticleCaloExtensionTool = "" ) 
   ToolSvc += EXOT7Classifier
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
   EXOT7ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(
                                 name              = "EXOT7ClassificationDecorator",
                                 ParticlesKey      = "TruthParticles",
                                 MCTruthClassifier = EXOT7Classifier) 
   ToolSvc += EXOT7ClassificationDecorator
   augTools.append(EXOT7ClassificationDecorator)
   from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
   from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
   BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (
                                    name = "EXOT7BkgElectronClassificationTool",
                                    MCTruthClassifierTool = BkgElectronMCTruthClassifier)
   ToolSvc += BkgElectronClassificationTool
   augTools.append(BkgElectronClassificationTool)

exot7Seq = CfgMgr.AthSequencer("EXOT7Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT7Stream )
EXOT7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT7Stream.AcceptAlgs(["EXOT7Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================
#Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT7ThinningHelper = ThinningHelper( "EXOT7ThinningHelper" )

#trigger navigation content
EXOT7ThinningHelper.TriggerChains = 'HLT_[0-9]*j.*|HLT_xe.*'
EXOT7ThinningHelper.AppendToStream( EXOT7Stream )

#=====================
# SETUP
#=====================
from DerivationFrameworkCore.WeightMetadata import *

from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

#set MC flag
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

#run GenFilterTool
if globalflags.DataSource() == 'geant4':
  from DerivationFrameworkMCTruth.GenFilterToolSetup import *
  augTools.append(ToolSvc.DFCommonTruthGenFilt)

from DerivationFrameworkExotics.JetDefinitions import *

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT7"] = []
reducedJetList = [
  "AntiKt2PV0TrackJets", #flavour-tagged automatically
  "AntiKt4PV0TrackJets",
  "AntiKt4TruthJets",
  "AntiKt4TruthWZJets",
  "AntiKt10TruthJets",
  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot7Seq,"EXOT7")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot7Seq,"EXOT7")

#some jets collections are not included in the new jet restoring mechanism and need to be added the old way
#addStandardJets("CamKt", 1.5, "PV0Track", mods = "track_ungroomed", algseq = exot7Seq, outputGroup = "EXOT7")#CamKt15PV0TrackJets #FIX #ATLJETMET-744
#addStandardJets("CamKt", 1.5, "LCTopo", mods = "calib_notruth", algseq = exot7Seq, outputGroup = "EXOT7") #CamKt15LCTopoJets #FIX #ATLJETMET-744

#jet calibration
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot7Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot7Seq)


#=======================================
# SKIMMING   
#=======================================

#====================================================================
# OPENING ANGLE TOOL
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
EXOT7DeltaRTool = DerivationFramework__DeltaRTool( name = "EXOT7DeltaRTool",
                                                   StoreGateEntryName = "dRkt4kt10",
                                                   ObjectRequirements = "(AntiKt4LCTopoJets.pt > 300*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)",
                                                   SecondObjectRequirements = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 300*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)",
                                                   ContainerName = "AntiKt4LCTopoJets",
                                                   SecondContainerName = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                 )
ToolSvc += EXOT7DeltaRTool

expression  = '((((count((AntiKt4LCTopoJets.pt > 300*GeV) && (abs(AntiKt4LCTopoJets.eta)<2.7)) >= 1)) && (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>300*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)) >0 ) && (count(dRkt4kt10 > 1.0) >= 1))'

# for ZinvThad/monotop (begin)
expression += ' || ( (count(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>250*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.2)) >= 1 ) && (HLT_xe70 || HLT_j80_xe80 || HLT_xe90_mht_L1XE50 || HLT_xe100_mht_L1XE50 || HLT_xe110_mht_L1XE50 || HLT_xe90_tc_lcw_L1XE50 || HLT_noalg_L1J400) )'
expression += ' || ( (count(AntiKt4LCTopoJets.pt>45*GeV && (abs(AntiKt4LCTopoJets.eta)<2.7)) >= 2 ) && (HLT_xe70 || HLT_j80_xe80 || HLT_xe90_mht_L1XE50 || HLT_xe100_mht_L1XE50 || HLT_xe110_mht_L1XE50 || HLT_xe90_tc_lcw_L1XE50 || HLT_noalg_L1J400) )'

# for ZinvThad/monotop (end)
#expression += ' || ( ( ( (count((CamKt15LCTopoJets.pt > 400*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 1) && (count((CamKt15LCTopoJets.pt > 150*GeV) && (abs(CamKt15LCTopoJets.eta) < 2.7)) >= 2) ) )'#FIX #ATLJETMET-744

# AND of lepton veto for tt res. all had (note the lepton p_T cuts are higher, to make the veto softer and allow for calibrations)
#expression += ' && !(((count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.4 && Electrons.eta < 2.4)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.4 && Muons.eta < 2.4)>0))) ))'#FIX #ATLJETMET-744
expression += '|| !(((count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.4 && Electrons.eta < 2.4)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.4 && Muons.eta < 2.4)>0))) )'#FIX #ATLJETMET-744
#expression += ' && !(((HLT_e28_tight_iloose || HLT_e60_medium) && (count(Electrons.Tight && Electrons.pt > 30*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( (HLT_mu26_imedium || HLT_mu50) && count(Muons.DFCommonGoodMuon && Muons.pt > 30*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0))) ))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT7StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT7StringSkimmingTool", expression = expression)

ToolSvc += EXOT7StringSkimmingTool
print EXOT7StringSkimmingTool

# define thinning tool 
thinningTools=[]

# tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT7MuonTPThinningTool",
                                                                         ThinningService         = EXOT7ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         SelectionString         = "Muons.pt > 7*GeV",
                                                                         ConeSize                = 0.4)
ToolSvc += EXOT7MuonTPThinningTool
thinningTools.append(EXOT7MuonTPThinningTool)

# tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT7ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT7ElectronTPThinningTool",
                                                                               ThinningService         = EXOT7ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                               SelectionString         = "Electrons.pt > 7*GeV",
                                                                               ConeSize                = 0.4)
ToolSvc += EXOT7ElectronTPThinningTool
thinningTools.append(EXOT7ElectronTPThinningTool)

# calo cluster thinning for electrons only (only affects egClusterCollection)
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT7ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name                    = "EXOT7ElectronCCThinningTool",
                                                                       ThinningService         = EXOT7ThinningHelper.ThinningSvc(),
                                                                       SGKey                   = "Electrons",
                                                                       CaloClCollectionSGKey   = "egammaClusters",
                                                                       #TopoClCollectionSGKey   = "CaloCalTopoCluster",
                                                                       SelectionString         = "Electrons.pt > 7*GeV",
                                                                       #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                       ConeSize                = 0.4)
ToolSvc += EXOT7ElectronCCThinningTool
thinningTools.append(EXOT7ElectronCCThinningTool)

#FIX #ATLJETMET-744
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
#EXOT7CA15CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT7CA15CCThinningTool",
#                                                                       ThinningService         = EXOT7ThinningHelper.ThinningSvc(),
#                                                                       SGKey                   = "CamKt15LCTopoJets",
#                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV",
#                                                                       ConeSize                = 0)
#ToolSvc += EXOT7CA15CCThinningTool
#thinningTools.append(EXOT7CA15CCThinningTool)

EXOT7A10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT7A10CCThinningTool",
                                                                       ThinningService         = EXOT7ThinningHelper.ThinningSvc(),
                                                                       SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT7A10CCThinningTool
thinningTools.append(EXOT7A10CCThinningTool)


from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT7MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7MCThinningTool",
                                                             ThinningService = EXOT7ThinningHelper.ThinningSvc(),
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = True,
	                                                     WriteBHadrons              = True,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = True,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = False,
	                                                     WriteBosons                = False,
	                                                     WriteBSMProducts           = False,
	                                                     WriteTopAndDecays          = False,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             WritettHFHadrons           = True,
                                                             PreserveDescendants        = False)

EXOT7TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7TMCThinningTool",
                                                             ThinningService = EXOT7ThinningHelper.ThinningSvc(),
	                                                     WritePartons               = False,
	                                                     WriteHadrons               = False,
	                                                     WriteBHadrons              = False,
	                                                     WriteGeant                 = False,
	                                                     GeantPhotonPtThresh        = -1.0,
	                                                     WriteTauHad                = False,
	                                                     PartonPtThresh             = -1.0,
	                                                     WriteBSM                   = True,
	                                                     WriteBosons                = True,
	                                                     WriteBSMProducts           = True,
	                                                     WriteTopAndDecays          = True,
	                                                     WriteEverything            = False,
	                                                     WriteAllLeptons            = False,
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = -1,
                                                             PreserveDescendants        = True)

EXOT7TAMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT7TAMCThinningTool",
                                                             ThinningService = EXOT7ThinningHelper.ThinningSvc(),
                                                             WritePartons               = False,
                                                             WriteHadrons               = False,
                                                             WriteBHadrons              = False,
                                                             WriteGeant                 = False,
                                                             GeantPhotonPtThresh        = -1.0,
                                                             WriteTauHad                = False,
                                                             PartonPtThresh             = -1.0,
                                                             WriteBSM                   = False,
                                                             WriteBosons                = False,
                                                             WriteBSMProducts           = False,
                                                             WriteTopAndDecays          = True,
                                                             WriteEverything            = False,
                                                             WriteAllLeptons            = False,
                                                             WriteLeptonsNotFromHadrons         = False,
                                                             WriteStatus3               = False,
                                                             WriteFirstN                = 10,
                                                             PreserveDescendants        = False,
                                                             PreserveAncestors          = True)

#
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT7MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT7MCGenThinningTool",
                                                                ThinningService = EXOT7ThinningHelper.ThinningSvc(),
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
                                                                PreserveDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT7MCThinningTool
    thinningTools.append(EXOT7MCThinningTool)
    ToolSvc += EXOT7TMCThinningTool
    thinningTools.append(EXOT7TMCThinningTool)
    ToolSvc += EXOT7TAMCThinningTool
    thinningTools.append(EXOT7TAMCThinningTool)
    ToolSvc += EXOT7MCGenThinningTool
    thinningTools.append(EXOT7MCGenThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot7Seq
exot7Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT7Kernel_skim",
                                                         AugmentationTools = [EXOT7DeltaRTool],
                                                         SkimmingTools = [EXOT7StringSkimmingTool])
exot7Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT7Kernel",
                                                         ThinningTools = thinningTools,
                                                         AugmentationTools = augTools)


# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT7ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT7ContentList import *
EXOT7SlimmingHelper = SlimmingHelper("EXOT7SlimmingHelper")
EXOT7SlimmingHelper.StaticContent = EXOT7Content

EXOT7SlimmingHelper.SmartCollections = EXOT7SmartCollections
EXOT7SlimmingHelper.ExtraVariables = EXOT7ExtraVariables
EXOT7SlimmingHelper.AllVariables = EXOT7AllVariables
#EXOT7SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
addMETOutputs(EXOT7SlimmingHelper, ["Track", "EXOT7"], ["AntiKt4EMTopo"])

addJetOutputs(EXOT7SlimmingHelper, ["EXOT7"])

EXOT7SlimmingHelper.AppendToDictionary = {}
#listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets', 'CamKt15LCTopoJets']#FIX #ATLJETMET-744
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']#FIX #ATLJETMET-744
if globalflags.DataSource()=='geant4':
  listJets.append('AntiKt10TruthTrimmedPtFrac5SmallR20Jets')
for i in listJets:
  EXOT7SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
  EXOT7SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'
EXOT7SlimmingHelper.IncludeJetTriggerContent = True
EXOT7SlimmingHelper.IncludeBJetTriggerContent = True
EXOT7SlimmingHelper.IncludeEtMissTriggerContent = True

EXOT7SlimmingHelper.AppendContentToStream(EXOT7Stream)
