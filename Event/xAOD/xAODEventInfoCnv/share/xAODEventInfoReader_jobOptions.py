# $Id: xAODEventInfoReader_jobOptions.py 592116 2014-04-09 12:04:38Z krasznaa $

# Set up the reading of a file:
FNAME = "xAOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the EventInfo reader algorithm:
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODReader__EventInfoReaderAlg
alg = xAODReader__EventInfoReaderAlg()
alg.OutputLevel = 2
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
