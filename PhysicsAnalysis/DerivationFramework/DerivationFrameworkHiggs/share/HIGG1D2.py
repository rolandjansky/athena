#====================================================================
# HIGG1D2.py
# This requires the reductionConf flag HIGG1D2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False


if globalflags.DataSource()=='geant4':
    from DerivationFrameworkHiggs.TruthCategories import *

from DerivationFrameworkCore.LHE3WeightMetadata import *

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    addStandardTruthContents()
    addPVCollection()
    print "HIGG1D2.py Applying MCTruthCommon"




#====================================================================
# SKIMMING TOOLS 
#====================================================================


SkipTriggerRequirement=((globalflags.DataSource()=='geant4'))
print "HIGG1D2.py SkipTriggerRequirement: ",  SkipTriggerRequirement
TriggerExp = []
TriggerMerged = []
if not SkipTriggerRequirement:
   
   try:
     year=int(rec.projectName()[4:6])
     if year > 17 :
       year=17
     print "HIGG1D2.py: Project tag: " + rec.projectName() +  " Year: " +  str(year)
   except:
     print "HIGG1D2.py: Failed to extract year from project tag "+ rec.projectName() +". Guessing 2017"
     year=17
   print "HIGG1D2.py: Setting up trigger requirement for year 20" + str(year)
   if year==17:
     TriggerMerged          = ["HLT_e25_mergedtight_g35_medium_Heg","HLT_2g50_loose_L12EM20VH"]
     TriggerExp             = [ 
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_g35_loose_L1EM24VHI_mu15_mu2noL1", # photon dimuon
                               "HLT_g35_loose_L1EM24VHI_mu18",  #photon muon
                               "HLT_g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1", # photon dimuon 
                               "HLT_g35_tight_icalotight_L1EM24VHI_mu18noL1", # photon muon
                               "HLT_2mu14", #dimuon 
                               "HLT_mu22_mu8noL1", #dimuon
                               "HLT_mu26_ivarmedium", #single muon
                               "HLT_mu50", 
                               "HLT_mu60_0eta105_msonly", 
                               "HLT_e26_lhtight_nod0_ivarloose", #single electron
                               "HLT_e60_lhmedium_nod0",
                               "HLT_e140_lhloose_nod0",
                               "HLT_2e17_lhvloose_nod0_L12EM15VHI", #dielectron
                               "HLT_2e24_lhvloose_nod0",
                               "HLT_e25_mergedtight_g35_medium_Heg", #merged electron
                               "HLT_2g50_loose_L12EM20VH", #di photon
                               "HLT_g200_loose", #single photon
                               "HLT_g300_etcut"  #single photon
                              ]
   elif year==16:
     #2016
     TriggerMerged          = ["HLT_g35_loose_g25_loose","HLT_e20_lhmedium_nod0_g35_loose"]
     TriggerExp             = [ 
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1", # photon dimuon
                               "HLT_g35_loose_L1EM22VHI_mu18noL1",  #photon muon
                               "HLT_2mu10",
                               "HLT_2mu14", #dimuon 
                               "HLT_mu22_mu8noL1", #dimuon
                               "HLT_mu20_mu8noL1",
                               "HLT_mu24_ivarmedium", #single muon
                               "HLT_mu26_ivarmedium",
                               "HLT_mu50", 
                               "HLT_e24_lhtight_nod0_ivarloose", #single electron
                               "HLT_e26_lhtight_nod0_ivarloose",
                               "HLT_e60_lhmedium_nod0",
                               "HLT_e60_medium",
                               "HLT_e140_lhloose_nod0",
                               "HLT_2e15_lhvloose_nod0_L12EM153VHI", #dielectron
                               "HLT_2e17_lhvloose_nod0",
                               "HLT_g35_loose_g25_loose", #di photon
                               "HLT_e20_lhmedium_nod0_g35_loose", # electron  +  photon
                               "HLT_g140_loose", #single photon
                               "HLT_g300_etcut"  #single photon
                              ]
   elif year == 2015: 
     #2015
     TriggerMerged          = ["HLT_g35_loose_g25_loose","HLT_e20_lhmedium_g35_loose"]
     TriggerExp             = [ 
                               "HLT_g25_medium_mu24", #photon muon
                               "HLT_g15_loose_2mu10_msonly", #photon dimuon
                               "HLT_2mu10", #dimuon 
                               "HLT_mu18_mu8noL1", #dimuon
                               "HLT_mu20_iloose_L1MU15", #single muon
                               "HLT_mu40", 
                               "HLT_mu60_0eta105_msonly", 
                               "HLT_e24_lhmedium_L1EM20VH", #single electron
                               "HLT_e60_lhmedium",
                               "HLT_e120_lhloose",
                               "HLT_2e12_lhvloose_L12EM10VH", #dielectron
                               "HLT_e20_lhmedium_g35_loose", #electron + photon
                               "HLT_g35_loose_g25_loose", #di photon
                               "HLT_g120_loose", #single photon
                               "HLT_g200_etcut"  #single photon
                              ]
   else:
     print "HIGG1D2.py : Year not supported -- results might be ugly. Year : ", year 



print "HIGG1D2.py TriggerExp", TriggerExp


RequireTrigger=not SkipTriggerRequirement

print "HIGG1D2.py RequireTrigger", RequireTrigger


from ROOT import egammaPID
MergedElectronIsEM = CfgMgr.AsgElectronIsEMSelector("MergedElectronIsEM",
                                             isEMMask=egammaPID.ElectronTightHLT,
                                             ConfigFile="ElectronPhotonSelectorTools/trigger/rel21_20161021/ElectronIsEMMergedTightSelectorCutDefs.conf")
ToolSvc += MergedElectronIsEM
    


from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG1
HIGG1D2SkimmingTool = DerivationFramework__SkimmingToolHIGG1(
                                 name = "HIGG1D2SkimmingTool",
                                 RequireGRL = False,
                                 ReqireLArError = True,
                                 RequireTrigger = RequireTrigger,
                                 IncludeDoublePhotonPreselection = False,
                                 RequirePreselection = False,
                                 RequireKinematic = False,
                                 RequireQuality = False,
                                 RequireIsolation = False,
                                 RequireInvariantMass = False,
                                 Triggers = TriggerExp,
                                 MergedElectronTriggers = TriggerMerged,
                                 IncludeSingleElectronPreselection = False,
                                 IncludeDoubleElectronPreselection = False,
                                 IncludeSingleMuonPreselection = False,
                                 IncludePhotonDoubleElectronPreselection = True,
                                 IncludeDoubleMuonPreselection = True,
                                 IncludePhotonMergedElectronPreselection = True,
                                 IncludeHighPtPhotonElectronPreselection = True,
                                 MinimumPhotonPt = 7.5*GeV,
                                 MinimumElectronPt = 7.5*GeV,
                                 MinimumMuonPt = 7.5*GeV,   
                                 MaxMuonEta = 2.7,     
                                 RemoveCrack = False,    
                                 MaxEta = 2.5,
                                 MergedElectronCutTool = MergedElectronIsEM
                                 )

ToolSvc += HIGG1D2SkimmingTool
print HIGG1D2SkimmingTool



#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG1D2MetTPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D2MetTPThinningTool",
                                                                       ThinningService         = "HIGG1D2ThinningSvc",
                                                                       SelectionString         = thinning_expression,
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       ApplyAnd                = True)
ToolSvc += HIGG1D2MetTPThinningTool
print HIGG1D2MetTPThinningTool
#thinningTools.append(HIGG1D2MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D2JetLCTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D2JetLCTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D2JetLCTPThinningTool
print HIGG1D2JetLCTPThinningTool
#thinningTools.append(HIGG1D2JetLCTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG1D2JetEMTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "HIGG1D2JetEMTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            JetKey                  = "AntiKt4EMTopoJets",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                            ApplyAnd                = True)
ToolSvc += HIGG1D2JetEMTPThinningTool
print HIGG1D2JetEMTPThinningTool
#thinningTools.append(HIGG1D2JetEMTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG1D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "HIGG1D2MuonTPThinningTool",
                                                                            ThinningService         = "HIGG1D2ThinningSvc",
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2MuonTPThinningTool
print HIGG1D2MuonTPThinningTool
#thinningTools.append(HIGG1D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2ElectronTPThinningTool",
                                                                                  ThinningService         = "HIGG1D2ThinningSvc",
                                                                                  SGKey                   = "Electrons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2ElectronTPThinningTool
print HIGG1D2ElectronTPThinningTool
#thinningTools.append(HIGG1D2ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG1D2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "HIGG1D2PhotonTPThinningTool",
                                                                                  ThinningService         = "HIGG1D2ThinningSvc",
                                                                                  SGKey                   = "Photons",
                                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2PhotonTPThinningTool
print HIGG1D2PhotonTPThinningTool
#thinningTools.append(HIGG1D2PhotonTPThinningTool)

# Tracks themselves
HIGG1D2TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HIGG1D2TPThinningTool",
                                                                    ThinningService         = "HIGG1D2ThinningSvc",
                                                                    SelectionString             = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG1D2TPThinningTool
print HIGG1D2TPThinningTool
#thinningTools.append(HIGG1D2TPThinningTool)



# Truth thinning
truth_cond_1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_3 = "((abs(TruthParticles.pdgId) ==  6))"                                     # Top quark
truth_cond_4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_cond_finalState = '(TruthParticles.status == 1 && TruthParticles.barcode < 200000)' # stable particles
truth_expression = '('+truth_cond_1+' || '+truth_cond_2 +' || '+truth_cond_3 +' || '+truth_cond_4+') || ('+truth_cond_finalState+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG1D2TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG1D2TruthThinningTool", 
                                                                      ThinningService         = "HIGG1D2ThinningSvc",
                                                                      ParticleSelectionString = truth_expression,
                                                                      PreserveDescendants     = False,
                                                                      PreserveGeneratorDescendants     = True,
                                                                      PreserveAncestors      = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG1D2.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG1D2TruthThinningTool
    thinningTools.append(HIGG1D2TruthThinningTool)
print "HIGG1D2.py thinningTools", thinningTools

# Create private sequence
# The name of the kernel  must be unique to this derivation
HIGG1D2Seq = CfgMgr.AthSequencer("HIGG1D2Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG1D2Kernel",
                                                                       #AugmentationTools = [HIGG1D2_EEMassTool,HIGG1D2_MuMuMassTool],
                                                                       SkimmingTools = [HIGG1D2SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                      )

# Before any custom jet reconstruction, it's good to set up the output list
OutputJets["HIGG1D2Jets"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
reducedJetList = [
                  "AntiKt4TruthJets",
                  "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList,HIGG1D2Seq,"HIGG1D2Jets")

DerivationFrameworkJob += HIGG1D2Seq

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG1D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG1D2Stream )
HIGG1D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG1D2Stream.AcceptAlgs(["HIGG1D2Kernel"])


# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG1D2ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG1D2SlimmingHelper = SlimmingHelper("HIGG1D2SlimmingHelper")
HIGG1D2Stream.AddItem("xAOD::EventShape#*")
HIGG1D2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

HIGG1D2SlimmingHelper.AppendToDictionary = {'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPrimaryVertices': 'xAOD::VertexContainer','TruthPrimaryVerticesAux': 'xAOD::VertexAuxContainer'}

HIGG1D2SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices" ]

HIGG1D2SlimmingHelper.AllVariables = ["Electrons","Photons","TruthPrimaryVertices","egammaClusters","GSFConversionVertices","TruthEvents", "TruthParticles", "TruthElectrons","TruthPhotons","TruthMuons","TruthBoson","TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets","PrimaryVertices","MET_Truth", "MET_Track", "egammaTruthParticles","CaloCalTopoClusters"]

HIGG1D2SlimmingHelper.ExtraVariables = ["Muons.quality.EnergyLoss.energyLossType.etcone20.ptconecoreTrackPtrCorrection",
                                        "MuonClusterCollection.eta_sampl.phi_sampl",
                                        "GSFTrackParticles.parameterY.parameterZ.vx.vy",
                                        "InDetTrackParticles.vx.vy",
                                        "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.PartonTruthLabelID.Jvt.JVFCorr.JvtRpt.ConstituentScale",
                                        "CombinedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "BTagging_AntiKt4EMTopo.MV1_discriminant",                                        
                                        "ExtrapolatedMuonTrackParticles.z0.vz.definingParametersCovMatrix",
                                        "MuonTruthParticles.truthOrigin.truthType"]

# Add AODCellContainer (have to find how to keep only cells belonging to e/gamma objects)
# Usage of AddItem is forbidden
# HIGG1D2Stream.AddItem("CaloCellContainer#AODCellContainer")

HIGG1D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG1D2SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG1D2SlimmingHelper.AppendContentToStream(HIGG1D2Stream)
