## @file PyAthenaTests/share/pythin_multistream_checker_jobOptions.py
## @purpose read a POOL file and inspect possibly thinned containers

###############################################
if not 'EVTMAX' in dir():
    EVTMAX=5
if not 'INPUT' in dir():
    INPUT = [
    'thinned_0.multistream_athexparticles.pool',
    ]
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
from PyAthenaTests.Lib import ThinChecker

## checking containers content
job += ThinChecker(name='thin_checker_0', OutputLevel = INFO)
job.thin_checker_0.coll_type = 'AthExParticles'
job.thin_checker_0.coll_name = 'Particles_0'

job += ThinChecker(name='thin_checker_1', OutputLevel = INFO)
job.thin_checker_1.coll_type = 'AthExParticles'
job.thin_checker_1.coll_name = 'Particles_1'

