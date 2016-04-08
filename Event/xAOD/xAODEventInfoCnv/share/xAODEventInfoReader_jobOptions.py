# $Id: xAODEventInfoReader_jobOptions.py 682548 2015-07-13 13:57:15Z krasznaa $

# Set up the reading of a file:
FNAME = "xAOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the EventInfo reader algorithm:
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODReader__EventInfoReaderAlg
alg = xAODReader__EventInfoReaderAlg( SGKey = "EventInfoTest" )
alg.OutputLevel = 2
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
