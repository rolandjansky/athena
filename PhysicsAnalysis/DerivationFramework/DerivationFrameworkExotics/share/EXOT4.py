#********************************************************************
# EXOT4.py 
# reductionConf flag EXOT4 in Reco_tf.py
#********************************************************************


 
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

# changed to a local version so we can update the eta range to 2.8
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#from DerivationFrameworkExotics.EXOTMuonsCommon import *

exot4Seq = CfgMgr.AthSequencer("EXOT4Sequence")


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT4Stream )
EXOT4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT4Stream.AcceptAlgs(["EXOT4Kernel"])


from DerivationFrameworkCore.WeightMetadata import *
#from DerivationFrameworkExotics.WeightMetadata import *
# use this to save the pile up histogram for each PDF variation
#addWeightsHist(EXOT4Stream, exot4Seq, svcMgr, CfgMgr, ToolSvc, "EXOT4", fileName, {'CT14nlo':20})
#svcMgr.THistSvc.OutputLevel = VERBOSE

#=======================================
# SKIMMING   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression_lep = ''
expression_jet = ''
if (beamEnergy < 4.1e+06):
    SkipTriggerRequirement=(globalflags.DataSource()=='geant4')
    if  SkipTriggerRequirement:
      expression_lep = '(((count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || ((count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
      expression_jet = '(((count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.4)) >0 ))'
    else:
      expression_lep = '((((EventInfo.eventTypeBitmask==1) || EF_e24vhi_medium1 || EF_e60_medium1) && (count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( ((EventInfo.eventTypeBitmask==1) || EF_mu24i_tight || EF_mu36_tight) && count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
      expression_jet = '(((count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.4)) >0 ))'
if (beamEnergy > 6.0e+06):
    SkipTriggerRequirement=(globalflags.DataSource()=='geant4')
    SkipTriggerRequirement = True # disable trigger
    if  SkipTriggerRequirement:
        expression_lep = '(((count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
        #expression_jet = '(((count((AntiKt4EMTopoJets.pt > 15*GeV) && (abs(AntiKt4EMTopoJets.eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt>100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta)<2.4)) >0 ))'
        expression_jet = '(((count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.4)) >0 ))'
    else:
        expression_lep = '(((HLT_e28_tight_iloose || HLT_e60_medium) && (count(Electrons.Medium && Electrons.pt > 15*GeV && Electrons.eta > -2.8 && Electrons.eta < 2.8)>0)) || (( (HLT_mu26_imedium || HLT_mu50) && count(Muons.DFCommonGoodMuon && Muons.pt > 15*GeV && Muons.eta > -2.8 && Muons.eta < 2.8)>0)))'
        expression_jet = '(((count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)) >= 2)) || (count (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 100*GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta)<2.4)) >0 ))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT4StringSkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool(name = "EXOT4StringSkimmingTool_lep",
                                                                         expression = expression_lep)

ToolSvc += EXOT4StringSkimmingTool_lep
print EXOT4StringSkimmingTool_lep

EXOT4StringSkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool(name = "EXOT4StringSkimmingTool_jet",
                                                                         expression = expression_jet)

ToolSvc += EXOT4StringSkimmingTool_jet
print EXOT4StringSkimmingTool_jet


# Tracks thinning 
#thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV)"
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
#EXOT4D1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    =  "EXOT4D1TPThinningTool",
#                                                                    ThinningService         =  "EXOT4ThinningSvc",
#                                                                    SelectionString         =  thinning_expression,
#                                                                    InDetTrackParticlesKey  =  "InDetTrackParticles",
#                                                                    ApplyAnd                =  False)
#ToolSvc += EXOT4D1TPThinningTool


# define thinning tool 
thinningTools=[]
# Tracks associated with jets (for e-in-jet OR)
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(    	name                    = "EXOT4JetTPThinningTool",
                                                                                        ThinningService         = "EXOT4ThinningSvc",
                                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                                        SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && AntiKt4EMTopoJets.DFCommonJets_Calib_eta > -2.8 && AntiKt4EMTopoJets.DFCommonJets_Calib_eta < 2.8",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT4JetTPThinningTool
thinningTools.append(EXOT4JetTPThinningTool)

EXOT4CA15JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT4CA15JetTPThinningTool",
                                                                       ThinningService         = "EXOT4ThinningSvc",
                                                                       JetKey                  = "CamKt15LCTopoJets",
                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV && CamKt15LCTopoJets.eta > -2.7 && CamKt15LCTopoJets.eta < 2.7",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT4CA15JetTPThinningTool
thinningTools.append(EXOT4CA15JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                       = "EXOT4MuonTPThinningTool",
                                                                            ThinningService         = "EXOT4ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            SelectionString = "Muons.pt > 7*GeV",
                                                                            ConeSize=0.4)
ToolSvc += EXOT4MuonTPThinningTool
thinningTools.append(EXOT4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(    	name                    = "EXOT4ElectronTPThinningTool",
                                                                                        ThinningService         = "EXOT4ThinningSvc",
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                        SelectionString         = "Electrons.pt > 7*GeV",
                                                                                        ConeSize = 0.4
                                                                              )
ToolSvc += EXOT4ElectronTPThinningTool
thinningTools.append(EXOT4ElectronTPThinningTool)

# calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT4ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT4ElectronCCThinningTool",
                                                                                     ThinningService         = "EXOT4ThinningSvc",
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT4ElectronCCThinningTool
thinningTools.append(EXOT4ElectronCCThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4CA15CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT4CA15CCThinningTool",
                                                                       ThinningService         = "EXOT4ThinningSvc",
                                                                       SGKey                   = "CamKt15LCTopoJets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "CamKt15LCTopoJets.pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT4CA15CCThinningTool
thinningTools.append(EXOT4CA15CCThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT4Ak10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "EXOT4Ak10CCThinningTool",
                                                                       ThinningService         = "EXOT4ThinningSvc",
                                                                       SGKey                   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                       TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                       SelectionString         = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV",
                                                                       ConeSize                = 0)
ToolSvc += EXOT4Ak10CCThinningTool
thinningTools.append(EXOT4Ak10CCThinningTool)

# Truth particles (this is not working for ghost-associated b-hadrons)
#pythia_status = "(TruthParticles.status == 1 || TruthParticles.status == 2)"
#truth_eta = "abs(TruthParticles.eta) < 2.8"
#truth_quarks  = "(abs(TruthParticles.pdgId) == 1 || abs(TruthParticles.pdgId) == 2 || abs(TruthParticles.pdgId) == 3 || abs(TruthParticles.pdgId) == 4 || abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 6)"
#truth_leptons = "(abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 14 ||abs(TruthParticles.pdgId) == 15 ||abs(TruthParticles.pdgId) ==16)"
#truth_bhadrons = "(abs(TruthParticles.pdgId)==51 ||abs(TruthParticles.pdgId)==52 ||abs(TruthParticles.pdgId)==53 || abs(TruthParticles.pdgId)==54 || abs(TruthParticles.pdgId)==55 || abs(TruthParticles.pdgId)==56 || abs(TruthParticles.pdgId)==511 ||abs(TruthParticles.pdgId)==521|| abs(TruthParticles.pdgId)==531 ||abs(TruthParticles.pdgId)==541 ||abs(TruthParticles.pdgId)==551 || abs(TruthParticles.pdgId)==513 || abs(TruthParticles.pdgId)==523 || abs(TruthParticles.pdgId)==533 ||abs(TruthParticles.pdgId)==543 || abs(TruthParticles.pdgId)==553 || abs(TruthParticles.pdgId)==5101 || abs(TruthParticles.pdgId)==5201 || abs(TruthParticles.pdgId)==5301 ||abs(TruthParticles.pdgId)==5401 ||  abs(TruthParticles.pdgId)==5103 || abs(TruthParticles.pdgId)==5203 || abs(TruthParticles.pdgId)==5303 || abs(TruthParticles.pdgId)==5403 || abs(TruthParticles.pdgId)==5503 || abs(TruthParticles.pdgId)==515 || abs(TruthParticles.pdgId)==525 || abs(TruthParticles.pdgId)==535 || abs(TruthParticles.pdgId)==545 || abs(TruthParticles.pdgId)==555 || abs(TruthParticles.pdgId)==5122 || abs(TruthParticles.pdgId)==5212 || abs(TruthParticles.pdgId)==5222 || abs(TruthParticles.pdgId)==5312 || abs(TruthParticles.pdgId)==5322 || abs(TruthParticles.pdgId)==5332 || abs(TruthParticles.pdgId)==5412 || abs(TruthParticles.pdgId)==5422  || abs(TruthParticles.pdgId)==5432 || abs(TruthParticles.pdgId)==5442 || abs(TruthParticles.pdgId)==5512 || abs(TruthParticles.pdgId)==5522 || abs(TruthParticles.pdgId)==5532 || abs(TruthParticles.pdgId)==5542 || abs(TruthParticles.pdgId)==5114 || abs(TruthParticles.pdgId)==5214 || abs(TruthParticles.pdgId)==5224 || abs(TruthParticles.pdgId)==5314 || abs(TruthParticles.pdgId)==5324 || abs(TruthParticles.pdgId)==5334 || abs(TruthParticles.pdgId)==5414 || abs(TruthParticles.pdgId)==5424 || abs(TruthParticles.pdgId)==5434 || abs(TruthParticles.pdgId)==5444 || abs(TruthParticles.pdgId)==5514 || abs(TruthParticles.pdgId)==5524 || abs(TruthParticles.pdgId)==5534 || abs(TruthParticles.pdgId)==5544 || abs(TruthParticles.pdgId)==5554  || abs(TruthParticles.pdgId)==5132 || abs(TruthParticles.pdgId)==5232 || abs(TruthParticles.pdgId)==5142 || abs(TruthParticles.pdgId)==5242 || abs(TruthParticles.pdgId)==5342 || abs(TruthParticles.pdgId)==5112)"
#truth_signal = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021"
#truth_WZ = "abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) ==23"
#truth_expression = '('+pythia_status+' && '+truth_eta+')'
#truth_expression2 = '('+truth_quarks+' || '+truth_leptons+' || '+truth_bhadrons+' || '+truth_signal+' || '+truth_WZ+')'
#final_expression = '('+truth_expression+' && '+truth_expression2+')'
 

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT4MCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4MCThinningTool",
                                                             ThinningService = "EXOT4ThinningSvc",
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
                                                             PreserveDescendants        = False)

EXOT4TMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TMCThinningTool",
                                                             ThinningService = "EXOT4ThinningSvc",
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

EXOT4TAMCThinningTool = DerivationFramework__MenuTruthThinning(name = "EXOT4TAMCThinningTool",
                                                             ThinningService = "EXOT4ThinningSvc",
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
	                                                     WriteLeptonsNotFromHadrons         = True,
	                                                     WriteStatus3               = False,
	                                                     WriteFirstN                = 10,
                                                             PreserveDescendants        = False,
                                                             PreserveAncestors          = True)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT4MCGenThinningTool = DerivationFramework__GenericTruthThinning(name = "EXOT4MCGenThinningTool",
                                                                ThinningService = "EXOT4ThinningSvc",
                                                                ParticleSelectionString = "abs(TruthParticles.pdgId) ==25 || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==32 || abs(TruthParticles.pdgId)==5100021", 
                                                                PreserveDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
    ToolSvc += EXOT4MCThinningTool
    thinningTools.append(EXOT4MCThinningTool)
    ToolSvc += EXOT4TMCThinningTool
    thinningTools.append(EXOT4TMCThinningTool)
    ToolSvc += EXOT4TAMCThinningTool
    thinningTools.append(EXOT4TAMCThinningTool)
    ToolSvc += EXOT4MCGenThinningTool
    thinningTools.append(EXOT4MCGenThinningTool)



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot4Seq
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel_lep", SkimmingTools = [EXOT4StringSkimmingTool_lep])


from DerivationFrameworkExotics.JetDefinitions import *
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm

augTools = []
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(exot4Seq)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "EXOT4GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  augTools.append(ToolSvc.EXOT4GenFilt)

#addDefaultTrimmedJets(exot4Seq, "EXOT4")
if globalflags.DataSource()=='geant4':
    addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=exot4Seq, outputGroup="EXOT4", mods="exotJetMods")
addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=exot4Seq, outputGroup="EXOT4", mods="exotJetMods")

addStandardJets("CamKt", 1.5, "LCTopo", mods = "calib_notruth", algseq = exot4Seq, outputGroup = "EXOT4")

applyJetCalibration_xAODColl("AntiKt4EMTopo", exot4Seq)
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot4Seq)

exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel_jet", SkimmingTools = [EXOT4StringSkimmingTool_jet])
exot4Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT4Kernel", ThinningTools = thinningTools, AugmentationTools = augTools)



# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT4ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT4
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT4ContentList import *
EXOT4SlimmingHelper = SlimmingHelper("EXOT4SlimmingHelper")

EXOT4SlimmingHelper.SmartCollections = EXOT4SmartCollections

EXOT4SlimmingHelper.ExtraVariables = EXOT4ExtraVariables

EXOT4SlimmingHelper.AllVariables = EXOT4AllVariables

EXOT4SlimmingHelper.StaticContent = EXOT4Content

addMETOutputs(EXOT4SlimmingHelper, ["Track"], ["AntiKt4EMTopo"])
EXOT4SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT4SlimmingHelper.IncludeMuonTriggerContent = True


EXOT4SlimmingHelper.AppendContentToStream(EXOT4Stream)


