# $Id: makeTrigJetD3PD.py 307763 2010-06-29 14:25:58Z krasznaa $
#
# Example jobO for saving the full trigger jet information from an ESD/AOD file.
#

# Properties that the users should modify:
if not "InputFiles" in dir():
    InputFiles = [ "AOD.pool.root" ]
if not "level" in dir():
    level = 10
if not "OutputFile" in dir():
    OutputFile = "trig_jet_d3pd_level%d.root" % level
if not "EvtMax" in dir():
    EvtMax = 100

# Set up the needed RecEx flags:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

# Set up what the RecExCommon job should and shouldn't do:
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#########################################################################
#                                                                       #
#                     Now set up the D3PDMaker job                      #
#                                                                       #
#########################################################################

# Create a logger, mainly for fun:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "makeTrigJetD3PD" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Save the full trigger jet information:
from TrigJetD3PDMaker.TrigJetD3PD import TrigJetD3PD
d3pdalg = TrigJetD3PD( OutputFile, level = level )

# Add some basic event-level information:
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 1 )

logger.info( "Configured the D3PDMaker algorithm" )

# Do some of the additional setup:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
