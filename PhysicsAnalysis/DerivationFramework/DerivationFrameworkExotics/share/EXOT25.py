#********************************************************************
# EXOT25.py (for 1-lepton)
# reductionConf flag EXOT25 in Reco_tf.py   
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
streamName = derivationFlags.WriteDAOD_EXOT25Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT25Stream )
EXOT25Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT25Stream.AcceptAlgs(["EXOT25Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
# EXOT25ThinningHelper_AntiKt10LCTopoJet = ThinningHelper("EXOT25ThinningHelperAntiKt10LCTopoJet")
EXOT25ThinningHelper = ThinningHelper("EXOT25ThinningHelper")
#trigger navigation content
EXOT25ThinningHelper.TriggerChains = 'HLT_e.*|HLT_mu.*|HLT_xe.*|HLT_j.*' 
EXOT25ThinningHelper.AppendToStream(EXOT25Stream) 

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common
thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('EXOT25',EXOT25ThinningHelper))
# thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrackParticleThinning('EXOT25',EXOT25ThinningHelper))
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'EXOT25',EXOT25ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'EXOT25',EXOT25ThinningHelper) )
thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'EXOT25',EXOT25ThinningHelper) )
thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'EXOT25',EXOT25ThinningHelper) )
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'EXOT25',EXOT25ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'EXOT25',EXOT25ThinningHelper) )
#thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'EXOT25',EXOT25ThinningHelper) )

thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('EXOT25',EXOT25ThinningHelper) )
#thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('EXOT25',EXOT25ThinningHelper) )

# MC truth thinning (not for data)
if DerivationFrameworkIsMonteCarlo :
    thinningTools.append(HIGG5Common.getTruthThinningTool('EXOT25', EXOT25ThinningHelper))

#========================================================================
# lepton selection (keep isolation failed leptons for QCD-MJ estimation)
#========================================================================
lepSel = '( count( (Muons.pt > 20.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon) ) + count( (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose) ) ) > 0'

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
EXOT25JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT25JetSkimmingTool",
                                                                      expression = jetSel)
ToolSvc += EXOT25JetSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT25LepSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT25LepSkimmingTool",
                                                                      expression = lepSel)
ToolSvc += EXOT25LepSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT25TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name          = "EXOT25TriggerSkimmingTool",
                                                                         TriggerListOR = triglist )
ToolSvc += EXOT25TriggerSkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
exot25Seq = CfgMgr.AthSequencer("EXOT25Sequence")


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

exot25Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT25Kernel_trig",
    SkimmingTools = [EXOT25TriggerSkimmingTool]
    )

exot25Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT25Kernel_lep",
    SkimmingTools = [EXOT25LepSkimmingTool]
    )


#====================================================================
# Standard jets
#====================================================================
if not "EXOT25Jets" in OutputJets:
    OutputJets["EXOT25Jets"] = []

    #AntiKt2PV0TrackJets
    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, exot25Seq, "EXOT25Jets")

    addDefaultTrimmedJets(exot25Seq,"EXOT25Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(exot25Seq,'EXOT25Jets') 

    #=======================================
    # TCC JETS
    #=======================================
#    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
    # Set up geometry and BField
#    import AthenaCommon.AtlasUnixStandardJob

#    include("RecExCond/AllDet_detDescr.py")
#    runTCCReconstruction(exot25Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")
#    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
#    addTCCTrimmedJets(exot25Seq, "EXOT25Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(exot25Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(exot25Seq, ToolSvc)

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=exot25Seq)
# # applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=exot25Seq)
# # applyJetCalibration_OTFJets("AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=exot25Seq)



#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
exot25Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


exot25Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT25Kernel_jet",
    SkimmingTools = [EXOT25JetSkimmingTool]
    )

exot25Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT25Kernel",
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += exot25Seq



# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="EXOT25ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D2ContentList import HIGG5D2Content
from DerivationFrameworkHiggs.HIGG5D2ExtraContent import *
EXOT25SlimmingHelper = SlimmingHelper("EXOT25SlimmingHelper")

EXOT25SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"               :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"            :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"       :   "xAOD::BTaggingAuxContainer",
  }

EXOT25SlimmingHelper.SmartCollections = [ "Electrons",
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
    EXOT25SlimmingHelper.SmartCollections += [
         "AntiKt4TruthJets",
         "AntiKt4TruthWZJets"
#          ,"AntiKt10TruthWZTrimmedPtFrac5SmallR20Jets"
    ]


EXOT25SlimmingHelper.ExtraVariables = ExtraContent
EXOT25SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
EXOT25SlimmingHelper.AllVariables = ExtraContainers
# EXOT25SlimmingHelper.AllVariables += ["AntiKtVR30Rmax4Rmin02TrackJets", "BTagging_AntiKtVR30Rmax4Rmin02Track"]
if DerivationFrameworkIsMonteCarlo :
    EXOT25SlimmingHelper.ExtraVariables += ExtraContentTruth
    EXOT25SlimmingHelper.AllVariables += ExtraContainersTruth
EXOT25SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["EXOT25Jets",
                 "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#        ,"AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"
        ]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]
# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(EXOT25SlimmingHelper,["EXOT25Jets"],slimmed_content,['AntiKt4PV0TrackJets'])

EXOT25SlimmingHelper.IncludeMuonTriggerContent = True
EXOT25SlimmingHelper.IncludeEGammaTriggerContent = True
#EXOT25SlimmingHelper.IncludeBPhysTriggerContent = True
#EXOT25SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT25SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT25SlimmingHelper.IncludeJetTriggerContent = True
# if globalflags.DataSource()!='geant4': # for very early data
# EXOT25SlimmingHelper.IncludeBJetTriggerContent = True

EXOT25SlimmingHelper.AppendContentToStream(EXOT25Stream)
