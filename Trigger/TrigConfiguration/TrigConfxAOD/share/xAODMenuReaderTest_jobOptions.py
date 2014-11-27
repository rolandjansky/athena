# $Id: xAODMenuReaderTest_jobOptions.py 612011 2014-08-15 13:43:57Z krasznaa $

# Set up the file reading:
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

# Access the algorithm sequece:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the test algorithm:
theJob += CfgMgr.TrigConf__xAODMenuReader()

# Some additional options:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
