import AthenaPoolCnvSvc.ReadAthenaPool
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

looper = theApp.EventLoop
if not hasattr(svcMgr, looper):
  svcMgr += getattr(CfgMgr, looper)()
getattr(svcMgr, looper).EventPrintoutInterval = 1000

if not hasattr(svcMgr, "THistSvc"):
    svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["TREE DATAFILE='mc_tree.root' OPT='RECREATE'"]

ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")

anaSeq = CfgMgr.AthSequencer("AnalysisSequence")
athAlgSeq += anaSeq

anaSeq += CfgMgr.HLT__MET__NNHLTFex(
    MonTool=getMETMonTool()
)
