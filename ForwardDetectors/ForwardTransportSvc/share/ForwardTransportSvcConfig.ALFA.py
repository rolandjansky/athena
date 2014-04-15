from ForwardTransportSvc.ForwardTransportFlags import forwardTransportFlags
forwardTransportFlags.detectorFlag = "ALFA"

from AthenaCommon.CfgGetter import getService
forwardTransportSvc = getService("ForwardTransportSvc", checkType=True)
