#********************************************************************
# HIGG5D1.py (for 0-lepton)
# reductionConf flag HIGG5D1 in Reco_tf.py
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
from AthenaCommon.GlobalFlags import globalflags
# print "DEBUG is MC ? ",DerivationFrameworkIsMonteCarlo

if DerivationFrameworkIsMonteCarlo :
  from DerivationFrameworkHiggs.TruthCategories import *


#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D1Stream )
HIGG5D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D1Stream.AcceptAlgs(["HIGG5D1Kernel"])

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG5D1ThinningHelper = ThinningHelper("HIGG5D1ThinningHelper")
#trigger navigation content
HIGG5D1ThinningHelper.TriggerChains = 'HLT_xe.*|HLT_j.*|HLT_g.*'
HIGG5D1ThinningHelper.AppendToStream(HIGG5D1Stream)

#TrackParticles thinning
# thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('HIGG5D1',HIGG5D1ThinningHelper) )
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'HIGG5D1',HIGG5D1ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'HIGG5D1',HIGG5D1ThinningHelper) )
# thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'HIGG5D1',HIGG5D1ThinningHelper) )
thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'HIGG5D1',HIGG5D1ThinningHelper) )

#Track Particles + CaloCluster thinning
thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('HIGG5D1',HIGG5D1ThinningHelper) )

#calocluster thinning
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'HIGG5D1',HIGG5D1ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'HIGG5D1',HIGG5D1ThinningHelper) )

#generic object thinning
thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('HIGG5D1',HIGG5D1ThinningHelper) )
if DerivationFrameworkIsMonteCarlo :
  thinningTools.append( HIGG5Common.getTruthThinningTool('HIGG5D1',HIGG5D1ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthTrimmedPtFrac5SmallR20Thinning('HIGG5D1',HIGG5D1ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthWZTrimmedPtFrac5SmallR20Thinning('HIGG5D1',HIGG5D1ThinningHelper) )

# MC truth thinning (not for data)
# if DerivationFrameworkIsMonteCarlo :
#  from DerivationFrameworkHiggs.TruthCategories import *
#  # thinningTools.append(HIGG5Common.getTruthThinningTool('HIGG5D1', HIGG5D1ThinningHelper))
#  #add Truth3 information
#  # from DerivationFrameworkMCTruth.MCTruthCommon import *
#  # addStandardTruthContents()

#====================================================================
# jet selection
#====================================================================
jetSel = '(( count( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMPFlowJets.pt > 15.*GeV) && (abs(AntiKt4EMPFlowJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoJets.pt > 150.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 150.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'

#====================================================================
# Trigger selection
#====================================================================
beamEnergy = jobproperties.Beam.energy()
triglist = []
if (beamEnergy < 4.1e+06): # 8 TeV, name should be EF_xxx in Rel19, HLT_xxx in Rel20
    #trigSel = '(EventInfo.eventTypeBitmask==1) || EF_xe100 || EF_xe50_j40_dphi1 || EF_xe80T_tclcw_loose || EF_xe80_tclcw_loose'
    triglist.append("HLT_xe100")
    triglist.append("HLT_xe50_j40_dphi1")
    triglist.append("HLT_xe80T_tclcw_loose")
    triglist.append("HLT_xe80_tclcw_loose")
if (beamEnergy > 6.0e+06): # 13 TeV, name should be HLT_xxx
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
    triglist.append("HLT_g120_loose")
    triglist.append("HLT_g140_loose") # added on Sep 2016
    triglist.append("HLT_g160_loose") # added on Sep 2016
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
if (beamEnergy > 6.0e+06 and rec.projectName.get_Value() in ['data17_13TeV','data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV','IS_SIMULATION']) :  # 13 TeV,   and projec
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
    if rec.projectName.get_Value() in ['data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV', 'IS_SIMULATION'] :  # 13 TeV,   and project 2017:
       triglist.append("HLT_xe110_pufit_xe65_L1XE50")
       triglist.append("HLT_xe110_pufit_xe70_L1XE50")
       triglist.append("HLT_xe120_pufit_L1XE50")

    # triglist.append("L1_XE50")
    # triglist.append("L1_XE60")
    # triglist.append("L1_XE70")


#====================================================================
# SKIMMING TOOL
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG5D1JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG5D1JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += HIGG5D1JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG5D1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "HIGG5D1TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += HIGG5D1TriggerSkimmingTool



#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d1Seq = CfgMgr.AthSequencer("HIGG5D1Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel_trig",
    SkimmingTools = [HIGG5D1TriggerSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "HIGG5D1Jets" in OutputJets:
    OutputJets["HIGG5D1Jets"] = []

    # TCC JETS reconstruction
    from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
    # Set up geometry and BField
    import AthenaCommon.AtlasUnixStandardJob

    include("RecExCond/AllDet_detDescr.py")
    runTCCReconstruction(higg5d1Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles")

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets", 'AntiKt10TrackCaloClusterJets']
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, higg5d1Seq, "HIGG5D1Jets")

    addDefaultTrimmedJets(higg5d1Seq,"HIGG5D1Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(higg5d1Seq,'HIGG5D1Jets')

    # TCC trimmed jets
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(higg5d1Seq, "HIGG5D1Jets")

HIGG5Common.addAntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub(higg5d1Seq)

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(higg5d1Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(higg5d1Seq, ToolSvc)

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg5d1Seq)
# FlavorTagInit(JetCollections = ['AntiKt4PV0TrackJets'], Sequencer = higg5d1Seq)

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d1Seq)
# # applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d1Seq)
# # applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=higg5d1Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg5d1Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

#====================================================================
# SET UP CUSTOM TRUTH3 INFORMATION
#====================================================================
if DerivationFrameworkIsMonteCarlo :
  HIGG5Common.getTruth3Collections(higg5d1Seq)

higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel_jet",
    SkimmingTools = [HIGG5D1JetSkimmingTool]
    )

higg5d1Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D1Kernel",
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += higg5d1Seq


# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="HIGG5D1ThinningSvc", outStreams=[evtStream] )

# QGTaggerTool ###
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=higg5d1Seq, algname="QGTaggerToolAlg")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D1ContentList import HIGG5D1Content
from DerivationFrameworkHiggs.HIGG5D1ExtraContent import *
HIGG5D1SlimmingHelper = SlimmingHelper("HIGG5D1SlimmingHelper")

HIGG5D1SlimmingHelper.AppendToDictionary = HIGG5Common.getHIGG5CommonDictionExtionson()
HIGG5D1SlimmingHelper.SmartCollections   = HIGG5Common.getHIGG5CommonSmartCollections()

HIGG5D1SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D1SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
HIGG5D1SlimmingHelper.AllVariables = ExtraContainers
# HIGG5D1SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if DerivationFrameworkIsMonteCarlo :
    HIGG5D1SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D1SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG5D1SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["HIGG5D1Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]

# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(HIGG5D1SlimmingHelper,["HIGG5D1Jets"],slimmed_content,['AntiKt4PV0TrackJets'])
addOriginCorrectedClusters(HIGG5D1SlimmingHelper, writeLC=True, writeEM=False)

# if not DerivationFrameworkIsMonteCarlo : # for very early data
#   HIGG5D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEGammaTriggerContent = True
# HIGG5D1SlimmingHelper.IncludeBJetTriggerContent = True
# HIGG5D1SlimmingHelper.IncludeBPhysTriggerContent = True
# HIGG5D1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG5D1SlimmingHelper.IncludeJetTriggerContent    = True

HIGG5D1SlimmingHelper.AppendContentToStream(HIGG5D1Stream)
