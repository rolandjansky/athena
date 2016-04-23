# $Id$

# copied from CPToolTests
# filename taken from DevelopingCPToolsForxAOD twiki page (as of 9/05/14)
FNAME= "/afs/cern.ch/work/k/krasznaa/public/xAOD/19.0.X_rel_4/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root "


import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
from xAODBTaggingEfficiency.xAODBTaggingEfficiencyConf import BTagToolTester
alg = BTagToolTester()
alg.BTaggingEfficiencyTool.TaggerName = 'MV1'
alg.BTaggingEfficiencyTool.OperatingPoint = '0_8119'
alg.BTaggingEfficiencyTool.JetAuthor = 'AntiKt4TopoEMJVF'
alg.BTaggingEfficiencyTool.ScaleFactorFileName = 'testCDI.root'
alg.BTaggingEfficiencyTool.EfficiencyFileName='testCDI.root'
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
