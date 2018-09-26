import AthenaCommon.SystemOfUnits as Units

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__HnlSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()
apitriggerlist = []
if apitriggers.doTriggerAPI:
    apitriggerlist = apitriggers.getHNLTriggers()

HnlFilterTool = skimtool( name = "HnlFilterTool",
                          Triggers  = ["HLT_mu26_ivarmedium"] + apitriggerlist,
                          Mu1PtMin  = 28*Units.GeV,
                          Mu1Types  = [0],
                          Mu1IsoCut = 0.05,
                          Mu2PtMin  = 5*Units.GeV,
                          Mu2Types  = [0,1,2],
                          Mu2IsoCut = 1,
                          Mu2d0Min  = 0.1 )

ToolSvc += HnlFilterTool
topSequence += kernel( "RPVLL_HnlFilterKernel",
                       SkimmingTools = [HnlFilterTool] )

RPVLLfilterNames.extend(["RPVLL_HnlFilterKernel"])
