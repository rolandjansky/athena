# $Id: TrigCostD3PD_prodJobOFragment.py tamartin
#
# Top jobO fragment setting up the common Trig Cost (NTUP_TRIG_COST).

# This jobO should not be included more than once:
include.block( "TrigCost3PDMaker/TrigCostD3PDMaker_prodJobOFragment.py" )

# Set up a logger:
from AthenaCommon.Logging import logging
TrigCostD3PDStream_msg = logging.getLogger( 'TrigCostD3PDMaker_prodJobOFragment' )

# Construct the stream and file names for the Trigger D3PD:
streamName = "StreamNTUP_TRIGCOST"
fileName   = "trig_cost.root"
TrigCostD3PDStream_msg.info( "Configuring Trigger Cost D3PD with streamName '%s' and fileName '%s'" % ( streamName, fileName ) )

# Create the D3PD stream(s):
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( streamName, fileName, "trig_cost" )
  
# As we may be running from bytestream, need to disable data headers.
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
theJob.StreamNTUP_TRIGCOSTAANTStream.ExistDataHeader = False
theJob.StreamNTUP_TRIGCOSTAANTStream.WriteInputDataHeader = False

##
## Add the trigger cost data:
## By default, these are allowed to be missing without terminating the job.
##
from TrigCostD3PDMaker.TrigCostD3PDObject import TrigCostD3PDObject

# Old naming format: HLT_OPI_HLT_monitoring_event

d3pdEntries = 0
costConfigL2 = False
costConfigEF = False
costConfigHLT = False

# Configure with magic variables
if ('costD3PD_doL2' in dir() or 'costD3PD_doEF' in dir() or 'costD3PD_doHLT' in dir()):
    if ('costD3PD_doL2' in dir() and bool(costD3PD_doL2) == True):
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_L2_monitoring_event', prefix='TrigCostL2_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by costD3PD_doL2 flag to monitor Level L2" )
        d3pdEntries += 1
        costConfigL2 = True
    if ('costD3PD_doEF' in dir() and bool(costD3PD_doEF) == True):
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_EF_monitoring_event', prefix='TrigCostEF_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by costD3PD_doEF flag to monitor Level EF" )
        d3pdEntries += 1
        costConfigEF = True
    if ('costD3PD_doHLT' in dir() and bool(costD3PD_doHLT) == True):
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_HLT_monitoring_event', prefix='TrigCostHLT_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by costD3PD_doHLT flag to monitor Level HLT" )
        d3pdEntries += 1
        costConfigHLT = True
#Otherwise configure with the TriggerFlags
else:
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if TriggerFlags.doLVL2():
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_L2_monitoring_event', prefix='TrigCostL2_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by TriggerFlags to monitor Level L2" )
        d3pdEntries += 1
        costConfigL2 = True
    if TriggerFlags.doEF():
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_EF_monitoring_event', prefix='TrigCostEF_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by TriggerFlags to monitor Level EF" )
        d3pdEntries += 1
        costConfigEF = True
    if TriggerFlags.doHLT():
        d3pdalg += TrigCostD3PDObject( 2, sgkey='HLT_TrigMonEventCollection_OPI_HLT_monitoring_event', prefix='TrigCostHLT_' )
        TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is set by TriggerFlags to monitor Level HLT" )
        d3pdEntries += 1
        costConfigHLT = True

if d3pdEntries == 0:
    TrigCostD3PDStream_msg.info( "Trigger Cost D3PD is NOT set up to run over any trigger levels, did you setup the trigger?" )

##
## Add the trigger configuration meta data:
## Second argument specifies to use the metadata from the trigger cost infrastructure.
##
from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
addTrigConfMetadata( d3pdalg, True, costConfigL2, costConfigEF, costConfigHLT )

## The following can only be used in a more offline environment (AtlasProduction).
## Disabled by default. No guarantees. 

##
## Add the trigger descision bits (L1) and shorts (HLT):
##
#from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
#d3pdalg += TrigDecisionD3PDObject( 10 )

##
## Add the bunch structure meta data:
##
#from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
#addBunchStructureMetadata( d3pdalg )

##
## Add the lumi meta data: 
##
#from EventCommonD3PDMaker.LBMetadataConfig import LBMetadataConfig
#d3pdalg.MetadataTools += [ LBMetadataConfig() ]

##
## Add lumi block length data:
##
#from EventCommonD3PDMaker.LBLengthD3PDObject import LBLengthD3PDObject
#d3pdalg += LBLengthD3PDObject( 0 )
