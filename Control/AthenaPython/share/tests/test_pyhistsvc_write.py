## joboptions file to write ROOT objects via ITHistSvc
import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.SystemOfUnits as Units
import AthenaCommon.Constants as Lvl

# the job
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaPython.tests.PyTHistTestsLib import PyHistWriter
job += PyHistWriter()

# define histsvc {in/out}put streams
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'THistSvc'):
    svcMgr += CfgMgr.THistSvc()
hsvc = svcMgr.THistSvc
hsvc.Output += [ "new DATAFILE='tuple1.root' TYP='ROOT' OPT='NEW'",
                 "upd DATAFILE='tuple2.root' TYP='ROOT' OPT='UPDATE'",
                 "rec DATAFILE='tuple3.root' TYP='ROOT' OPT='RECREATE'", ]
hsvc.PrintAll = True

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20

#################################
## just to make sure the 'new'
## thistsvc stream will work as expected
import os
if os.path.exists('tuple1.root'):
    os.unlink('tuple1.root')
    pass
#################################
