if not isinstance(FNAMES, (list,tuple)):
    FNAMES = [FNAMES]
    pass

if not 'TUPLENAME' in dir():
    TUPLENAME = 'egamma'

import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = FNAMES
svcMgr.EventSelector.TupleName = TUPLENAME

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

import AthenaRootComps.AthenaRootCompsConf as arcc
if not 'USEVARHANDLE' in dir():
    USEVARHANDLE=1
if USEVARHANDLE: job += arcc.Athena__RootAsciiDumperAlgHandle("rootdumper")
else:            job += arcc.Athena__RootAsciiDumperAlg      ("rootdumper")


import os, time
print 'attach now', os.getpid()
time.sleep(10)

theApp.initialize()
theApp.nextEvent()
theApp.nextEvent()
theApp.seekEvent(12177)
theApp.seekEvent(24412)
theApp.seekEvent(24339)

theApp.finalize().ignore()
theApp.exit()
