#********************************************************************
# EXOT24.py (for 0-lepton)
# reductionConf flag EXOT24 in Reco_tf.py   
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
streamName = derivationFlags.WriteDAOD_EXOT24Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT24Stream )
EXOT24Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT24Stream.AcceptAlgs(["EXOT24Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
EXOT24ThinningHelper = ThinningHelper("EXOT24ThinningHelper") 
#trigger navigation content
EXOT24ThinningHelper.TriggerChains = 'HLT_xe.*|HLT_j.*|HLT_g.*' 
EXOT24ThinningHelper.AppendToStream(EXOT24Stream)

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common
thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'EXOT24',EXOT24ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'EXOT24',EXOT24ThinningHelper) )
#thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'EXOT24',EXOT24ThinningHelper) )

thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('EXOT24',EXOT24ThinningHelper) )
#thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('EXOT24',EXOT24ThinningHelper) )

# MC truth thinning (not for data)
if DerivationFrameworkIsMonteCarlo :
    thinningTools.append(HIGG5Common.getTruthThinningTool('EXOT24', EXOT24ThinningHelper))

#====================================================================
# jet selection 
#====================================================================
jetSel = '(( count( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15.*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt4EMTopoJets.pt > 100.0*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoJets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoJets.eta) < 2.6) ) ) > 0)'
jetSel += '|| (( count( (AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
#jetSel += '|| (( count( (AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'

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
# print 'DEBUG projectName=%s [%s;%s]' % (rec.projectName.get_Value(),type(rec.projectName),dir(rec.projectName))
if (beamEnergy > 6.0e+06 and rec.projectName.get_Value() in ['data17_13TeV','data18_13TeV','mc16_13TeV', 'mc17_13TeV', 'mc18_13TeV']) :  # 13 TeV,   and project 2017:
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

    # triglist.append("L1_XE50")
    # triglist.append("L1_XE60")
    # triglist.append("L1_XE70")


#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT24JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT24JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += EXOT24JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT24TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "EXOT24TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += EXOT24TriggerSkimmingTool



#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
exot24Seq = CfgMgr.AthSequencer("EXOT24Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

exot24Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT24Kernel_trig",
    SkimmingTools = [EXOT24TriggerSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "EXOT24Jets" in OutputJets:
    OutputJets["EXOT24Jets"] = []

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, exot24Seq, "EXOT24Jets")

    addDefaultTrimmedJets(exot24Seq,"EXOT24Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(exot24Seq,'EXOT24Jets')

    #=======================================
    # TCC JETS
    #=======================================
#    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
    # Set up geometry and BField
#    import AthenaCommon.AtlasUnixStandardJob

#    include("RecExCond/AllDet_detDescr.py")
#    runTCCReconstruction(exot24Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")
#    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
#    addTCCTrimmedJets(exot24Seq, "EXOT24Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(exot24Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(exot24Seq, ToolSvc)

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=exot24Seq)
# # applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=exot24Seq)
# # applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=exot24Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
exot24Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


exot24Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT24Kernel_jet",
    SkimmingTools = [EXOT24JetSkimmingTool]
    )

exot24Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT24Kernel",
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += exot24Seq


# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="EXOT24ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D1ContentList import EXOT24Content
from DerivationFrameworkHiggs.HIGG5D1ExtraContent import *
EXOT24SlimmingHelper = SlimmingHelper("EXOT24SlimmingHelper")

EXOT24SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"               :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"            :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"       :   "xAOD::BTaggingAuxContainer",
  }

EXOT24SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Photons",
                                           "Muons",
                                           "TauJets",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                           "AntiKt4TruthJets",
#                                            "AntiKtVR30Rmax4Rmin02Track",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt2Track",
#                                           "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]
if DerivationFrameworkIsMonteCarlo :
    EXOT24SlimmingHelper.SmartCollections += [
         "AntiKt4TruthJets",
         "AntiKt4TruthWZJets"
#          ,"AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets"
    ]



EXOT24SlimmingHelper.ExtraVariables = ExtraContent
EXOT24SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
EXOT24SlimmingHelper.AllVariables = ExtraContainers
# EXOT24SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if DerivationFrameworkIsMonteCarlo :
    EXOT24SlimmingHelper.ExtraVariables += ExtraContentTruth
    EXOT24SlimmingHelper.AllVariables += ExtraContainersTruth
EXOT24SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["EXOT24Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]

# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(EXOT24SlimmingHelper,["EXOT24Jets"],slimmed_content,['AntiKt4PV0TrackJets'])

# if not DerivationFrameworkIsMonteCarlo : # for very early data
# EXOT24SlimmingHelper.IncludeMuonTriggerContent = True
EXOT24SlimmingHelper.IncludeEGammaTriggerContent = True
# EXOT24SlimmingHelper.IncludeBJetTriggerContent = True
# EXOT24SlimmingHelper.IncludeBPhysTriggerContent = True
# EXOT24SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT24SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT24SlimmingHelper.IncludeJetTriggerContent = True

EXOT24SlimmingHelper.AppendContentToStream(EXOT24Stream)
