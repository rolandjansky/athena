## @file PyAthenaTests/share/pyevtinfo_jobOptions.py
## @purpose read a POOL file and dump/print out run and event numbers

###########################
if not 'EVTMAX' in dir():
    EVTMAX=-1
if not 'INPUT' in dir():
    INPUT = [
    'filtered.pool',
    ]
if not 'ASCIILOG' in dir():
    ASCIILOG = 'evtlist.ascii'
###########################
    
## import the SI units
import AthenaCommon.SystemOfUnits as Units

## setup the Athena application to read-in POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
## now the appropriate EventSelector has been added and
## configured to the list of services
##  --> tell this service which file to read-in
svcMgr.EventSelector.InputCollections = INPUT

## get a handle on the sequence of algorithms to be run
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## import my algorithm and add it to the list of algorithms to be run
from PyAthenaTests.Lib import EventInfoDumper
job += EventInfoDumper(OutputLevel = INFO)
job.EventInfoDumper.evtinfo = 'ByteStreamEventInfo'
job.EventInfoDumper.filename = ASCIILOG

## get a handle on the application manager
from AthenaCommon.AppMgr import theApp

## read over all events from the input file(s)
theApp.EvtMax = EVTMAX

## EOF ##
