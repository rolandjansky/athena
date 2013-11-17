# $Id: BunchCrossingExample_jobOptions.py 326080 2010-10-08 13:50:09Z krasznaa $
#
# This example shows how to set up a generic job that is able to use the
# TrigBunchCrossingTool package's tools.
#
# author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 326080 $
# $Date: 2010-10-08 15:50:09 +0200 (Fri, 08 Oct 2010) $

# Set up the basic job properties:
InputFiles = [ "AOD.pool.root" ]
EvtMax = 10

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

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#######################################################################
#                                                                     #
#                   Now set up the actual job...                      #
#                                                                     #
#######################################################################

# Get ourselves a logger, mainly for fun:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "BunchCrossingExample" )
logger.info( "Setting up the bunch crossing example job" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the example algorithm, configuring the appropriate Trig::IBunchCrossingTool
# in the process:
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__BunchCrossingExample
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
theJob += Trig__BunchCrossingExample( "BunchCrossingExample" )
theJob.BunchCrossingExample.BCTool = BunchCrossingTool()
theJob.BunchCrossingExample.OutputLevel = DEBUG

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
