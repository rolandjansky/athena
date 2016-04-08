## @file PyAthenaTests/share/pythin_multistream_jobOptions.py
## @purpose read a POOL file and thin out some elements

###############################################
if not 'EVTMAX' in dir():
    EVTMAX=5
if not 'INPUT' in dir():
    INPUT = [
    'multistream_athexparticles.pool',
    ]
if not 'OUTPUT_0' in dir():
    OUTPUT='thinned_0.multistream_athexparticles.pool'
if not 'OUTPUT_1' in dir():
    OUTPUT='thinned_1.multistream_athexparticles.pool'
if not 'OUTPUT_NOTHINNING' in dir():
    OUTPUT='not_thinned.multistream_athexparticles.pool'
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

## thin out odds
job += BasicThinner(name='thinner_0', OutputLevel = INFO)
job.thinner_0.coll_type = 'AthExParticles'
job.thinner_0.coll_name = 'Particles_0'
job.thinner_0.filter_fct= lambda idx,p: idx%2!=0
job.thinner_0.thinSvc = 'ThinningSvc/ThinningSvc_0'

## thin out evens
job += BasicThinner(name='thinner_1', OutputLevel = INFO)
job.thinner_1.coll_type = 'AthExParticles'
job.thinner_1.coll_name = 'Particles_1'
job.thinner_1.filter_fct= lambda idx,p: idx%2==0
job.thinner_1.thinSvc = 'ThinningSvc/ThinningSvc_1'


theApp.EvtMax = EVTMAX

# POOL Persistency
import AthenaPoolCnvSvc.WriteAthenaPool

###############################################################################
###------------ output streams ------------------

## thinning of odds elements (1,3,5,...)
#########################################
job += CfgMgr.AthenaOutputStream("OutStream_0",
                                  WritingTool="AthenaPoolOutputStreamTool")
outStream = job.OutStream_0
outStream.ItemList = [
     "EventInfo#*",
     "AthExParticles#*",
     ]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStream.OutputFile = OUTPUT_0

###
## no-thinning
#########################################
job += CfgMgr.AthenaOutputStream("OutStream_nothinning",
                                  WritingTool="AthenaPoolOutputStreamTool")
outStream = job.OutStream_nothinning
outStream.ItemList = [
     "EventInfo#*",
     "AthExParticles#*",
     ]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStream.OutputFile = OUTPUT_NOTHINNING

## thinning of evens elements (0,2,4,...)
#########################################
job += CfgMgr.AthenaOutputStream("OutStream_1",
                                  WritingTool="AthenaPoolOutputStreamTool")
outStream = job.OutStream_1
outStream.ItemList = [
     "EventInfo#*",
     "AthExParticles#*",
     ]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStream.OutputFile = OUTPUT_1

## configure the ThinningSvc
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
svcMgr += createThinningSvc(svcName="ThinningSvc_0",
                            outStreams=[job.OutStream_0])
svcMgr += createThinningSvc(svcName="ThinningSvc_1",
                            outStreams=[job.OutStream_1])
