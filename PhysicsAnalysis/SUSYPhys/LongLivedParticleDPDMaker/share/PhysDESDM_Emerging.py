#jets = primRPVLLDESDM.Emerging_containerFlags.jetCollectionName
jets = primRPVLLDESDM.Emerging_containerFlags.hltJetCollectionName

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool

###########################################################
## Emerging jet filter                                     
###########################################################

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__EmergingJetsFilterTool

EmergingJetsFilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsFilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtMin,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsFilterTool

topSequence += kernel( "RPVLL_EmergingFilterKernel",
                       SkimmingTools = [EmergingJetsFilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingFilterKernel"])


###########################################################
## INCREASED JET PT THRESHOLDS -- need to reduce rate
###########################################################
# pt > 120 GeV
EmergingJetsPt120FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsPt120FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsPt120FilterTool

topSequence += kernel( "RPVLL_EmergingPt120FilterKernel",
                       SkimmingTools = [EmergingJetsPt120FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt120FilterKernel"])

# pt > 130 GeV
EmergingJetsPt130FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsPt130FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsPt130FilterTool

topSequence += kernel( "RPVLL_EmergingPt130FilterKernel",
                       SkimmingTools = [EmergingJetsPt130FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt130FilterKernel"])

# pt > 140 GeV
EmergingJetsPt140FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsPt140FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsPt140FilterTool

topSequence += kernel( "RPVLL_EmergingPt140FilterKernel",
                       SkimmingTools = [EmergingJetsPt140FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt140FilterKernel"])

# pt > 150 GeV
EmergingJetsPt150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsPt150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc += EmergingJetsPt150FilterTool

topSequence += kernel( "RPVLL_EmergingPt150FilterKernel",
                       SkimmingTools = [EmergingJetsPt150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt150FilterKernel"])


# REQUIRE HIGH-PT JET on top of increased thresholds
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__EmergingJetsHighPtFilterTool

# pt > 120; high pt > 250 GeV
EmergingJetsPt120HighPt250FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt120HighPt250FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetHighPtCut = 250.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt120HighPt250FilterTool

topSequence += kernel( "RPVLL_EmergingPt120HighPt250FilterKernel",
                       SkimmingTools = [EmergingJetsPt120HighPt250FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt120HighPt250FilterKernel"])

# pt > 130; high pt > 250 GeV
EmergingJetsPt130HighPt250FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt130HighPt250FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetHighPtCut = 250.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt130HighPt250FilterTool

topSequence += kernel( "RPVLL_EmergingPt130HighPt250FilterKernel",
                       SkimmingTools = [EmergingJetsPt130HighPt250FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt130HighPt250FilterKernel"])

# pt > 140; high pt > 250 GeV
EmergingJetsPt140HighPt250FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt140HighPt250FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetHighPtCut = 250.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt140HighPt250FilterTool

topSequence += kernel( "RPVLL_EmergingPt140HighPt250FilterKernel",
                       SkimmingTools = [EmergingJetsPt140HighPt250FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt140HighPt250FilterKernel"])

# pt > 150; high pt > 250 GeV
EmergingJetsPt150HighPt250FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt150HighPt250FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetHighPtCut = 250.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt150HighPt250FilterTool

topSequence += kernel( "RPVLL_EmergingPt150HighPt250FilterKernel",
                       SkimmingTools = [EmergingJetsPt150HighPt250FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt150HighPt250FilterKernel"])


# pt > 120; high pt > 300 GeV
EmergingJetsPt120HighPt300FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt120HighPt300FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetHighPtCut = 300.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt120HighPt300FilterTool

topSequence += kernel( "RPVLL_EmergingPt120HighPt300FilterKernel",
                       SkimmingTools = [EmergingJetsPt120HighPt300FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt120HighPt300FilterKernel"])

# pt > 130; high pt > 300 GeV
EmergingJetsPt130HighPt300FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt130HighPt300FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetHighPtCut = 300.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt130HighPt300FilterTool

topSequence += kernel( "RPVLL_EmergingPt130HighPt300FilterKernel",
                       SkimmingTools = [EmergingJetsPt130HighPt300FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt130HighPt300FilterKernel"])

# pt > 140; high pt > 300 GeV
EmergingJetsPt140HighPt300FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt140HighPt300FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetHighPtCut = 300.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt140HighPt300FilterTool

topSequence += kernel( "RPVLL_EmergingPt140HighPt300FilterKernel",
                       SkimmingTools = [EmergingJetsPt140HighPt300FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt140HighPt300FilterKernel"])

# pt > 150; high pt > 300 GeV
EmergingJetsPt150HighPt300FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt150HighPt300FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetHighPtCut = 300.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt150HighPt300FilterTool

topSequence += kernel( "RPVLL_EmergingPt150HighPt300FilterKernel",
                       SkimmingTools = [EmergingJetsPt150HighPt300FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt150HighPt300FilterKernel"])


# pt > 120; high pt > 350 GeV
EmergingJetsPt120HighPt350FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt120HighPt350FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetHighPtCut = 350.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt120HighPt350FilterTool

topSequence += kernel( "RPVLL_EmergingPt120HighPt350FilterKernel",
                       SkimmingTools = [EmergingJetsPt120HighPt350FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt120HighPt350FilterKernel"])

# pt > 130; high pt > 350 GeV
EmergingJetsPt130HighPt350FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt130HighPt350FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetHighPtCut = 350.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt130HighPt350FilterTool

topSequence += kernel( "RPVLL_EmergingPt130HighPt350FilterKernel",
                       SkimmingTools = [EmergingJetsPt130HighPt350FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt130HighPt350FilterKernel"])

# pt > 140; high pt > 350 GeV
EmergingJetsPt140HighPt350FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt140HighPt350FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetHighPtCut = 350.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt140HighPt350FilterTool

topSequence += kernel( "RPVLL_EmergingPt140HighPt350FilterKernel",
                       SkimmingTools = [EmergingJetsPt140HighPt350FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt140HighPt350FilterKernel"])

# pt > 150; high pt > 350 GeV
EmergingJetsPt150HighPt350FilterTool = DerivationFramework__EmergingJetsHighPtFilterTool(
    name = "EmergingJetsPt150HighPt350FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetHighPtCut = 350.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed,
    nHighPtJetsRequired = 1 )

ToolSvc += EmergingJetsPt150HighPt350FilterTool

topSequence += kernel( "RPVLL_EmergingPt150HighPt350FilterKernel",
                       SkimmingTools = [EmergingJetsPt150HighPt350FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingPt150HighPt350FilterKernel"])



###########################################################
## DiJet filter -- for EJ background studies
###########################################################

# HLT_j110
# offline jet pt > 100 GeV cut
EmergingJetsDiJet110FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtMin,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet110FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet110FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110FilterKernel"])

# offline jet pt > 120 GeV cut
EmergingJetsDiJet110Pt120FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110Pt120FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110Pt120FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet110Pt120FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet110Pt120FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110Pt120FilterKernel"])

# offline jet pt > 130 GeV cut
EmergingJetsDiJet110Pt130FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110Pt130FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110Pt130FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet110Pt130FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet110Pt130FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110Pt130FilterKernel"])

# offline jet pt > 140 GeV cut
EmergingJetsDiJet110Pt140FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110Pt140FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110Pt140FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet110Pt140FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet110Pt140FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110Pt140FilterKernel"])

# offline jet pt > 150 GeV cut
EmergingJetsDiJet110Pt150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet110Pt150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet110FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet110FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet110FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet110Pt150FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet110Pt150FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet110Pt150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet110Pt150FilterKernel"])


# HLT_j150
# offline jet pt > 100 GeV cut
EmergingJetsDiJet150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet150FilterFlags.Triggers,
    JetPtCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtMin,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet150FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet150FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet150FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet150FilterKernel"])

# offline jet pt > 120 GeV cut
EmergingJetsDiJet150Pt120FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet150Pt120FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet150FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet150FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet150Pt120FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet150Pt120FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet150Pt120FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet150Pt120FilterKernel"])

# offline jet pt > 130 GeV cut
EmergingJetsDiJet150Pt130FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet150Pt130FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet150FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet150FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet150Pt130FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet150Pt130FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet150Pt130FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet150Pt130FilterKernel"])

# offline jet pt > 140 GeV cut
EmergingJetsDiJet150Pt140FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet150Pt140FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet150FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet150FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet150Pt140FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet150Pt140FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet150Pt140FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet150Pt140FilterKernel"])

# offline jet pt > 150 GeV cut
EmergingJetsDiJet150Pt150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet150Pt150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet150FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet150FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet150FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet150Pt150FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet150Pt150FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet150Pt150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet150Pt150FilterKernel"])


# HLT_j175
# offline jet pt > 100 GeV cut
EmergingJetsDiJet175FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet175FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet175FilterFlags.Triggers,
    JetPtCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtMin,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet175FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet175FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet175FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet175FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet175FilterKernel"])

# offline jet pt > 120 GeV cut
EmergingJetsDiJet175Pt120FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet175Pt120FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet175FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet175FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet175Pt120FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet175Pt120FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet175Pt120FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet175Pt120FilterKernel"])

# offline jet pt > 130 GeV cut
EmergingJetsDiJet175Pt130FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet175Pt130FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet175FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet175FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet175Pt130FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet175Pt130FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet175Pt130FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet175Pt130FilterKernel"])

# offline jet pt > 140 GeV cut
EmergingJetsDiJet175Pt140FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet175Pt140FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet175FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet175FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet175Pt140FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet175Pt140FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet175Pt140FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet175Pt140FilterKernel"])

# offline jet pt > 150 GeV cut
EmergingJetsDiJet175Pt150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet175Pt150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet175FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet175FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet175FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet175Pt150FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet175Pt150FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet175Pt150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet175Pt150FilterKernel"])


# HLT_j260
# offline jet pt > 100 GeV
EmergingJetsDiJet260FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet260FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet260FilterFlags.Triggers,
    JetPtCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtMin,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet260FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet260FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet260FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet260FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet260FilterKernel"])

# offline jet pt > 120 GeV cut
EmergingJetsDiJet260Pt120FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet260Pt120FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet260FilterFlags.Triggers,
    JetPtCut = 120.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet260FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet260Pt120FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet260Pt120FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet260Pt120FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet260Pt120FilterKernel"])

# offline jet pt > 130 GeV cut
EmergingJetsDiJet260Pt130FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet260Pt130FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet260FilterFlags.Triggers,
    JetPtCut = 130.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet260FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet260Pt130FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet260Pt130FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet260Pt130FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet260Pt130FilterKernel"])

# offline jet pt > 140 GeV cut
EmergingJetsDiJet260Pt140FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet260Pt140FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet260FilterFlags.Triggers,
    JetPtCut = 140.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet260FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet260Pt140FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet260Pt140FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet260Pt140FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet260Pt140FilterKernel"])

# offline jet pt > 150 GeV cut
EmergingJetsDiJet260Pt150FilterTool = DerivationFramework__EmergingJetsFilterTool(
    name = "EmergingJetsDiJet260Pt150FilterTool",
    JetContainerKey = jets,
    Triggers = primRPVLLDESDM.Emerging_DiJet260FilterFlags.Triggers,
    JetPtCut = 150.0*Units.GeV,
    JetEtaCut = primRPVLLDESDM.Emerging_DiJet260FilterFlags.cutEtaMax,
    nJetsRequired = primRPVLLDESDM.Emerging_DiJet260FilterFlags.nPassed )

ToolSvc += EmergingJetsDiJet260Pt150FilterTool

topSequence += kernel( "RPVLL_EmergingDiJet260Pt150FilterKernel",
                       SkimmingTools = [EmergingJetsDiJet260Pt150FilterTool],
                     )

RPVLLfilterNames.extend(["RPVLL_EmergingDiJet260Pt150FilterKernel"])




