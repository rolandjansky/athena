import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

import AthenaPoolCnvSvc.AthenaPool
import IOVDbSvc.IOVDb

theApp.EvtMax = 1

from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

svcMgr += getConfigurable( "CondProxyProvider" )()
svcMgr.CondProxyProvider.InputCollections = ["L1CaloRampData.pool.root"]

IOVSvc=Service("IOVSvc")
IOVSvc.preLoadData=True

from IOVDbSvc.CondDB import conddb
conddb.addFolder("", "<dbConnection>sqlite://;schema=energyscanresults.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Results/EnergyScanResults<tag>HEAD</tag>")

from TrigT1CaloCalibUtils.L1CaloDumpRampDataAlgorithm import L1CaloDumpRampDataAlgorithm
topSequence += L1CaloDumpRampDataAlgorithm()
