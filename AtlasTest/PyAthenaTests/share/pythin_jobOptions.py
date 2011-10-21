## @file PyAthenaTests/share/pythin_jobOptions.py
## @purpose read an AOD POOL file and thin out some elements

if not 'EVTMAX' in dir():
    EVTMAX=-1
if not 'INPUT' in dir():
    INPUT = [
    '/afs/cern.ch/atlas/offline/test/atlfast.aod.herwig_rel13.pool',
    ]
if not 'OUTPUT' in dir():
    OUTPUT='thinned.data.pool'

import os
if os.path.exists(OUTPUT):
    os.unlink(OUTPUT)
    pass

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
from PyAthenaTests.Lib import Thinner
job += Thinner(OutputLevel = INFO)
#job.Thinner.filter_fct= lambda x: x.e() > 40.*Units.GeV

## get a handle on the application manager
from AthenaCommon.AppMgr import theApp

## read over all events from the input file(s)
theApp.EvtMax = EVTMAX

## configure the job to write out POOL files
import AthenaPoolCnvSvc.WriteAthenaPool
## create an output stream
job += CfgMgr.AthenaOutputStream(
    'OutStream',
    WritingTool = "AthenaPoolOutputStreamTool"
    )
# Copy everything from the input and must force reading of all input
# objects
job.OutStream.TakeItemsFromInput = True
job.OutStream.ForceRead          = True
job.OutStream.OutputFile         = OUTPUT
## configure the ThinningSvc
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
svcMgr += createThinningSvc(svcName="MyThinningSvc",
                            outStreams=[job.OutStream])

## remap some clids/keys to new ones
try:
    import JetRec.ParticleJetCompatibility
except ImportError:
    include ('JetRec/ParticleJetCompatibility.py')
    # FIXME: remove when bug #38062 properly fixed
    _tk=svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps[:] + [
        "ParticleJetContainer#AtlfastParticleJetContainer->"\
        "JetCollection#AtlfastJetContainer"
        ]
    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps = [
        i.replace("ParticleJetContainer#", "1118613496#")
        for i in _tk
        ]
    pass
## EOF ##
