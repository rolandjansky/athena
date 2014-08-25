include.block( "MissingETPerformance/FakeMissingET_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     FakeMissingETTool as ConfiguredFakeMissingETTool
FakeMissingETTool = ConfiguredFakeMissingETTool(
    minJetEtCut = 1.,
    fakeFullCut = 0.5,

    nbinsMET   = 200,
    lowerbinMET   = 0.,
    upperbinMET   = 200.,

    nbinsMETxy   = 800,
    lowerbinMETxy   = -400.,
    upperbinMETxy   = 400.,
    )

ToolSvc += FakeMissingETTool
FakeMissingETTool.OutputLevel = INFO
print      FakeMissingETTool
