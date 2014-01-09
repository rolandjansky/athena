# Code shared among different embedded generator job options.
#
# Author: Andrei Gaponenko

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = EvtMax

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.RunNumber = RunNumber
ServiceMgr.EventSelector.FirstEvent = FirstEvent

import AthenaPoolCnvSvc.AthenaPool

from PartPropSvc.PartPropSvcConf import *

#================================================================
# Add an extra EventSelector to read in real data events

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
InputDataEventSelector = EventSelectorAthenaPool("InputDataEventSelector")
InputDataEventSelector.InputCollections = InputCollections
InputDataEventSelector.SkipEvents = 0;

ServiceMgr += InputDataEventSelector

#----------------------------------------------------------------=
# And a separate event store for the custom selector
from StoreGate.StoreGateConf import StoreGateSvc
sg2 = StoreGateSvc("InputData_Event_SG")
#sg2.Dump = True
#sg2.OutputLevel = VERBOSE

#================================================================
# Output

from OutputStreamAthenaPool.OutputStreamAthenaPool import createOutputStream
outStream = createOutputStream( "EmbeddedGeneratorOut" )
outStream.Store = ServiceMgr.StoreGateSvc
outStream.OutputFile  = OutputFile
outStream.ItemList += [ "McEventCollection#*" ]

#================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from TruthExamples.TruthExamplesConf import PrintMC
topSeq += PrintMC()
topSeq.PrintMC.FirstEvent = FirstEvent
topSeq.PrintMC.LastEvent = FirstEvent+10

#================================================================
# Set log level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# ServiceMgr.MessageSvc.infoLimit  = 100000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = INFO

#==============================================================
print "At the end of the jO script: algorithms =\n", topSeq
print "\nAt the end of the jO script: services =\n", ServiceMgr
#==============================================================
# EOF
