from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__HnlSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

from AthenaCommon.Logging import logging
msg = logging.getLogger("PhysDESDM_HNL")
msg.setLevel(logging.INFO)

## TriggerAPI (not working correctly) ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
from LongLivedParticleDPDMaker.RPVLLTriggers import rpvllTrig
# Hardcoded lists are necessary.
# https://twiki.cern.ch/twiki/bin/view/Atlas/LowestUnprescaled
apiSingleMuonTriggerlist = [
    # 2015
    "HLT_mu20_iloose_L1MU15",
    # 2016 A
    "HLT_mu24_iloose",
    # 2016 A
    "HLT_mu24_ivarloose",
    # 2016 B-E
    "HLT_mu24_ivarmedium",
    # 2016 B-E
    "HLT_mu24_imedium",
    # 2016 D4-G2
    "HLT_mu26_imedium",
    # 2016 I4-, 2017, 2018
    "HLT_mu26_ivarmedium",
    # 2015, 2016 A
    "HLT_mu40",
    # 2016, 2017, 2018
    "HLT_mu50",
    # 2015, 2017, 2018
    "HLT_mu60_0eta105_msonly",
]
apiSingleElectronTriggerlist = [
    # 2015
    "HLT_e24_lhmedium_L1EM20VH",
    # 2016 A-D3
    "HLT_e24_lhtight_nod0_ivarloose",
    # 2018
    "HLT_e26_lhtight_nod0",
    # 2016 D4-, 2017, 2018
    "HLT_e26_lhtight_nod0_ivarloose",
    # 2016, 2017, 2018
    "HLT_e60_lhmedium_nod0",
    # 2015
    "HLT_e60_lhmedium",
    # 2016
    "HLT_e60_medium",
    # 2015
    "HLT_e120_lhloose",
    # 2016, 2017, 2018
    "HLT_e140_lhloose_nod0",
    # 2016, 2017, 2018
    "HLT_e300_etcut",
]
apiMultiMuonTriggerlist = [
    # 2016 A
    "HLT_2mu10",
    # 2016 A
    "HLT_2mu10_nomucomb",
    # 2016 B-I3, 2017, 2018
    "HLT_2mu14",
    # 2016 B-
    "HLT_2mu14_nomucomb",
    # 2016 A-D3
    "HLT_mu20_nomucomb_mu6noL1_nscan03",
    # 2016 A-E
    "HLT_mu20_mu8noL1",
    # 2016 D4-, 2017, 2018
    "HLT_mu22_mu8noL1",
    # 2017
    "HLT_mu22_mu8noL1_calotag_0eta010",
    # 2016 A, I4-
    "HLT_3mu4",
    # 2016 A-D3
    "HLT_mu6_2mu4",
    # 2016 B-D3, 2017, 2018
    "HLT_3mu6",
    # 2016 D4-, 2017
    "HLT_3mu6_msonly",
    # 2016, 2017, 2018
    "HLT_mu20_2mu4noL1",
    # 2017
    "HLT_4mu4",
    # 2016 A-D3
    "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6",
    # 2016 A-D3
    "HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6_bTau",
    # 2016 A-D3
    "HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb",
    # 2016 A-D3
    "HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau",
    # 2016 A-D3
    "HLT_mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4",
    # 2016 A-D3
    "HLT_2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4",
]
apiMultiElectronTriggerlist = [
    # 2015
    "HLT_2e12_lhloose_L12EM10VH",
    # 2016 A-D3
    "HLT_2e15_lhvloose_nod0_L12EM13VH",
    # 2015
    "HLT_e17_lhloose_2e9_lhloose",
    # 2016
    "HLT_e17_lhloose_nod0_2e9_lhloose_nod0",
    # 2017 G-
    "HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH",
    # 2017 D4-
    "HLT_2e17_lhvloose_nod0",
    # 2017, 2018
    "HLT_2e17_lhvloose_nod0_L12EM15VHI",
    # 2017, 2018
    "HLT_2e24_lhvloose_nod0",
    # 2017, 2018
    "HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH",
]
apiElectronMuonTriggerlist = [
    # 2015 C2-D6
    "HLT_e17_lhloose_mu14",
    # 2016, 2017, 2018
    "HLT_e17_lhloose_nod0_mu14",
    # 2016
    "HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1",
    # 2017, 2018
    "HLT_e26_lhmedium_nod0_mu8noL1",
    # 2016, 2017, 2018
    "HLT_e7_lhmedium_nod0_mu24",
    # 2016, 2017, 2018
    "HLT_e12_lhloose_nod0_2mu10",
    # 2015 C2-D6
    "HLT_2e12_lhloose_mu10",
    # 2016, 2017, 2018
    "HLT_2e12_lhloose_nod0_mu10",
    # 2015 C2-D6
    "HLT_e7_lhmedium_mu24",
    # 2015 C2-D6
    "HLT_e12_lhloose_2mu10",
]
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
    if primRPVLLDESDM.doHnlMultiLeptonTriggers():
        HnlElMuTriggers += apiElectronMuonTriggerlist
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
    if primRPVLLDESDM.doHnlMultiLeptonTriggers():
        HnlMuElTriggers += apiElectronMuonTriggerlist
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
