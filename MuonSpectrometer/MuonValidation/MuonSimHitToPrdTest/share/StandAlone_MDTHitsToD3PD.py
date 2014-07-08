
#
# Define the input and output files, the number of events to process, and
# the general detail level:
#

InputFiles = [ "/afs/cern.ch/user/k/kbachas/public/HITS.pool.root" ]
OutputFile = "test_simHits.root"
EvtMax = 50

# Set up the POOL file reading:
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.InputCollections = InputFiles

# Create a logger, mainly for fun:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "test_simHits" )

# Create the D3PD stream:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( "simHits", OutputFile )

# Add basic event information to the stream:
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 10 )

# Add the new SimHit object(s):
from MuonD3PDMaker.MDTSimHitD3PDObject import MDTSimHitD3PDObject
d3pdalg += MDTSimHitD3PDObject( 10 )

#Not there yet
#from MuonD3PDMaker.RPCSimHitD3PDObject import RPCSimHitD3PDObject
#d3pdalg += RPCSimHitD3PDObject( 10 )

# Let the user know what we did:
logger.info( "Configured the D3PDMaker algorithm" )

# Do some of the additional setup:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = EvtMax
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
