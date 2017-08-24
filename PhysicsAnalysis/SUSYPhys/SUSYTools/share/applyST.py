###############################
# jobOptions for ApplySUSYTools
###############################

import AthenaPoolCnvSvc.ReadAthenaPool

################
# Job Parameters
################

# Event number
EVTMAX = 1000

# Output file name
#xAODFileName = "DAOD_AST.test.pool.root"
#xAODFileName = "DAOD_AST.stop1_jul20.pool.root"
xAODFileName = "DAOD_AST.topaod_jul20.pool.root"

# Input dataset
# SZ - MIND that if you change this, you should also change accordingly the hard-coded "dataSource = 1" in line 73
svcMgr.EventSelector.InputCollections= [os.environ['ASG_TEST_FILE_MC']]


from glob import glob
#inputDir = "/usatlas/groups/bnl_local2/paige/SUSY1/mc15_13TeV.387200.MadGraphPythia8EvtGen_A14NNPDF23LO_TT_directTT_800_100.merge.DAOD_SUSY1.e3969_s2608_r7772_r7676_p2666"
#inputDir = "/usatlas/groups/bnl_local2/paige/AOD/mc15_13TeV.387198.MadGraphPythia8EvtGen_A14NNPDF23LO_TT_directTT_800_1.merge.AOD.e3969_a766_a821_r7676"
#inputAOD = "%s/*.pool.root*" % inputDir
#svcMgr.EventSelector.InputCollections = glob(inputAOD)

svcMgr.MessageSvc.defaultLimit = 9999999
#svcMgr.MessageSvc.Format = "% F%18W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

# Truth type:
# 1: MenuTruthThinning with PreserveAncestors (c.f. SUSY1)
# 2: TruthHard with reclustered parton shower (c.f. TRUTH2)
doTruth = 1


#################
# Sample metadata
#################

# See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
af.fileinfos    #this is a dict of dicts
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
beam_energy = af.fileinfos['beam_energy']
conditions_tag = af.fileinfos['conditions_tag']
if isMC:
    isFullSim = af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour']=='default'

# isData     = 0:  data in ApplySUSYTools
#            = 1:  MC in ApplySUSYTools
# dataSource = 0:  data in ST__SUSYObjDef_xAOD
# dataSource = 1:  fullsim in ST__SUSYObjDef_xAOD
# dataSource = 2:  AtlFastII in ST__SUSYObjDef_xAOD

if isMC:  
    isData = 0
    if isFullSim: 
        dataSource = 1
    else:
        dataSource = 2
else:
    isData = 1
    dataSource = 0

# SZ - UNFORTUNATELY METADATA IS BROKEN IN R21 :-(
# UUNCOMMENT BELOW TO HARD-CODE 'dataSource' TO FULLSIM MC
# SINCE ANYWAY WE'RE RUNNING OVER 'ASG_TEST_FILE_MC'
#dataSource = 1

###############
# Configure job
###############

# List of containers to convert to AuxContainerBase with AuxDyn variables.
# Change list if you change ApplySUSYTools inputs.

auxList = ["PrimaryVerticesAux.", "AntiKt4EMTopoJetsAux.", "ElectronsAux.",
    "MuonsAux.", "BTagging_AntiKt4EMTopoAux.", "AntiKt4TruthJetsAux.",
    "InDetTrackParticlesAux.", "GSFTrackParticlesAux.",
    "MuonSpectrometerTrackParticlesAux.", "MET_TrackAux.", "TauJetsAux.",
    "PhotonsAux.", "GSFConversionVerticesAux."]

# Get the configuration manager
from AthenaCommon import CfgMgr

# Get a handle to the main athsequencer, for adding things to later!
AST99Job = CfgMgr.AthSequencer("AthAlgSeq")

# Add a tool for thinning derivations
AST99Job += CfgMgr.xAODMaker__AuxStoreWrapper("AST99AuxStoreWrapperAlg", 
                                              SGKeys = auxList,
                                              OutputLevel = INFO)

###################################
# Create Output Stream and Thinning
###################################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStreamName = "AST99Stream"
AST99Stream = MSMgr.NewPoolRootStream( xAODStreamName, xAODFileName )

from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc("AST99ThinningSvc",
             Streams = [xAODStreamName],
         OutputLevel = INFO)

thinningTools = []
skimmingTools = []

#########################################
# Configure SUSYTools and object thinning
#########################################

# Initial SUSYObjDef_xAOD
# PRWConfigFiles and PRWLumiCalcFiles from minimalExampleJobOptions_mc.py
AST99ObjDef = CfgMgr.ST__SUSYObjDef_xAOD("AST99ObjDef",
                                         DataSource = dataSource,
                                         ConfigFile = "SUSYTools/SUSYTools_Default.conf",
                                         JESNuisanceParameterSet = 1, 
                                         PRWConfigFiles = [
                                         "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
                                         "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"],
                                         PRWLumiCalcFiles = ["/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ilumicalc_histograms_None_276262-284154_IBLOFF.root"],
                                         OutputLevel = WARNING)
ToolSvc += AST99ObjDef

# For tau CP tools need to run TauTruthMatchingTool. Should be provided
# by most derivations but NOT in xAOD. 
# Can avoid by using SkipTruthMatchCheck = True.

### Not needed for 2.4?
AST99tauTruthTool = CfgMgr.TauAnalysisTools__TauTruthMatchingTool(
                                        name = "AST99TauTruthMatchingTool",
                              WriteTruthTaus = True,
                                 OutputLevel = INFO)
ToolSvc += AST99tauTruthTool

# SZ - commenting BuildTruthTaus out, after having discussed this with David Kirchmeier:
# TauTruthMatchingTool inherits from it and can be used instead
#
#AST99tauBuildTruthTaus = CfgMgr.tauRecTools__BuildTruthTaus(
#                                        name = "AST99TauBuildTruthTaus",
#                              WriteTruthTaus = True,
#                                 OutputLevel = INFO)
#ToolSvc += AST99tauBuildTruthTaus

# Not until https://its.cern.ch/jira/browse/ATLASG-794 is solved
#tauSmearingTool = CfgMgr.TauAnalysisTools__TauSmearingTool("TauSmearingTool",
#                                                           SkipTruthMatchCheck = False,
#                                                           OutputLevel = INFO)
#ToolSvc += tauSmearingTool
#AST99ObjDef.TauSmearingTool = tauSmearingTool

# Set up trigger tools to avoid multiple instances
# Both SUSYObjDef and CutTool for skimming need TrigDecisionTool

configTool = CfgMgr.TrigConf__xAODConfigTool()
ToolSvc += configTool

trigDecTool = CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool",
                                            ConfigTool = configTool,
                                            TrigDecisionKey = "xTrigDecision")
ToolSvc += trigDecTool

AST99ObjDef.TrigConfigTool = configTool
AST99ObjDef.TrigDecisionTool = trigDecTool


# Initialize ApplySUSYTools
# SUSYTools uses a 20GeV cut for "baseline"; JetsPtCut is in addition.
# Triggers are just typical examples

applyST = CfgMgr.ST__ApplySUSYTools(
                         MuonsName = "Muons",
                MuonSpecTracksName = "MuonSpectrometerTrackParticles",
                     ElectronsName = "Electrons",
                          JetsName = "AntiKt4EMTopoJets",
# No fat jets unless they are set in the config file
                       FatJetsName = "NONE", #"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                      BTaggingName = "BTagging_AntiKt4EMTopo",
                     TruthJetsName = "AntiKt4TruthJets",
                          METsName = "MET_Reference_AntiKt4EMTopo",
                         JetsPtCut = 20000.,
                   InDetTracksName = "InDetTrackParticles",
                        InDetPtCut = 1000.,
                     GSFTracksName = "GSFTrackParticles",
                   GSFVerticesName = "GSFConversionVertices",
                  ElectronTriggers = ["HLT_e60_medium", "HLT_2e12_loose1_L12EM10VH"],
                      MuonTriggers = ["HLT_mu24_imedium", "HLT_2mu10"],
                       TauJetsName = "TauJets",
                       PhotonsName = "Photons",
                    PhotonTriggers = ["HLT_g120_loose", "HLT_2g20_tight"],
                             DoTST = True,
                            IsData = isData,
                          MaxCount = 10,
                         SUSYTools = ToolSvc.AST99ObjDef,
              TauTruthMatchingTool = AST99tauTruthTool,
                    #BuildTruthTaus = AST99tauBuildTruthTaus, 
                       ThinningSvc = "AST99ThinningSvc",
                       OutputLevel = Lvl.INFO)
AST99Job += applyST


# Jets and systematics
if isMC:
    jetsysnames = ["","Nominal", "JET_JER_SINGLE_NP__1up", "JET_GroupedNP_1__1up", "JET_GroupedNP_1__1down", "JET_GroupedNP_2__1up", "JET_GroupedNP_2__1down", "JET_GroupedNP_3__1up", "JET_GroupedNP_3__1down"]
    if dataSource == 2:
        jetsysnames += ["JET_RelativeNonClosure_AFII__1up", "JET_RelativeNonClosure_AFII__1down"]
else:
    jetsysnames = ["","Nominal"]

# Electrons and systematics
if isMC:
    elsysnames = ["", "Nominal", "EG_RESOLUTION_ALL__1down", "EG_RESOLUTION_ALL__1up", "EG_SCALE_ALL__1down",  "EG_SCALE_ALL__1up", "EL_EFF_ID_TotalCorrUncertainty__1down", "EL_EFF_ID_TotalCorrUncertainty__1up", "EL_EFF_Reco_TotalCorrUncertainty__1down", "EL_EFF_Reco_TotalCorrUncertainty__1up", "EL_EFF_Trigger_TotalCorrUncertainty__1down", "EL_EFF_Trigger_TotalCorrUncertainty__1up"]
else:
    elsysnames = ["", "Nominal"]

# Muons and systematics
# What muon TrackParticles are needed?
if isMC:
    musysnames = ["","Nominal","MUONS_ID__1down", "MUONS_ID__1up", "MUONS_MS__1down", "MUONS_MS__1up", "MUONS_SCALE__1down", "MUONS_SCALE__1up", "MUON_EFF_STAT__1down", "MUON_EFF_STAT__1up", "MUON_EFF_TrigStatUncertainty__1down", "MUON_EFF_TrigStatUncertainty__1up", "MUON_EFF_TrigSystUncertainty__1down", "MUON_EFF_TrigSystUncertainty__1up", "MUON_ISO_STAT__1down", "MUON_ISO_STAT__1up", "MUON_ISO_SYS__1down", "MUON_ISO_SYS__1up"]
else:
    musysnames = ["","Nominal"]

# MET - keep full containers
# Default MET corresponds to "" in jetsysnames
if applyST.DoTST :
    metkey = (applyST.METsName + "Trk")
else:
    metkey = (applyST.METsName + "Clus")

CSTsysnames = ["MET_SoftCalo_Reso", "MET_SoftCalo_ScaleDown", "MET_SoftCalo_ScaleUp"]
TSTsysnames = ["MET_SoftTrk_ResoPara", "MET_SoftTrk_ResoPerp", "MET_SoftTrk_ScaleDown", "MET_SoftTrk_ScaleUp"]

# TauJets
if isMC:
    tausysnames = ["", "Nominal", "TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1down", "TAUS_TRUEHADTAU_EFF_ELEOLR_TOTAL__1up", "TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1down", "TAUS_TRUEHADTAU_EFF_JETID_TOTAL__1up", "TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1down", "TAUS_TRUEHADTAU_EFF_RECO_TOTAL__1up", "TAUS_TRUEHADTAU_SME_TES_TOTAL__1down", "TAUS_TRUEHADTAU_SME_TES_TOTAL__1up"]
else:
    tausysnames = ["", "Nominal"]

# Photons
if isMC:
    phsysnames = ["", "Nominal", "EG_RESOLUTION_ALL__1down", "EG_RESOLUTION_ALL__1up", "EG_SCALE_ALL__1down",  "EG_SCALE_ALL__1up", "PH_EFF_Uncertainty__1down", "PH_EFF_Uncertainty__1up"]
else:
    phsysnames = ["", "Nominal"]


##########################
# Configure Truth Thinning
##########################

if isMC and doTruth==1:
    # Based on SUSY1.py. Thin TruthParticles keeping ancestors to preserve
    # history. Added WritePi0Bosons to MenuTruthThinning.
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    AST99TruthThinning = DerivationFramework__MenuTruthThinning(
                         name                         = "AST99TruthThinning",
                         ThinningService              = "AST99ThinningSvc",
                         WritePartons                 = False,
                         WriteHadrons                 = False,
                         WriteBHadrons                = True,
                         WriteGeant                   = False,
                         GeantPhotonPtThresh          = 20000.,
                         WriteTauHad                  = True,
                         PartonPtThresh               = -1.0,
                         WriteBSM                     = True,
                         WriteBosons                  = True,
                         WriteBSMProducts             = True,
                         WriteBosonProducts           = True,
                         WriteTopAndDecays            = True,
                         WriteEverything              = False,
                         WriteAllLeptons              = False,
                         WriteLeptonsNotFromHadrons   = False,
                         WriteStatus3                 = False,
                         WriteFirstN                  = -1,
                         PreserveAncestors            = True,
                         PreserveGeneratorDescendants = False,
                         SimBarcodeOffset             = 200000)
    ToolSvc += AST99TruthThinning
    thinningTools.append(AST99TruthThinning)


if isMC and doTruth==2:
    # Add back-translator from xAOD::TruthEvent to HepMC for xAOD input
    # Needed to run hard truth on xAOD input files
    # Use GEN_EVENT name for output
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__xAODtoHepMCCnvAlg
    AST99Job += DerivationFramework__xAODtoHepMCCnvAlg("xAOD2GEN_EVENT",
                                                       xAODTruthEventKey  = "TruthEvents",
                                                       HepMCTruthEventKey = "GEN_EVENT",
                                                       MaxCount           = 0,
                                                       OutputLevel        = Lvl.INFO)

    # Build compact hard truth object
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__CompactHardTruth
    makeHard = DerivationFramework__CompactHardTruth(McEvent     = "GEN_EVENT",
                                                     McEventOut  = "GEN_HARD",
                                                     MaxCount    = 0,
                                                     OutputLevel = Lvl.INFO)
    AST99Job += makeHard

    # Convert GEN_HARD to TruthEvent with new names
    # Need to set TruthLinks to avoid conflict with standard name?!
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    AST99Job += xAODMaker__xAODTruthCnvAlg(
                         "GEN_HARD2xAOD",
                         AODContainerName               = "GEN_HARD",
                         xAODTruthEventContainerName    = "TruthHardEvents",
                         xAODTruthParticleContainerName = "TruthHardParticles",
                         xAODTruthVertexContainerName   = "TruthHardVertices",
                         TruthLinks                     = "TruthHardLinks",
                         OutputLevel                    = Lvl.INFO)

    # Set up HardTruthThinning tool
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HardTruthThinning
    # Keep B,D,tau decays
    cHadrons = [411,421,431, 4122,4132,4232,4332]
    bHadrons = [511,521,531, 5122,5132,5232,5332]
    tau      = [15]
    keepers  = cHadrons + bHadrons + tau
    # Keep particles in jets and around hard leptons
    TruthThinningTool = DerivationFramework__HardTruthThinning(
                      name            = "TruthThinningTool",
                      ThinningService = "AST99ThinningSvc",
                      EventInfo       = "McEventInfo",
                      TruthParticles  = "TruthParticles",
                      TruthVertices   = "TruthVertices",
                      HardParticles   = "TruthHardParticles",
                      KeepIds         = keepers,
                      JetName         = "AntiKt4TruthJets",
                      JetPtCut        = 20000,
                      JetEtaCut       = 2.5,
                      JetConstPtCut   = 1000,
                      IsolRadius      = 0.2,
                      IsolPtCut       = 1000,
                      MaxCount        = 0,
                      OutputLevel     = Lvl.INFO)

    # Declare to thinning service
    ToolSvc += TruthThinningTool
    thinningTools.append(TruthThinningTool)


############################
# Configure Trigger Skimming
############################

# From https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/trunk/share/skimmingExampleJobOptions.py

xeTrig  = ["HLT_xe70", "HLT_j80_xe80"]
jetTrig = ["HLT_j360", "HLT_4j85"]
lepTrig = ["HLT_e24_lhtight_iloose", "HLT_mu24"]
allTrig = xeTrig + jetTrig + lepTrig

cutExpression = '(' + ' || '.join(xeTrig) + ')'
AST99CutTool = CfgMgr.CutTool("AST99CutTool",
            TrigDecisionTool = trigDecTool,
                         Cut = cutExpression,
                 OutputLevel = Lvl.INFO)

ToolSvc += AST99CutTool
skimmingTools += [AST99CutTool]


####################
# Configure Slimming
####################

# DerivationFramework uses SlimmingHelper, which automatically
# handles variables for CP tools. We can use AddItem directly.
# Start with xxxCPContent; remove all not needed for analysis.
# Variables xxxVarAux are CP tool dependent.
# Variables xxxVarAux0 are independent - base container only.
# Add SUSYTools decorations to xxxVarAux.

# Jets
jetVarAux = ".pt.eta.phi.m"
jetVarAux += ".baseline.signal.passOR.bad.bjet.bjet_loose"
jetVarAux0 = ".JetEtaJESScaleMomentum_pt.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_m.JetGSCScaleMomentum_pt.JetGSCScaleMomentum_eta.JetGSCScaleMomentum_phi.JetGSCScaleMomentum_m.JvtJvfcorr.JvtRpt.Jvt.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.TrackWidthPt1000.Width.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.btagging.btaggingLink.GhostBHadronsFinal.GhostCHadronsFinal.GhostTausFinal.GhostTruth.OriginVertex.GhostTrack.HighestJVFVtx.ConeTruthLabelID.PartonTruthLabelID"

# Electrons
elVarAux = ".pt.eta.phi.m.Loose.Medium.Tight"
elVarAux += ".baseline.signal.passOR.isol"
elVarAux0 = ".trackParticleLinks.truthParticleLink.author.ptcone20.ptcone30.ptcone40.topoetcone20.topoetcone30.topoetcone40.charge"
if len(applyST.ElectronTriggers) > 0:
    elVarAux0 += ".match" + ".match".join(applyST.ElectronTriggers)

# Muons
muVarAux = ".pt.eta.phi.InnerDetectorPt.MuonSpectrometerPt.momentumBalanceSignificance.CaloLRLikelihood.CaloMuonIDTag"
muVarAux += ".baseline.signal.passOR.isol.cosmic.passedHighPtCuts"
muVarAux0 = ".truthType.truthOrigin.author.muonType.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.clusterLink.ptcone20.ptcone30.ptcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.charge.extrapolatedMuonSpectrometerTrackParticleLink"
if len(applyST.MuonTriggers) > 0:
    muVarAux0 += ".match" + ".match".join(applyST.MuonTriggers)

# InDet Tracks - minimal information
# vz NOT in InDetTrackParticlesCPContent?!
indetVarAux = ".phi.d0.z0.theta.qOverP.vz.definingParametersCovMatrix.numberOfPixelHits.numberOfSCTHits.vertexLink.numberOfInnermostPixelLayerHits.truthParticleLink"
indetVarAux += ".TrkIsoPt1000_ptcone20.TrkIsoPt1000_ptcone30.TrkIsoPt1000_ptcone40"
indetVarAux += ".TrkIsoPt500_ptcone20.TrkIsoPt500_ptcone30.TrkIsoPt500_ptcone40"

#gsfTrkVarAux = ".phi.d0.z0.theta.qOverP.vz.definingParametersCovMatrix"
gsfTrkVarAux = ".phi.d0.z0.theta.qOverP.vz"

gsfVtxVarAux = ".trackParticleLinks.x.y.z.px.py.pz.pt1.pt2..minRfirstHit"

# MuonSpec Tracks - what is really needed?
muspVarAux = ".phi.d0.z0.theta.qOverP.vz.parameterX.parameterY.parameterZ.parameterPX.parameterPY.parameterPZ.numberOfPrecisionLayers.numberOfPhiLayers.numberOfTriggerEtaLayers"

# BTagging - MV1 not in xAOD
btagVarAux = ".MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb"

# TauJets - recommendations from Serban
tauVarAux = ".pt.eta.phi.m"
tauVarAux += ".baseline.signal"
tauVarAux0 = ".trackLinks.jetLink.vertexLink.charge.isTauFlags.BDTJetScore.BDTEleScore.Likelihood.SafeLikelihood"

# Photons
phVarAux = ".pt.eta.phi.m"
phVarAux += ".baseline.signal"
phVarAux0 = ".vertexLinks.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.truthParticleLink.truthOrigin.truthType.author.Loose.Medium.Tight"

# Primary vertices - more than PrimaryVerticesCPContent but no track links
privtxVarAux = ".x.y.z.sumPt2.chiSquared.numberDoF.vertexType"


#########################
# Create Kernel Algorithm
#########################

# Run truth thinning and any skimming
# Object thinning done by ApplySUSYTools
# See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#Event_skimming_filtering

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
AST99Job += CfgMgr.DerivationFramework__DerivationKernel(
                                                   name = "AST99Kernel",
                                          SkimmingTools = skimmingTools,
                                          ThinningTools = thinningTools,
                                            OutputLevel = Lvl.INFO )

AST99Stream.AddAcceptAlgs( "AST99Kernel" )

# Need this to write TriggerMenu and other MetaData
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool("TriggerMenuMetaDataTool")
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]

# Should this use CutFlowHelpers??
from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
svcMgr += CutFlowSvc()
theApp.CreateSvc += ['CutFlowSvc/CutFlowSvc']


##########################
# Add Containers to Stream
##########################

# Add (Thinned/Slimmed) Base Containers
# DAOD contains EventAuxInfo and JetAuxContainer
# Many Base Containers are AuxContainerBase in DAOD.

AST99Stream.AddItem( "xAOD::EventInfo#EventInfo" )
AST99Stream.AddItem( "xAOD::EventAuxInfo#EventInfoAux." )

AST99Stream.AddItem( "xAOD::VertexContainer#PrimaryVertices" )
AST99Stream.AddItem( "xAOD::AuxContainerBase#PrimaryVerticesAux%s" % (privtxVarAux) )

AST99Stream.AddItem( "xAOD::JetContainer#%s" % (applyST.JetsName) )
#AST99Stream.AddItem( "xAOD::JetAuxContainer#%s" % (applyST.JetsName + "Aux" + jetVarAux + jetVarAux0) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.JetsName + "Aux" + jetVarAux + jetVarAux0) )

AST99Stream.AddItem( "xAOD::ElectronContainer#%s" % (applyST.ElectronsName) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.ElectronsName + "Aux" + elVarAux + elVarAux0 ) )

AST99Stream.AddItem( "xAOD::MuonContainer#%s" % (applyST.MuonsName) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.MuonsName + "Aux" + muVarAux + muVarAux0) )

AST99Stream.AddItem( "xAOD::BTaggingContainer#%s" % (applyST.BTaggingName) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.BTaggingName + "Aux" + btagVarAux) )

AST99Stream.AddItem( "xAOD::TauJetContainer#%s" % (applyST.TauJetsName) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.TauJetsName + "Aux" + tauVarAux + tauVarAux0) )

AST99Stream.AddItem( "xAOD::PhotonContainer#%s" % (applyST.PhotonsName) )
AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.PhotonsName + "Aux" + phVarAux + phVarAux0) )

if applyST.TruthJetsName != "":
    AST99Stream.AddItem( "xAOD::JetContainer#%s" % (applyST.TruthJetsName) )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.TruthJetsName + "Aux" + jetVarAux + jetVarAux0) )

if applyST.InDetTracksName != "":
    AST99Stream.AddItem( ["xAOD::TrackParticleContainer#%s" % (applyST.InDetTracksName)] )
    AST99Stream.AddItem( ["xAOD::AuxContainerBase#%s" % (applyST.InDetTracksName + "Aux" + indetVarAux)] )

if applyST.GSFTracksName != "":
    AST99Stream.AddItem( "xAOD::TrackParticleContainer#%s" % (applyST.GSFTracksName) )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.GSFTracksName + "Aux" + gsfTrkVarAux) )

if applyST.GSFVerticesName != "":
    AST99Stream.AddItem( "xAOD::VertexContainer#%s" % (applyST.GSFVerticesName) )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.GSFVerticesName + "Aux" + gsfVtxVarAux) )

if applyST.MuonSpecTracksName != "":
    AST99Stream.AddItem( "xAOD::TrackParticleContainer#%s" % (applyST.MuonSpecTracksName) )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#%s" % (applyST.MuonSpecTracksName + "Aux" + muspVarAux) )

# Trigger
AST99Stream.AddItem( "xAOD::TrigDecision#xTrigDecision" )
AST99Stream.AddItem( "xAOD::TrigDecisionAuxInfo#xTrigDecisionAux." )
AST99Stream.AddItem( "xAOD::TrigConfKeys#TrigConfKeys" )
AST99Stream.AddMetaDataItem( "xAOD::TriggerMenuContainer#*" )
AST99Stream.AddMetaDataItem( "xAOD::TriggerMenuAuxContainer#*" )

# Other MetaData
AST99Stream.AddMetaDataItem( "xAOD::CutBookkeeperContainer#*" )
AST99Stream.AddMetaDataItem( "xAOD::CutBookkeeperAuxContainer#*" )

# Add syst != "" variations with slimmed aux containers
# Slimmed base containers already added

for syst in jetsysnames:
    if syst != "":
        AST99Stream.AddItem( 'xAOD::JetContainer#%s' % (applyST.JetsName+syst) )
        AST99Stream.AddItem( 'xAOD::ShallowAuxContainer#%sAux%s' % (applyST.JetsName+syst, jetVarAux) )

for syst in elsysnames:
    if syst != "":
        AST99Stream.AddItem( 'xAOD::ElectronContainer#%s' % (applyST.ElectronsName+syst) )
        AST99Stream.AddItem( 'xAOD::ShallowAuxContainer#%sAux%s' % (applyST.ElectronsName+syst, elVarAux) )

for syst in musysnames:
    if syst != "":
        AST99Stream.AddItem( 'xAOD::MuonContainer#%s' % (applyST.MuonsName+syst) )
        AST99Stream.AddItem( 'xAOD::ShallowAuxContainer#%sAux%s' % (applyST.MuonsName+syst, muVarAux) )

# Full MET containers including syst = ""
AST99Stream.AddItem( 'xAOD::MissingETContainer#%s' % (metkey) )
AST99Stream.AddItem( 'xAOD::MissingETAuxContainer#%sAux.' % (metkey) )

for syst in jetsysnames:
    AST99Stream.AddItem( 'xAOD::MissingETContainer#%s' % (metkey+syst) )
    AST99Stream.AddItem( 'xAOD::MissingETAuxContainer#%sAux.' % (metkey+syst) )

if isMC:
    if applyST.DoTST :
        for syst in TSTsysnames:
            AST99Stream.AddItem( "xAOD::MissingETContainer#%s" % (metkey+syst) )
            AST99Stream.AddItem( "xAOD::MissingETAuxContainer#%sAux." % (metkey+syst) )
    else:
        for syst in CSTsysnames:
            AST99Stream.AddItem( "xAOD::MissingETContainer#%s" % (metkey+syst) )
            AST99Stream.AddItem( "xAOD::MissingETAuxContainer#%sAux." % (metkey+syst) )

# Track MET
AST99Stream.AddItem( 'xAOD::MissingETContainer#MET_Track' )
AST99Stream.AddItem( 'xAOD::AuxContainerBase#MET_TrackAux.' )

for syst in tausysnames:
    if syst != "":
        AST99Stream.AddItem( 'xAOD::TauJetContainer#%s' % (applyST.TauJetsName+syst) )
        AST99Stream.AddItem( 'xAOD::ShallowAuxContainer#%sAux%s' % (applyST.TauJetsName+syst, tauVarAux) )

for syst in phsysnames:
    if syst != "":
        AST99Stream.AddItem( 'xAOD::PhotonContainer#%s' % (applyST.PhotonsName+syst) )
        AST99Stream.AddItem( 'xAOD::ShallowAuxContainer#%sAux%s' % (applyST.PhotonsName+syst, phVarAux) )

# Truth MET
if isMC:
    AST99Stream.AddItem( 'xAOD::MissingETContainer#MET_Truth' )
    AST99Stream.AddItem( 'xAOD::AuxContainerBase#MET_TruthAux.' )

# Add thinned truth
if isMC:
    AST99Stream.AddItem( "xAOD::TruthVertexContainer#TruthVertices" )
    AST99Stream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#TruthVerticesAux." )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#TruthParticlesAux." )

# Add hard truth containers if made
if doTruth==2:
    AST99Stream.AddItem( "xAOD::TruthEventContainer#TruthHardEvents" )
    AST99Stream.AddItem( "xAOD::TruthEventAuxContainer#TruthHardEventsAux." )
    AST99Stream.AddItem( "xAOD::TruthParticleContainer#TruthHardParticles" )
    AST99Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthHardParticlesAux." )
    AST99Stream.AddItem( "xAOD::TruthVertexContainer#TruthHardVertices" )
    AST99Stream.AddItem( "xAOD::TruthVertexAuxContainer#TruthHardVerticesAux." )

# Add R=0.4 track jets - just kinematics
AST99Stream.AddItem( "xAOD::JetContainer#AntiKt4PV0TrackJets" )
AST99Stream.AddItem( "xAOD::JetAuxContainer#AntiKt4PV0TrackJetsAux.pt.eta.phi.m" )

# Add TruthTaus
if isMC:
    truthTauVar =  ".pt.eta.phi.m.pdgId.barcode.status.IsHadronicTau.DecayModeVector"
    truthTauVar += ".pt_vis.eta_vis.phi_vis.m_vis.numCharged"
    AST99Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
    AST99Stream.AddItem( "xAOD::AuxContainerBase#TruthTausAux.%s" % (truthTauVar) )

# Set event number
theApp.EvtMax=EVTMAX

