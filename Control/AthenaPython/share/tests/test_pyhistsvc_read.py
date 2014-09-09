## joboptions file to read ROOT objects via ITHistSvc
import AthenaCommon.AtlasUnixStandardJob
import AthenaCommon.SystemOfUnits as Units
import AthenaCommon.Constants as Lvl

# the job
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaPython.tests.PyTHistTestsLib import PyHistReader
job += PyHistReader()

# define histsvc {in/out}put streams
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'THistSvc'):
    svcMgr += CfgMgr.THistSvc()
hsvc = svcMgr.THistSvc
hsvc.Input = [ "read1 DATAFILE='tuple2.root' TYP='ROOT' OPT='READ'",
               "read2 DATAFILE='tuple3.root' TYP='ROOT' OPT='READ'", ]
hsvc.PrintAll = True

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1
