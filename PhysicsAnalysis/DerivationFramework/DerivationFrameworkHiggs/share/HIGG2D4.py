#====================================================================
# HIGG2D4.py for 2L2Q filter
# This requires the reductionConf flag HIGG2D4 in Reco_tf.py   
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
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkHiggs.TruthCategories import *
import AthenaCommon.SystemOfUnits as Units
# SET UP STREAM 
#==================================================================== 
streamName = derivationFlags.WriteDAOD_HIGG2D4Stream.StreamName 
fileName = buildFileName(derivationFlags.WriteDAOD_HIGG2D4Stream) 
HIGG2D4Stream = MSMgr.NewPoolRootStream( streamName, fileName) 
HIGG2D4Stream.AcceptAlgs(["HIGG2D4Kernel"]) 

import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG2D4ThinningHelper = ThinningHelper("HIGG2D4ThinningHelper") 
#trigger navigation content  
HIGG2D4ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*|HLT_xe.*|HLT_j.*|HLT_b.*' 
HIGG2D4ThinningHelper.AppendToStream(HIGG2D4Stream) 


import DerivationFrameworkHiggs.HIGG5Common as HIGG5Common
#TrackParticles thinning
# thinningTools.append( HIGG5Common.getAntiKt4EMTopoTrackParticleThinning('HIGG2D4',HIGG2D4ThinningHelper) )
thinningTools.append( HIGG5Common.getMuonTrackParticleThinning(         'HIGG2D4',HIGG2D4ThinningHelper) )
thinningTools.append( HIGG5Common.getElectronTrackParticleThinning(     'HIGG2D4',HIGG2D4ThinningHelper,   BestMatchOnly=False) )
# thinningTools.append( HIGG5Common.getPhotonTrackParticleThinning(       'HIGG2D4',HIGG2D4ThinningHelper) )
# thinningTools.append( HIGG5Common.getTauTrackParticleThinning(          'HIGG2D4',HIGG2D4ThinningHelper) )
thinningTools.append( HIGG5Common.getTCCTrackParticleThinning(          'HIGG2D4',HIGG2D4ThinningHelper) )

#Track Particles + CaloCluster thinning
thinningTools.append( HIGG5Common.getAntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Thinning('HIGG2D4',HIGG2D4ThinningHelper) )

#calocluster thinning
thinningTools.append( HIGG5Common.getTauCaloClusterThinning(            'HIGG2D4',HIGG2D4ThinningHelper) )
thinningTools.append( HIGG5Common.getAntiKt10LCTopoCaloClusterThinning( 'HIGG2D4',HIGG2D4ThinningHelper) )

#generic object thinning
thinningTools.append( HIGG5Common.getAntiKt10LCTopoTrimmedPtFrac5SmallR20Thinning('HIGG2D4',HIGG2D4ThinningHelper) )
if DerivationFrameworkIsMonteCarlo :
  thinningTools.append( HIGG5Common.getTruthThinningTool('HIGG2D4',HIGG2D4ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthTrimmedPtFrac5SmallR20Thinning('HIGG2D4',HIGG2D4ThinningHelper) )
  thinningTools.append( HIGG5Common.getAntiKt10TruthWZTrimmedPtFrac5SmallR20Thinning('HIGG2D4',HIGG2D4ThinningHelper) )

# Truth particles
# if DerivationFrameworkIsMonteCarlo:
#     thinningTools.append(HIGG5Common.getTruthThinningTool('HIGG2D4', HIGG2D4ThinningHelper))
#     from DerivationFrameworkMCTruth.MCTruthCommon import *
#     addStandardTruthContents()

# print "HIGG2D4.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
beamEnergy = jobproperties.Beam.energy()
print "HIGG2D4.py jobproperties.Beam.energy()", beamEnergy

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
print "HIGG2D4.py SkipTriggerRequirement", SkipTriggerRequirement
if SkipTriggerRequirement:
    triggerRequirement=[]
print "HIGG2D4.py triggerRequirement", triggerRequirement

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
SkimmingToolHIGG2D4_preLep = DerivationFramework__xAODStringSkimmingTool(name = "SkimmingToolHIGG2D4_preLep",
                                                                         expression = leptonRequirement)
ToolSvc += SkimmingToolHIGG2D4_preLep

# @TODO take TCC jets into account for skimming
# jetSel = '|| (( count( (AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.pt > 150.0*GeV) && (abs(AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.eta) < 2.6) ) ) > 0)'
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D4EMTopo = DerivationFramework__SkimmingToolHIGG2(name              = "SkimmingToolHIGG2D4EMTopo",
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
                                                             MergedJetPtCut1         = 150.*Units.GeV,
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
ToolSvc += SkimmingToolHIGG2D4EMTopo

SkimmingToolHIGG2D4PFlow = DerivationFramework__SkimmingToolHIGG2(name                  = "SkimmingToolHIGG2D4PFlow",
                                                             FilterType                 = "2L2Q",
                                                             NumberOfLeptons            = 2,
                                                             NumberOfElectrons          = 0,
                                                             NumberOfMuons              = 0,
                                                             JetContainerKey            = "AntiKt4EMPFlowJets",
                                                             NumberOfJets               = 1,
                                                             JetPtCut                   = 15.*Units.GeV,
                                                             JetEtaCut                  = 2.6,
                                                             MergedJetContainerKey0     = "AntiKt4EMPFlowJets",
                                                             UseDFCommonJetFourMomentum = False,
                                                             NumberOfMergedJets0        = 0,
                                                             MergedJetPtCut0            = 100.*Units.GeV,
                                                             MergedJetEtaCut0           = 2.6,
                                                             NumberOfMergedJets1        = 0,
                                                             MergedJetPtCut1            = 150.*Units.GeV,
                                                             MergedJetEtaCut1           = 2.6,
                                                             NumberOfPhotons            = 0,
                                                             ElectronQuality            = "DFCommonElectronsLHVeryLoose",
                                                             ElectronEtCut              = 6.*Units.GeV,
                                                             MuonQuality                = "DFCommonMuonsPreselection",
                                                             MuonPtCut                  = 6.*Units.GeV,
                                                             RequireTightLeptons        = False,
                                                             InvariantMassCut           = 5.*Units.GeV,
                                                             DRElectronJetCut           = -1.,
                                                             Trigger2L2Q                = triggerRequirement)
ToolSvc += SkimmingToolHIGG2D4PFlow


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SkimmingToolHIGG2D4Combined = DerivationFramework__FilterCombinationOR(
                                           name       = 'SkimmingToolHIGG2D4Combined',
                                           FilterList = [SkimmingToolHIGG2D4EMTopo,SkimmingToolHIGG2D4PFlow])
ToolSvc += SkimmingToolHIGG2D4Combined

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg2d4Seq = CfgMgr.AthSequencer("HIGG2D4Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg2d4Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG2D4Kernel_preLep",
    SkimmingTools = [SkimmingToolHIGG2D4_preLep]
    )

#====================================================================
# Standard jets
#====================================================================

if not "HIGG2D4Jets" in OutputJets:
    OutputJets["HIGG2D4Jets"] = []

    # have to run tcc reconstruction prior to jet finding
    from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
    # Set up geometry and BField
    import AthenaCommon.AtlasUnixStandardJob

    include("RecExCond/AllDet_detDescr.py")
    runTCCReconstruction(higg2d4Seq, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles")

    reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets", 'AntiKt10TrackCaloClusterJets']
    if jetFlags.useTruth:
        reducedJetList += ['AntiKt4TruthJets','AntiKt4TruthWZJets']
    replaceAODReducedJets(reducedJetList, higg2d4Seq, "HIGG2D4Jets")

    addDefaultTrimmedJets(higg2d4Seq,"HIGG2D4Jets");
    if jetFlags.useTruth:
      HIGG5Common.addTrimmedTruthWZJets(higg2d4Seq,'HIGG2D4Jets')

    # also add TCC trimed jets
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(higg2d4Seq, "HIGG2D4Jets")

HIGG5Common.addAntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub(higg2d4Seq)

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
#====================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(higg2d4Seq)
# Also add Hbb Tagger
addRecommendedXbbTaggers(higg2d4Seq, ToolSvc)

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg2d4Seq)

# Jet calibration should come after fat jets
# applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg2d4Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence 
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg2d4Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

#====================================================================
# SET UP CUSTOM TRUTH3 INFORMATION
#====================================================================
if DerivationFrameworkIsMonteCarlo :
  HIGG5Common.getTruth3Collections(higg2d4Seq)

# Main selection
higg2d4Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG2D4Kernel",
    SkimmingTools = [SkimmingToolHIGG2D4Combined],
    ThinningTools = thinningTools
    )

DerivationFrameworkJob += higg2d4Seq

# QGTaggerTool ###
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=higg2d4Seq, algname="QGTaggerToolAlg")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D4ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D4SlimmingHelper = SlimmingHelper("HIGG2D4SlimmingHelper")

HIGG2D4SlimmingHelper.AppendToDictionary = HIGG5Common.getHIGG5CommonDictionExtionson()
HIGG2D4SlimmingHelper.SmartCollections   = HIGG5Common.getHIGG5CommonSmartCollections()

HIGG2D4SlimmingHelper.ExtraVariables = ExtraContent
HIGG2D4SlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
HIGG2D4SlimmingHelper.AllVariables = ExtraContainers
if DerivationFrameworkIsMonteCarlo:
    HIGG2D4SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG2D4SlimmingHelper.AllVariables += ExtraContainersTruth
HIGG2D4SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

# Add the jet containers to the stream
slimmed_content=["HIGG2D4Jets","AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
if DerivationFrameworkIsMonteCarlo :
    slimmed_content+=[
             "AntiKt4TruthJets",
             "AntiKt4TruthWZJets"
             ]

# AntiKt4PV0TrackJets are needed in conjunction with AntiKt10LCTopoJets, but the can be removed from the
# output
HIGG5Common.addJetOutputs(HIGG2D4SlimmingHelper,["HIGG2D4Jets"],slimmed_content, ['AntiKt4PV0TrackJets'])
addOriginCorrectedClusters(HIGG2D4SlimmingHelper, writeLC=True, writeEM=False)

HIGG2D4SlimmingHelper.IncludeMuonTriggerContent   = True
HIGG2D4SlimmingHelper.IncludeEGammaTriggerContent = True
HIGG2D4SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG2D4SlimmingHelper.IncludeJetTriggerContent    = True
HIGG2D4SlimmingHelper.IncludeBJetTriggerContent   = True

HIGG2D4SlimmingHelper.AppendContentToStream(HIGG2D4Stream)

