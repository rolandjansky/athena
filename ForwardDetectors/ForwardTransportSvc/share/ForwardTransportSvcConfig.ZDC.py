from ForwardTransportSvc.ForwardTransportFlags import forwardTransportFlags
forwardTransportFlags.detectorFlag = "ZDC"

from AthenaCommon.CfgGetter import getService
forwardTransportSvc = getService("ForwardTransportSvc", checkType=True)
