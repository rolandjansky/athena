include("AthenaPoolDataModelTest/esdtoesd_base.py")

svcMgr.EventSelector.InputCollections = [ "myESD.pool.root" ]
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
checker = TrigEDMChecker()
topSequence += checker
