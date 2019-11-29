from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__HnlSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

from AthenaCommon.Logging import logging
msg = logging.getLogger("PhysDESDM_HNL")
msg.setLevel(logging.INFO)

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
from LongLivedParticleDPDMaker.RPVLLTriggers import rpvllTrig
apiSingleMuonTriggerlist = []
apiSingleElectronTriggerlist = []
apiMultiMuonTriggerlist = []
apiMultiElectronTriggerlist = []
if rpvllTrig.doRPVLLTriggerAPI:
    apiSingleMuonTriggerlist = apitriggers.getHNLSingleMuonTriggers()
    apiSingleElectronTriggerlist = apitriggers.getHNLSingleElectronTriggers()
    apiMultiMuonTriggerlist = apitriggers.getHNLMultiMuonTriggers()
    apiMultiElectronTriggerlist = apitriggers.getHNLMultiElectronTriggers()

from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import primRPVLLDESDM

# Filter for a prompt muon + a displaced muon
HnlMuMuTriggers = apiSingleMuonTriggerlist
if primRPVLLDESDM.doHnlMultiLeptonTriggers():
    HnlMuMuTriggers += apiMultiMuonTriggerlist
HnlMuMuFilterTool = skimtool( name = "HnlMuMuFilterTool",
                              IsPromptMuon = True,
                              IsDisplacedMuon = True,
                              Triggers  = HnlMuMuTriggers,
                              Mu1PtMin  = primRPVLLDESDM.promptMuonPtThreshHnl(),
                              Mu1Types  = primRPVLLDESDM.promptMuonTypesHnl(),
                              Mu1IsoCut = primRPVLLDESDM.promptMuonIsoThreshHnl(),
                              Mu2PtMin  = primRPVLLDESDM.displacedMuonPtThreshHnl(),
                              Mu2Types  = primRPVLLDESDM.displacedMuonTypesHnl(),
                              Mu2IsoCut = primRPVLLDESDM.displacedMuonIsoThreshHnl(),
                              Mu2d0Min  = primRPVLLDESDM.displacedMuonD0ThreshHnl(),
                              dPhiMin   = primRPVLLDESDM.leptonDphiThreshHnl())
ToolSvc += HnlMuMuFilterTool
msg.info("HnlMuMuFilterTool.Triggers:")
msg.info(HnlMuMuFilterTool.Triggers)
topSequence += kernel( "RPVLL_HnlMuMuFilterKernel",
                       SkimmingTools = [HnlMuMuFilterTool] )
RPVLLfilterNames.extend(["RPVLL_HnlMuMuFilterKernel"])

# Filter for a prompt electron + a displaced muon
if primRPVLLDESDM.doHnlElMu():
    HnlElMuTriggers = apiSingleElectronTriggerlist
    HnlElMuFilterTool = skimtool( name = "HnlElMuFilterTool",
                                  IsPromptMuon = False,
                                  IsDisplacedMuon = True,
                                  Triggers  = HnlElMuTriggers,
                                  El1PtMin  = primRPVLLDESDM.promptElectronPtThreshHnl(),
                                  El1ID     = primRPVLLDESDM.promptElectronIDHnl(),
                                  El1IsoType= primRPVLLDESDM.promptElectronIsoTypeHnl(),
                                  El1IsoCut = primRPVLLDESDM.promptElectronIsoThreshHnl(),
                                  Mu2PtMin  = primRPVLLDESDM.displacedMuonPtThreshHnl(),
                                  Mu2Types  = primRPVLLDESDM.displacedMuonTypesHnl(),
                                  Mu2IsoCut = primRPVLLDESDM.displacedMuonIsoThreshHnl(),
                                  Mu2d0Min  = primRPVLLDESDM.displacedMuonD0ThreshHnl(),
                                  dPhiMin   = primRPVLLDESDM.leptonDphiThreshHnl())
    ToolSvc += HnlElMuFilterTool
    msg.info("HnlElMuFilterTool.Triggers:")
    msg.info(HnlElMuFilterTool.Triggers)
    topSequence += kernel( "RPVLL_HnlElMuFilterKernel",
                           SkimmingTools = [HnlElMuFilterTool] )
    RPVLLfilterNames.extend(["RPVLL_HnlElMuFilterKernel"])

# Filter for a prompt electron + a displaced electron
if primRPVLLDESDM.doHnlElEl():
    HnlElElTriggers = apiSingleElectronTriggerlist
    if primRPVLLDESDM.doHnlMultiLeptonTriggers():
        HnlElElTriggers += apiMultiElectronTriggerlist
    HnlElElFilterTool = skimtool( name = "HnlElElFilterTool",
                                  IsPromptMuon = False,
                                  IsDisplacedMuon = False,
                                  Triggers  = HnlElElTriggers,
                                  El1PtMin  = primRPVLLDESDM.promptElectronPtThreshHnl(),
                                  El1ID     = primRPVLLDESDM.promptElectronIDHnl(),
                                  El1IsoType= primRPVLLDESDM.promptElectronIsoTypeHnl(),
                                  El1IsoCut = primRPVLLDESDM.promptElectronIsoThreshHnl(),
                                  El2PtMin  = primRPVLLDESDM.displacedElectronPtThreshHnl(),
                                  El2IsoType= primRPVLLDESDM.displacedElectronIsoTypeHnl(),
                                  El2IsoCut = primRPVLLDESDM.displacedElectronIsoThreshHnl(),
                                  El2d0Min  = primRPVLLDESDM.displacedElectronD0ThreshHnl(),
                                  dPhiMin   = primRPVLLDESDM.leptonDphiThreshHnl())
    ToolSvc += HnlElElFilterTool
    msg.info("HnlElElFilterTool.Triggers:")
    msg.info(HnlElElFilterTool.Triggers)
    topSequence += kernel( "RPVLL_HnlElElFilterKernel",
                           SkimmingTools = [HnlElElFilterTool] )
    RPVLLfilterNames.extend(["RPVLL_HnlElElFilterKernel"])

# Filter for a prompt muon + a displaced electron
if primRPVLLDESDM.doHnlMuEl():
    HnlMuElTriggers = apiSingleMuonTriggerlist
    HnlMuElFilterTool = skimtool( name = "HnlMuElFilterTool",
                                  IsPromptMuon = True,
                                  IsDisplacedMuon = False,
                                  Triggers  = HnlMuElTriggers,
                                  Mu1PtMin  = primRPVLLDESDM.promptMuonPtThreshHnl(),
                                  Mu1Types  = primRPVLLDESDM.promptMuonTypesHnl(),
                                  Mu1IsoCut = primRPVLLDESDM.promptMuonIsoThreshHnl(),
                                  El2PtMin  = primRPVLLDESDM.displacedElectronPtThreshHnl(),
                                  El2IsoType= primRPVLLDESDM.displacedElectronIsoTypeHnl(),
                                  El2IsoCut = primRPVLLDESDM.displacedElectronIsoThreshHnl(),
                                  El2d0Min  = primRPVLLDESDM.displacedElectronD0ThreshHnl(),
                                  dPhiMin   = primRPVLLDESDM.leptonDphiThreshHnl())
    ToolSvc += HnlMuElFilterTool
    msg.info("HnlMuElFilterTool.Triggers:")
    msg.info(HnlMuElFilterTool.Triggers)
    topSequence += kernel( "RPVLL_HnlMuElFilterKernel",
                           SkimmingTools = [HnlMuElFilterTool] )
    RPVLLfilterNames.extend(["RPVLL_HnlMuElFilterKernel"])
