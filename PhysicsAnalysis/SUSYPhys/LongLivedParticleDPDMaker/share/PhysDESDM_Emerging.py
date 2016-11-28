jets = primRPVLLDESDM.Emerging_containerFlags.jetCollectionName
Bjets = primRPVLLDESDM.Emerging_containerFlags.bjetCollectionName
recoTracks = primRPVLLDESDM.Emerging_containerFlags.trackCollectionName

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool

###########################################################
## Emerging jet filter                                     
###########################################################

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__EmergingJetsFilterTool

EmergingJetsFilterTool = DerivationFramework__EmergingJetsFilterTool( name = "EmergingJetsFilterTool",
                                                                              JetContainerKey = jets,
                                                                              BJetContainerKey = Bjets,
                                                                              TrackContainerKey = recoTracks,
                                                                              Triggers = primRPVLLDESDM.Emerging_FilterFlags.Triggers,
                                                                              JetPtCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtMin,
                                                                              JetEtaCut = primRPVLLDESDM.Emerging_FilterFlags.cutEtaMax,
                                                                              sumPtTrkCut = primRPVLLDESDM.Emerging_FilterFlags.cutSumPtTrkMax,
                                                                              sumJetsPtCut = primRPVLLDESDM.Emerging_FilterFlags.cutSumJetsPtMax,
                                                                              nJetsRequired = primRPVLLDESDM.Emerging_FilterFlags.nPassed )

ToolSvc+=EmergingJetsFilterTool

topSequence += kernel( "RPVLL_EmergingFilterKernel",
                       SkimmingTools = [EmergingJetsFilterTool],
                       )

RPVLLfilterNames.extend(["RPVLL_EmergingFilterKernel"])
