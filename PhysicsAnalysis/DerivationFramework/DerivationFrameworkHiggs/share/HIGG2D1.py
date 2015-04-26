#====================================================================
# HIGG2D1.py for 2L filter
# This requires the reductionConf flag HIGG2D1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
import AthenaCommon.SystemOfUnits as Units

#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG2D1TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D1TPThinningTool",
                                                                   ThinningService        = "HIGG2D1ThinningSvc",
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                   ApplyAnd               = True)
ToolSvc += HIGG2D1TPThinningTool
thinningTools.append(HIGG2D1TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D1JetTPThinningTool",
                                                                         ThinningService        = "HIGG2D1ThinningSvc",
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         ApplyAnd               = True)
ToolSvc += HIGG2D1JetTPThinningTool
thinningTools.append(HIGG2D1JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D1MuonTPThinningTool",
                                                                           ThinningService        = "HIGG2D1ThinningSvc",
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1MuonTPThinningTool
thinningTools.append(HIGG2D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D1ElectronTPThinningTool",
                                                                                 ThinningService        = "HIGG2D1ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D1ElectronTPThinningTool
thinningTools.append(HIGG2D1ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D1TauTPThinningTool",
                                                                         ThinningService        = "HIGG2D1ThinningSvc",
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1TauTPThinningTool
thinningTools.append(HIGG2D1TauTPThinningTool)

# Truth particles
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG2D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D1TruthThinningTool", 
                                                                     ThinningService              = "HIGG2D1ThinningSvc",
                                                                     ParticleSelectionString      = truth_expression,
                                                                     PreserveDescendants          = False,
                                                                     PreserveGeneratorDescendants = True,
                                                                     PreserveAncestors            = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG2D1.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D1TruthThinningTool
    thinningTools.append(HIGG2D1TruthThinningTool)
print "HIGG2D1.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

electronPtRequirement='(DFCommonElectrons_pt>10.*GeV)'
electronRequirement='('+electronPtRequirement+')'
combinedMuonQualityRequirement='(Muons.muonType==0)'
standaloneMuonQualityRequirement='(Muons.muonType==1 && abs(abs(Muons.eta)-2.6)<0.12)'
segmentTaggedMuonQualityRequirement='(Muons.muonType==2)'
calorimeterTaggedMuonQualityRequirement='(Muons.muonType==3 && abs(Muons.eta)<0.2)'
muonQualityRequirement='('+combinedMuonQualityRequirement+'||'+standaloneMuonQualityRequirement+'||'+segmentTaggedMuonQualityRequirement+'||'+calorimeterTaggedMuonQualityRequirement+')'
muonPtRequirement='(Muons.pt>10.*GeV)'
muonRequirement='('+muonQualityRequirement+'&&'+muonPtRequirement+')'
leptonRequirement='(count('+electronRequirement+')+count('+muonRequirement+')>= 2)'

from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
SkipTriggerRequirement=True # Temporally disable trigger requirement (2015-01-30) ###################################################################################################################################
print "HIGG2D1.py SkipTriggerRequirement", SkipTriggerRequirement

singleElectronTriggerRequirement='(EF_e24vhi_medium1 || EF_e60_medium1)'
diElectronTriggerRequirement='(EF_2e12Tvh_loose1 || EF_2e12Tvh_loose1_L2StarB)'
singleMuonTriggerRequirement='(EF_mu24i_tight || EF_mu36_tight)'
diMuonTriggerRequirement='(EF_2mu13 || EF_mu18_tight_mu8_EFFS)'
electronMuonTriggerRequirement='(EF_e12Tvh_medium1_mu8 || EF_e24vhi_loose1_mu8)'

if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # singleElectronTriggerRequirement='(HLT_e28_tight_iloose || HLT_e60_medium)'
        singleElectronTriggerRequirement='(HLT_e24_medium_iloose || HLT_e24_tight_L1EM20V || HLT_e28_tight_iloose || HLT_e60_medium)'
        # diElectronTriggerRequirement='(HLT_2e17_loose1)'
        diElectronTriggerRequirement='(HLT_2e12_loose1 || HLT_2e17_loose1)'
        # singleMuonTriggerRequirement='(HLT_mu26_imedium || HLT_mu50)'
        singleMuonTriggerRequirement='(HLT_mu24_imedium || HLT_mu26_imedium || HLT_mu50)'
        # diMuonTriggerRequirement='(HLT_2mu14 || HLT_mu24_mu8noL1)'
        diMuonTriggerRequirement='(HLT_2mu10 || HLT_2mu14 || HLT_mu24_mu8noL1)'
        electronMuonTriggerRequirement='(HLT_e17_medium1_mu12)'
    elif jobproperties.Beam.energy()==4000000.0:
        # 8 TeV data
        singleElectronTriggerRequirement='(EF_e24vhi_medium1 || EF_e60_medium1)'
        diElectronTriggerRequirement='(EF_2e12Tvh_loose1 || EF_2e12Tvh_loose1_L2StarB)'
        singleMuonTriggerRequirement='(EF_mu24i_tight || EF_mu36_tight)'
        diMuonTriggerRequirement='(EF_2mu13 || EF_mu18_tight_mu8_EFFS)'
        electronMuonTriggerRequirement='(EF_e12Tvh_medium1_mu8 || EF_e24vhi_loose1_mu8)'
    else:
        SkipTriggerRequirement = True

triggerRequirement='('+singleElectronTriggerRequirement+'||'+diElectronTriggerRequirement+'||'+singleMuonTriggerRequirement+'||'+diMuonTriggerRequirement+'||'+electronMuonTriggerRequirement+')'

if SkipTriggerRequirement:
    isMC='(EventInfo.eventTypeBitmask==1)'
    triggerRequirement='('+triggerRequirement+'||'+isMC+')'

print "HIGG2D1.py triggerRequirement", triggerRequirement

expression='('+leptonRequirement+'&&'+triggerRequirement+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SkimmingToolHIGG2D1 = DerivationFramework__xAODStringSkimmingTool(name = "SkimmingToolHIGG2D1",
                                                                  expression = expression)
ToolSvc += SkimmingToolHIGG2D1
print SkimmingToolHIGG2D1

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D1Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D1],
                                                                       ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG2D1Stream )
HIGG2D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG2D1Stream.AcceptAlgs(["HIGG2D1Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG2D1ThinningSvc", outStreams=[evtStream] )
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D1ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D1SlimmingHelper = SlimmingHelper("HIGG2D1SlimmingHelper")

HIGG2D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "MET_Reference_AntiKt4LCTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4LCTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4LCTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D1SlimmingHelper.ExtraVariables = HIGG2D1ExtraContent
HIGG2D1SlimmingHelper.AllVariables = HIGG2D1ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D1SlimmingHelper.ExtraVariables += HIGG2D1ExtraContentTruth
    HIGG2D1SlimmingHelper.AllVariables += HIGG2D1ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D1SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D1SlimmingHelper.AppendContentToStream(HIGG2D1Stream)
