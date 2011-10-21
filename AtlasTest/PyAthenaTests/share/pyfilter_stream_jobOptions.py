## @file PyAthenaTests/share/pyfilter_stream_jobOptions.py
## @purpose read a POOL file and dump/print out run and event numbers and filter
##          out one event out of 2

###########################
if not 'EVTMAX' in dir():
    EVTMAX=-1
if not 'INPUT' in dir():
    INPUT = [ # a RIG "reference" file
        '/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_ESD.AOD_50Events/data10_7TeV.00167607.physics_JetTauEtmiss.recon.ESD.f298._lb0087._SFO-4._0001.1_50Events_rel.16.0.3.8_rereco'
        ]
    #INPUT = ['filtered.pool',]
if not 'OUTPUT' in dir():
    OUTPUT = 'pyathena_basic_filtered.pool'
if not 'ASCIILOG' in dir():
    ASCIILOG = 'evtlist.ascii'
###########################

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp, ToolSvc as toolSvc, ServiceMgr as svcMgr

# configure application for reading POOL files
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = INPUT
theApp.EvtMax = EVTMAX

# sequence of top algs
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from PyAthenaTests.Lib import PyFilterAlg
job += PyFilterAlg(evtinfo='ByteStreamEventInfo')
job.PyFilterAlg.filename = ASCIILOG

## configure the job to write out POOL files
import AthenaPoolCnvSvc.WriteAthenaPool
## create an output stream
job += CfgMgr.AthenaOutputStream(
    'OutStream',
    WritingTool = "AthenaPoolOutputStreamTool"
    )
# Copy everything from the input and must force reading of all input
# objects
job.OutStream.OutputFile = OUTPUT
job.OutStream.ItemList   = ['EventInfo#*']
job.OutStream.AcceptAlgs = [job.PyFilterAlg.getName()]
