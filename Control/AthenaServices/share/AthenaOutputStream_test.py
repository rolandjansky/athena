## @file   AthenaOutputStream_test.py
# @brief job opts for AthenaOutputStream unit test
#
# $Id: AthenaOutputStream_test.py,v 1.4 2007-06-23 01:20:31 calaf Exp $
include( "StoreGate/StoreGateTestCommon.py" )

theApp.Dlls += [ "AthenaServices" ]
MessageSvc.OutputLevel = 3
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3
AthenaOutputStream = Algorithm( "AthenaOutputStream" )
AthenaOutputStream.OutputLevel = 1
AthenaOutputStream.ItemList=["Bar#uno", "Bar#due", "Bar#tre" ,
			     "8101#*","13#*"]       
AthenaOutputStream.AcceptAlgs=["AthenaOutputStream", "aSFQS"]
