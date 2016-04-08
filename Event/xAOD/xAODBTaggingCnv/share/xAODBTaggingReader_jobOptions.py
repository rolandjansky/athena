# $Id: xAODBTaggingReader_jobOptions.py 583111 2014-02-13 17:31:49Z christos $

# Set up the reading of the input xAOD:
FNAME = "xAOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the xAOD reader algorithm(s):
from xAODBTaggingCnv.xAODBTaggingCnvConf import xAODReader__BTaggingReader

alg = xAODReader__BTaggingReader()
alg.OutputLevel = DEBUG
theJob += alg

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
