#====================================================================
# EXOT26.py for 2L2Q filter
# This requires the reductionConf flag EXOT26 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import (
  addRecommendedXbbTaggers, xbbTaggerExtraVariables
)
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkHiggs.TruthCategories import *
import AthenaCommon.SystemOfUnits as Units

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==================================================================== 
# SET UP STREAM 
#==================================================================== 
streamName = derivationFlags.WriteDAOD_EXOT26Stream.StreamName 
fileName = buildFileName(derivationFlags.WriteDAOD_EXOT26Stream) 
EXOT26Stream = MSMgr.NewPoolRootStream( streamName, fileName) 
EXOT26Stream.AcceptAlgs(["EXOT26Kernel"]) 

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
EXOT26ThinningHelper = ThinningHelper("EXOT26ThinningHelper") 
#trigger navigation content  
EXOT26ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*|HLT_xe.*|HLT_j.*|HLT_b.*' 
EXOT26ThinningHelper.AppendToStream(EXOT26Stream) 


import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common
thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('EXOT26',EXOT26ThinningHelper) )
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'EXOT26',EXOT26ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'EXOT26',EXOT26ThinningHelper,   BestMatchOnly=False) )
thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'EXOT26',EXOT26ThinningHelper) )
thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'EXOT26',EXOT26ThinningHelper) )
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'EXOT26',EXOT26ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'EXOT26',EXOT26ThinningHelper) )
#thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'EXOT26',EXOT26ThinningHelper) )

thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('EXOT26',EXOT26ThinningHelper) )
#thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('EXOT26',EXOT26ThinningHelper) )

# Truth particles
if DerivationFrameworkIsMonteCarlo:
    thinningTools.append(HIGG5Common.getTruthThinningTool('EXOT26', EXOT26ThinningHelper))

print "EXOT26.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
beamEnergy = jobproperties.Beam.energy()
print "EXOT26.py jobproperties.Beam.energy()", beamEnergy

# 13 TeV
singleElectronTriggerRequirement=["HLT_e.*"]
diElectronTriggerRequirement=["HLT_2e.*"]
singleMuonTriggerRequirement=["HLT_mu.*"]
diMuonTriggerRequirement=["HLT_2mu.*"]
metTriggerRequirement=[]
# add MET trigger to recover muon trigger efficiency at high pt
if True :
    triglist=[]
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
    metTriggerRequirement+=triglist


electronMuonTriggerRequirement=[]
if beamEnergy==4000000.0:
    # 8 TeV
    singleElectronTriggerRequirement=["EF_e24vhi_medium1", "EF_e60_medium1"]
    diElectronTriggerRequirement=["EF_2e12Tvh_loose1", "EF_2e12Tvh_loose1_L2StarB"]
    singleMuonTriggerRequirement=["EF_mu24i_tight", "EF_mu36_tight"]
    diMuonTriggerRequirement=["EF_2mu13", "EF_mu18_tight_mu8_EFFS"]
    electronMuonTriggerRequirement=["EF_e12Tvh_medium1_mu8", "EF_e24vhi_loose1_mu8"]
triggerRequirement=singleElectronTriggerRequirement+diElectronTriggerRequirement+singleMuonTriggerRequirement+diMuonTriggerRequirement+electronMuonTriggerRequirement+metTriggerRequirement
# 8 TeV MC does not have trigger information
SkipTriggerRequirement=(DerivationFrameworkIsMonteCarlo and (beamEnergy==4000000.0))
print "EXOT26.py SkipTriggerRequirement", SkipTriggerRequirement
if SkipTriggerRequirement:
    triggerRequirement=[]
print "EXOT26.py triggerRequirement", triggerRequirement

# Pre-selection for di-leptons
electronPtRequirement='(Electrons.pt>6.*GeV)'
electronRequirement='('+electronPtRequirement+')'
#electronQualityRequirement='(Electrons.DFCommonElectronsLHVeryLoose)'
#electronRequirement='('+electronQualityRequirement+'&&'+electronPtRequirement+')'
combinedMuonQualityRequirement='(Muons.muonType==0)'
standaloneMuonQualityRequirement='(Muons.muonType==1 && abs(abs(Muons.eta)-2.6)<0.12)'
segmentTaggedMuonQualityRequirement='(Muons.muonType==2)'
calorimeterTaggedMuonQualityRequirement='(Muons.muonType==3 && abs(Muons.eta)<0.2)'
muonQualityRequirement='('+combinedMuonQualityRequirement+'||'+standaloneMuonQualityRequirement+'||'+segmentTaggedMuonQualityRequirement+'||'+calorimeterTaggedMuonQualityRequirement+')'
muonPtRequirement='(Muons.pt>6.*GeV)'
muonRequirement='('+muonQualityRequirement+'&&'+muonPtRequirement+')'
leptonRequirement='(count('+electronRequirement+')+count('+muonRequirement+')>= 2)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SkimmingToolEXOT26_preLep = DerivationFramework__xAODStringSkimmingTool(name = "SkimmingToolEXOT26_preLep",
                                                                         expression = leptonRequirement)
ToolSvc += SkimmingToolEXOT26_preLep

# @TODO take TCC jets into account for skimming
# jetSel = '|| (( count( (AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 100.0*GeV) && (abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolEXOT26 = DerivationFramework__SkimmingToolHIGG2(name                    = "SkimmingToolEXOT26",
                                                             FilterType              = "2L2Q", 
                                                             NumberOfLeptons         = 2,
                                                             NumberOfElectrons       = 0,
                                                             NumberOfMuons           = 0,
                                                             JetContainerKey         = "AntiKt4EMTopoJets",
                                                             NumberOfJets            = 1,
                                                             JetPtCut                = 15.*Units.GeV,
                                                             JetEtaCut               = 2.6, 
                                                             MergedJetContainerKey0  = "AntiKt4EMTopoJets",
                                                             NumberOfMergedJets0     = 1,
                                                             MergedJetPtCut0         = 100.*Units.GeV,
                                                             MergedJetEtaCut0        = 2.6, 
                                                             MergedJetContainerKey1  = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                             NumberOfMergedJets1     = 1,
                                                             MergedJetPtCut1         = 100.*Units.GeV,
                                                             MergedJetEtaCut1        = 2.6, 
                                                             NumberOfPhotons         = 0,
                                                             ElectronQuality         = "DFCommonElectronsLHVeryLoose",
                                                             ElectronEtCut           = 6.*Units.GeV,
                                                             MuonQuality             = "DFCommonMuonsPreselection",
                                                             MuonPtCut               = 6.*Units.GeV,
                                                             RequireTightLeptons     = False,
                                                             InvariantMassCut        = 5.*Units.GeV,
                                                             DRElectronJetCut        = -1.,
                                                             Trigger2L2Q             = triggerRequirement)
ToolSvc += SkimmingToolEXOT26
print SkimmingToolEXOT26




#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
exot26Seq = CfgMgr.AthSequencer("EXOT26Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

exot26Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT26Kernel_preLep",
    SkimmingTools = [SkimmingToolEXOT26_preLep]
    )

#====================================================================
# Standard jets
#====================================================================

if not "EXOT26Jets" in OutputJets:
    OutputJets["EXOT26Jets"] = []

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, exot26Seq, "EXOT26Jets")

    addDefaultTrimmedJets(exot26Seq,"EXOT26Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(exot26Seq,'EXOT26Jets')

    #=======================================
    # TCC JETS
    #=======================================
#    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
    # Set up geometry and BField
#    import AthenaCommon.AtlasUnixStandardJob

#    include("RecExCond/AllDet_detDescr.py")
#    runTCCReconstruction(exot26Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")
#    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
#    addTCCTrimmedJets(exot26Seq, "EXOT26Jets")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(exot26Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(exot26Seq, ToolSvc)


#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=exot26Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
exot26Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

# Main selection
exot26Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "EXOT26Kernel",
    SkimmingTools = [SkimmingToolEXOT26],
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += exot26Seq

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D4ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT26SlimmingHelper = SlimmingHelper("EXOT26SlimmingHelper")

EXOT26SlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"                :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"             :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"           :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"        :   "xAOD::BTaggingAuxContainer",
  }

EXOT26SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                          "AntiKt4TruthJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt2Track",
#                                           "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
#                                           "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

EXOT26SlimmingHelper.ExtraVariables = ExtraContent
EXOT26SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
EXOT26SlimmingHelper.AllVariables = ExtraContainers
if DerivationFrameworkIsMonteCarlo:
    EXOT26SlimmingHelper.ExtraVariables += ExtraContentTruth
    EXOT26SlimmingHelper.AllVariables += ExtraContainersTruth
EXOT26SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["EXOT26Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]

# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(EXOT26SlimmingHelper,["EXOT26Jets"],slimmed_content, ['AntiKt4PV0TrackJets'])

EXOT26SlimmingHelper.IncludeMuonTriggerContent = True
EXOT26SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT26SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT26SlimmingHelper.IncludeJetTriggerContent = True
EXOT26SlimmingHelper.IncludeBJetTriggerContent = True

EXOT26SlimmingHelper.AppendContentToStream(EXOT26Stream)

