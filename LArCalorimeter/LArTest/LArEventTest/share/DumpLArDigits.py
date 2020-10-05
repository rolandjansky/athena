#tested setup: asetup 20.1.5.8,here


from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

include("LArConditionsCommon/LArMinimalSetup.py")

svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2015-05"

#Specify the input file(s)
svcMgr.EventSelector.Input=["/scratch/wlampl/data15_13TeV/data15_13TeV.00267073.express_express.merge.RAW._lb0706._SFO-ALL._0001.1",]

# Specify the object you want to read from ByteStream
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]

from LArEventTest.LArEventTestConf import DumpLArDigits
topSequence+=DumpLArDigits(LArDigitContainerName="FREE")

theApp.EvtMax=5

#svcMgr.MessageSvc.OutputLevel=DEBUG
svcMgr.StoreGateSvc.Dump=True
