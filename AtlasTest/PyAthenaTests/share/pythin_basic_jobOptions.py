## @file PyAthenaTests/share/pythin_basic_jobOptions.py
## @purpose read a POOL file and thin out some elements

###############################################
if not 'EVTMAX' in dir():
    EVTMAX=5
if not 'INPUT' in dir():
    INPUT = [
    'basic_athexparticles.pool',
    ]
if not 'OUTPUT' in dir():
    OUTPUT='thinned.basic_athexparticles.pool'
###############################################
    
## import the SI units
import AthenaCommon.SystemOfUnits as Units

## setup the Athena application to read-in POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
## now the appropriate EventSelector has been added and
## configured to the list of services
##  --> tell this service which file to read-in
svcMgr.EventSelector.InputCollections = INPUT

## get a handle on the application manager
from AthenaCommon.AppMgr import theApp

## read over all events from the input file(s)
theApp.EvtMax = EVTMAX

# basic configuration
import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr

# job sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## import my algorithm and add it to the list of algorithms to be run
from PyAthenaTests.Lib import BasicThinner
job += BasicThinner(name='thinner', OutputLevel = INFO)
job.thinner.coll_type = 'AthExParticles'
job.thinner.coll_name = 'athexparticles'
#job.thinner.filter_fct= lambda idx,p: idx%2==0
job.thinner.thinSvc = 'ThinningSvc/MyThinningSvc'

theApp.EvtMax = EVTMAX

# POOL Persistency
import AthenaPoolCnvSvc.WriteAthenaPool

job += CfgMgr.AthenaOutputStream("OutStream",
                                  WritingTool="AthenaPoolOutputStreamTool")
outStream = job.OutStream
outStream.ItemList = [
     "EventInfo#*",
     "AthExParticles#*",
     ]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStream.OutputFile = OUTPUT

## configure the ThinningSvc
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
svcMgr += createThinningSvc(svcName="MyThinningSvc",
                            outStreams=[job.OutStream])
