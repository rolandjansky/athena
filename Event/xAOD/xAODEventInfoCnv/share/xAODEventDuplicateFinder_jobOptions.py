# $Id: xAODEventDuplicateFinder_jobOptions.py 663741 2015-04-29 11:52:21Z krasznaa $

# Set up the reading of a file:
FNAME = "xAOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the event duplicate finder algorithm:
theJob += CfgMgr.xAODReader__EventDuplicateFinderAlg( "EventDuplicateFinderAlg",
                                                      OutputLevel = 2 )

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
