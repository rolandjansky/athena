#********************************************************************
# HIGG5D2.py (for 1-lepton)
# reductionConf flag HIGG5D2 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import (
  addRecommendedXbbTaggers, xbbTaggerExtraVariables
)
# from DerivationFrameworkJetEtMiss.JetMomentFix import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *

if DerivationFrameworkIsMonteCarlo: 
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

# running on data or MC
# from AthenaCommon.GlobalFlags import globalflags
# print "DEBUG is MC ? ",DerivationFrameworkIsMonteCarlo

if DerivationFrameworkIsMonteCarlo :
  from DerivationFrameworkHiggs.TruthCategories import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D2Stream )
HIGG5D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D2Stream.AcceptAlgs(["HIGG5D2Kernel","HIGG5D2KernelTruth"])

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common


#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
# HIGG5D2ThinningHelper_AntiKt10LCTopoJet = ThinningHelper("HIGG5D2ThinningHelperAntiKt10LCTopoJet")
HIGG5D2ThinningHelper = ThinningHelper("HIGG5D2ThinningHelper")
#trigger navigation content
HIGG5D2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_xe.*|HLT_j.*' 
HIGG5D2ThinningHelper.AppendToStream(HIGG5D2Stream) 

# thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('HIGG5D2',HIGG5D2ThinningHelper))
# thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrackParticleThinning('HIGG5D2',HIGG5D2ThinningHelper))
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'HIGG5D2',HIGG5D2ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'HIGG5D2',HIGG5D2ThinningHelper) )
# thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'HIGG5D2',HIGG5D2ThinningHelper) )
# thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'HIGG5D2',HIGG5D2ThinningHelper) )
thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'HIGG5D2',HIGG5D2ThinningHelper) )

#Track Particles + CaloCluster thinning
thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('HIGG5D2',HIGG5D2ThinningHelper) )

#calocluster thinning
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'HIGG5D2',HIGG5D2ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'HIGG5D2',HIGG5D2ThinningHelper) )

#generic jet thinning
thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('HIGG5D2',HIGG5D2ThinningHelper) )
if DerivationFrameworkIsMonteCarlo :
  thinningTools.append( HIGG5Common.getTruthThinningTool('HIGG5D2',HIGG5D2ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthTrimmedPtFrac5SmallR20Thinning('HIGG5D2',HIGG5D2ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthWZTrimmedPtFrac5SmallR20Thinning('HIGG5D2',HIGG5D2ThinningHelper) )

#========================================================================
# lepton selection (keep isolation failed leptons for QCD-MJ estimation)
#========================================================================
lepSel = '( count( (Muons.pt > 20.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon) ) + count( (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose) ) ) > 0'

#====================================================================
# jet selection 
#====================================================================
jetSel = '(( count( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMPFlowJets.pt > 15.*GeV) && (abs(AntiKt4EMPFlowJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoJets.pt > 150.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 150.0*GeV) && (abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'


#====================================================================
# Trigger selection
#====================================================================
beamEnergy = jobproperties.Beam.energy()
triglist = []
if (beamEnergy < 4.1e+06): # 8 TeV, name should be EF_xxx in Rel19, HLT_xxx in Rel20
    triglist.append("HLT_e24vhi_medium1")
    triglist.append("HLT_e60_medium1")
    triglist.append("HLT_mu24i_tight")
    triglist.append("HLT_mu36_tight")
    triglist.append("HLT_xe100")
    triglist.append("HLT_xe50_j40_dphi1")
    triglist.append("HLT_xe80T_tclcw_loose")
    triglist.append("HLT_xe80_tclcw_loose")
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
    triglist.append("HLT_e17_lhloose_L1EM15")
    triglist.append("HLT_e24_medium_iloose_L1EM18VH")
    triglist.append("HLT_e24_medium_iloose_L1EM20VH")
    triglist.append("HLT_e24_lhmedium_iloose_L1EM20VH")
    triglist.append("HLT_e24_tight_iloose")
    triglist.append("HLT_e24_lhmedium_L1EM20VHI")
    triglist.append("HLT_e24_lhmedium_L1EM20VH")
    triglist.append("HLT_e24_lhmedium_L1EM18VH")
    triglist.append("HLT_e24_lhmedium_ivarloose")
    triglist.append("HLT_e24_lhmedium_nod0_ivarloose")
    triglist.append("HLT_e24_lhmedium_nod0_L1EM20VH")
    triglist.append("HLT_e24_lhtight_iloose")
    triglist.append("HLT_e24_lhtight_ivarloose")
    triglist.append("HLT_e24_lhtight_nod0_ivarloose")
    triglist.append("HLT_e26_tight_iloose")
    triglist.append("HLT_e26_tight1_iloose")
    triglist.append("HLT_e26_lhtight_iloose")
    triglist.append("HLT_e26_lhtight_iloose")
    triglist.append("HLT_e26_lhtight_ivarloose")
    triglist.append("HLT_e26_lhtight_nod0_ivarloose")
    triglist.append("HLT_e26_lhtight_smooth_ivarloose") # added on Aug 2016
    triglist.append("HLT_e60_medium")
    triglist.append("HLT_e60_medium1")
    triglist.append("HLT_e60_lhmedium")
    triglist.append("HLT_e60_lhmedium_nod0")
    triglist.append("HLT_e120_lhloose")
    triglist.append("HLT_e140_lhloose")
    triglist.append("HLT_e140_lhloose_nod0")
    triglist.append("HLT_e300_etcut")
    triglist.append("HLT_mu14")
    triglist.append("HLT_mu20_iloose_L1MU15")
    triglist.append("HLT_mu20_ivarloose_L1MU15")
    triglist.append("HLT_mu24_iloose_L1MU15")
    triglist.append("HLT_mu24_iloose")
    triglist.append("HLT_mu24_ivarloose")
    triglist.append("HLT_mu24_ivarloose_L1MU15")
    triglist.append("HLT_mu24_imedium")
    triglist.append("HLT_mu24_ivarmedium")
    triglist.append("HLT_mu26_imedium")
    triglist.append("HLT_mu26_ivarmedium")
    triglist.append("HLT_mu40")
    triglist.append("HLT_mu50")
    triglist.append("HLT_xe35")
    triglist.append("HLT_xe50")
    triglist.append("HLT_xe60")
    triglist.append("HLT_xe70")
    triglist.append("HLT_xe80")
    triglist.append("HLT_xe80_L1XE50")
    triglist.append("HLT_xe80_L1XE70")
    triglist.append("HLT_xe100")
    triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20XE50")
    triglist.append("HLT_j80_xe80_dphi1_L1J40_DPHI-J20s2XE50")
    triglist.append("HLT_j100_xe80_L1J40_DPHI-J20XE50")
    triglist.append("HLT_j100_xe80_L1J40_DPHI-J20s2XE50")
    triglist.append("HLT_xe80_tc_lcw_L1XE50") # added on Apr 2016
    triglist.append("HLT_xe90_tc_lcw_L1XE50")
    triglist.append("HLT_xe100_tc_lcw_L1XE50")
    triglist.append("HLT_xe110_tc_lcw_L1XE60")
    triglist.append("HLT_xe80_mht_L1XE50")
    triglist.append("HLT_xe90_mht_L1XE50")
    triglist.append("HLT_xe100_mht_L1XE50")
    triglist.append("HLT_xe100_mht_L1XE60")
    triglist.append("HLT_xe110_mht_L1XE50") # added on Aug 2016
    triglist.append("HLT_xe110_mht_L1XE50_AND_xe70_L1XE50") # added on Sep 2016
    triglist.append("HLT_xe130_mht_L1XE50") # added on Aug 2016
    triglist.append("HLT_xe90_L1XE50")
    triglist.append("HLT_xe100_L1XE50")
    triglist.append("HLT_xe110_L1XE60")
    triglist.append("HLT_xe80_tc_em_L1XE50")
    triglist.append("HLT_xe90_tc_em_L1XE50")
    triglist.append("HLT_xe100_tc_em_L1XE50")
    triglist.append("HLT_xe80_tc_lcw")
    triglist.append("HLT_xe90_tc_lcw")
    triglist.append("HLT_xe100_tc_lcw")
    triglist.append("HLT_xe90_mht")
    triglist.append("HLT_xe100_mht")
    triglist.append("HLT_xe90_tc_lcw_wEFMu_L1XE50")
    triglist.append("HLT_xe90_mht_wEFMu_L1XE50")
    triglist.append("HLT_xe120_pueta")
    triglist.append("HLT_xe120_pufit")
    triglist.append("HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20")
    triglist.append("HLT_mu14_ivarloose_3j20_L1MU10_3J20")
    triglist.append("HLT_xe100_tc_lcw_L1XE60") # added on Jun 2016
    triglist.append("HLT_xe110_tc_em_L1XE50")
    triglist.append("HLT_xe110_tc_em_wEFMu_L1XE50")
    triglist.append("HLT_xe120_pueta_wEFMu")
    triglist.append("HLT_xe120_mht")
    triglist.append("HLT_xe120_tc_lcw")
    triglist.append("HLT_xe120_mht_wEFMu")
    triglist.append("HLT_xe110_L1XE50")
    triglist.append("HLT_xe100_L1XE60")
    triglist.append("HLT_xe120_pufit_wEFMu")
    triglist.append("HLT_xe120_tc_lcw_wEFMu")
    triglist.append("HLT_xe120_tc_em")
    triglist.append("HLT_noalg_L1J400") # added on Nov 2016
# print 'DEBUG projectName=%s [%s;%s]' % (rec.projectName.get_Value(),type(rec.projectName),dir(rec.projectName))
if (beamEnergy > 6.0e+06 and rec.projectName.get_Value() in ['data17_13TeV','data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV','IS_SIMULATION']) :  # 13 TeV,   and project 2017:
    triglist.append("HLT_e28_lhtight_nod0_ivarloose")
    triglist.append("HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM")
    triglist.append("HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM")
    triglist.append("HLT_e60_lhmedium_nod0_L1EM24VHI")
    triglist.append("HLT_e140_lhloose_nod0_L1EM24VHI")
    triglist.append("HLT_e300_etcut_L1EM24VHI")
    triglist.append("HLT_mu60")
    triglist.append("HLT_xe110_pufit_L1XE60")
    triglist.append("HLT_xe120_pufit_L1XE60")
    triglist.append("HLT_xe120_mht_xe80_L1XE60")
    triglist.append("HLT_xe110_pufit_L1XE55")
    triglist.append("HLT_xe120_pufit_L1XE55")
    triglist.append("HLT_xe120_mht_xe80_L1XE55")
    triglist.append("HLT_xe110_pufit_L1XE50")
    triglist.append("HLT_xe120_pufit_L1XE50")
    triglist.append("HLT_xe120_mht_xe80_L1XE50")
    if rec.projectName.get_Value() in ['data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV','IS_SIMULATION'] :  # 13 TeV,   and project 2017:
       triglist.append("HLT_xe110_pufit_xe65_L1XE50")
       triglist.append("HLT_xe110_pufit_xe70_L1XE50")
       triglist.append("HLT_xe120_pufit_L1XE50")

    # triglist.append("L1_EM18VH")
    # triglist.append("L1_EM20VH")
    # triglist.append("L1_EM20VHI")
    # triglist.append("L1_EM22VHI")
    # triglist.append("L1_MU15")
    # triglist.append("L1_MU20")
    # triglist.append("L1_XE50")
    # triglist.append("L1_XE60")
    # triglist.append("L1_XE70")


#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D2JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D2JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += HIGG5D2JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D2LepSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D2LepSkimmingTool",
                                                                      expression = lepSel)
ToolSvc += HIGG5D2LepSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG5D2TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "HIGG5D2TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += HIGG5D2TriggerSkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d2Seq = CfgMgr.AthSequencer("HIGG5D2Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_trig",
    SkimmingTools = [HIGG5D2TriggerSkimmingTool]
    )

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_lep",
    SkimmingTools = [HIGG5D2LepSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "HIGG5D2Jets" in OutputJets:
    OutputJets["HIGG5D2Jets"] = []

    # have to run TCC reconstruction prior to TCC jet finding
    from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
    # Set up geometry and BField
    import AthenaCommon.AtlasUnixStandardJob

    include("RecExCond/AllDet_detDescr.py")
    runTCCReconstruction(higg5d2Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles")

    #AntiKt2PV0TrackJets
    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets", 'AntiKt10TrackCaloClusterJets']
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, higg5d2Seq, "HIGG5D2Jets")

    addDefaultTrimmedJets(higg5d2Seq,"HIGG5D2Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(higg5d2Seq,'HIGG5D2Jets') 

    # and also add thr trimmed tcc jets
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(higg5d2Seq, "HIGG5D2Jets")

HIGG5Common.addAntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub(higg5d2Seq)

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(higg5d2Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(higg5d2Seq, ToolSvc)

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg5d2Seq)
# FlavorTagInit(JetCollections = ['AntiKt4PV0TrackJets'], Sequencer = higg5d2Seq)

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d2Seq)
# # applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d2Seq)
# # applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=higg5d2Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d2Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

#====================================================================
# SET UP CUSTOM TRUTH3 INFORMATION (only for higg5D2Kernel)
#====================================================================
if DerivationFrameworkIsMonteCarlo :
  HIGG5Common.getTruth3Collections(higg5d2Seq)

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D2Kernel_jet",
    SkimmingTools = [HIGG5D2JetSkimmingTool]
    )

higg5d2Seq += CfgMgr.DerivationFramework__DerivationKernel(
  "HIGG5D2Kernel",
  ThinningTools = thinningTools
  )

DerivationFrameworkJob += higg5d2Seq





# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="HIGG5D2ThinningSvc", outStreams=[evtStream] )

# QGTaggerTool ###
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=higg5d2Seq, algname="QGTaggerToolAlg")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D2ContentList import HIGG5D2Content
from DerivationFrameworkHiggs.HIGG5D2ExtraContent import *
HIGG5D2SlimmingHelper = SlimmingHelper("HIGG5D2SlimmingHelper")

HIGG5D2SlimmingHelper.AppendToDictionary = HIGG5Common.getHIGG5CommonDictionExtionson()
HIGG5D2SlimmingHelper.SmartCollections =   HIGG5Common.getHIGG5CommonSmartCollections()

HIGG5D2SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D2SlimmingHelper.ExtraVariables = xbbTaggerExtraVariables
HIGG5D2SlimmingHelper.AllVariables = ExtraContainers
# HIGG5D2SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if DerivationFrameworkIsMonteCarlo :
    HIGG5D2SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D2SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG5D2SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["HIGG5D2Jets",
                 "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                 "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"
        ]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]
# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(HIGG5D2SlimmingHelper,["HIGG5D2Jets"],slimmed_content,['AntiKt4PV0TrackJets'])
addOriginCorrectedClusters(HIGG5D2SlimmingHelper, writeLC=True, writeEM=False)

HIGG5D2SlimmingHelper.IncludeMuonTriggerContent         = True
HIGG5D2SlimmingHelper.IncludeEGammaTriggerContent       = True
# HIGG5D2SlimmingHelper.IncludeBPhysTriggerContent        = True
# HIGG5D2SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
HIGG5D2SlimmingHelper.IncludeEtMissTriggerContent       = True
HIGG5D2SlimmingHelper.IncludeJetTriggerContent          = True
# if globalflags.DataSource()!='geant4': # for very early data
#   HIGG5D2SlimmingHelper.IncludeBJetTriggerContent       = True

HIGG5D2SlimmingHelper.AppendContentToStream(HIGG5D2Stream)
