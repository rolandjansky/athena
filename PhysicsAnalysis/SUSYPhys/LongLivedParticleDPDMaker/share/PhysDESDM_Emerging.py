jets = primRPVLLDESDM.Emerging_containerFlags.jetCollectionName
#jets = primRPVLLDESDM.Emerging_containerFlags.hltJetCollectionName

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool

###########################################################
## Emerging jet filter                                     
###########################################################

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__EmergingJetsFilterTool

# offline jet pt > 120 GeV cut
EmergingJetsFilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsFilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsFilterTool

#topSequence += kernel( "RPVLL_EmergingFilterKernel",
#                       SkimmingTools = [EmergingJetsFilterTool],
#                     )
#
#RPVLLfilterNames.extend(["RPVLL_EmergingFilterKernel"])



## REQUIRE HIGH-PT JET on top of increased thresholds
#from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__EmergingJetsHighPtFilterTool
#
## pt > 120; high pt > 250 GeV
#EmergingJetsPt120HighPt250FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
#    name = "EmergingJetsPt120HighPt250FilterTool",
#    JetContainerKey = jets,
#    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
#    JetPtCut = 120.0*Units.GeV,
#    JetHighPtCut = 250.0*Units.GeV,
#    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
#    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
#    nHighPtJetsRequired = 1 )
#
#ToolSvc += EmergingJetsPt120HighPt250FilterTool
#
#topSequence += kernel( "RPVLL_EmergingPt120HighPt250FilterKernel",
#                       SkimmingTools = [EmergingJetsPt120HighPt250FilterTool],
#                     )
#
#RPVLLfilterNames.extend(["RPVLL_EmergingPt120HighPt250FilterKernel"])



###########################################################
## DiJet filter -- for EJ background studies
###########################################################

# HLT_j110
# offline jet pt > 120 GeV cut
EmergingJetsDiJet110FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110FilterTool

#topSequence += kernel( "RPVLL_EmergingDiJet110FilterKernel",
#                       SkimmingTools = [EmergingJetsDiJet110FilterTool],
#                     )
#
#RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110FilterKernel"])



###########################################################
## COMBINED Emerging + DiJet filter
###########################################################
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR

EmergingJets_Combined_FilterTool = DerivationFramework__FilterCombinationOR(
    name = "EmergingJets_Combined_FilterTool",
    FilterList = [EmergingJetsFilterTool, EmergingJetsDiJet110FilterTool] )

ToolSvc += EmergingJets_Combined_FilterTool

topSequence += kernel( "RPVLL_EmergingFilterKernel",
                       SkimmingTools = [EmergingJets_Combined_FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingFilterKernel"])


