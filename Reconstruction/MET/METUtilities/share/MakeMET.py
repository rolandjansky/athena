import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from glob import glob
filelist = ["xAOD.METAssoc.test1.pool.root"]
ServiceMgr.EventSelector.InputCollections = filelist

# The tools are accessible via the configurations in metFlags
from AthenaCommon.AppMgr import ToolSvc
metMaker = CfgMgr.met__METMaker("METMaker",
                                JetPtCut=20e3,
                                DoJetJvfCut=False,
                                OutputLevel=INFO);
ToolSvc += metMaker

streamName = 'MetHistoStream'
fileName   = 'metMakerTest_metReco.root'

metAlg = CfgMgr.met__METMakerAlg("METMakerAlg",
                                 OutputLevel=INFO,
                                 RootStreamName=streamName,
                                 RootDirName='/',
                                 METCoreName='MET_New',
                                 METName='MET_Newer',
                                 Maker=metMaker)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += metAlg

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
HistoXAODStream = MSMgr.NewRootStream( streamName, fileName )

#from Valkyrie.JobOptCfg import ValgrindSvc
#svcMgr += ValgrindSvc( OutputLevel = DEBUG,
#                       ProfiledAlgs = ["METMakerAlg"],
#                       ProfiledIntervals = ["METMakerAlg.execute"])

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

#ServiceMgr.MessageSvc.defaultLimit = 9999999
theApp.EvtMax = 1000
ServiceMgr.EventSelector.SkipEvents = 0
