# CoolHistExample_jobOptions.py
# demonstration of reading histograms stored in files referenecd by COOL

from AthenaCommon.AlgSequence import AlgSequence
topSeq=AlgSequence()
from DetDescrCondExample.DetDescrCondExampleConf import CoolHistExample
topSeq+=CoolHistExample(RetrieveDir=False)

# setup the database access - the file share/histdemo.db should be copied
# to your run directory before running Athena
from IOVDbSvc.CondDB import conddb
conddb.addFolder("","<db>sqlite://;schema=histdemo.db;dbname=HISTDEMO</db> /DEMO/TESTHIST1 <tag>demo_tag</tag>")
conddb.addFolder("","<db>sqlite://;schema=histdemo.db;dbname=HISTDEMO</db> /DEMO/TESTDIR1 <tag>demodir_tag</tag>")

# see debugging printout from CoolHistSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from DetDescrCondTools.DetDescrCondToolsConf import CoolHistSvc
svcMgr+=CoolHistSvc(OutputLevel=2)
