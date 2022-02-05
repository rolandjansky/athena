include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

LArCondCnvDbServer="atlobk02.cern.ch"

#LArCondCnvDbServer="atlmysql01.cern.ch"
include("LArCondCnv/LArCondCnv_Config_jobOptions.py")
include("LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py")

theApp.Dlls += [ "IOVDbSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.serverName=LArCondCnvDbServer
IOVDbSvc.DBname     = "conditions_ctb_2004";
IOVDbSvc.userName   = "conditions";
IOVDbSvc.userPwd    = "conditions";
#IOVDbSvc.OutputLevel = DEBUG
IOVDbSvc.Folders += ["/lar/LArElecCalibTB04/LArCalibParams"]
#---- ByteStream-Files input --------------------

#include( "TBCnv/TBReadH8BS_jobOptions.py" )
theApp.Dlls += [ "LArByteStream"]

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/lar/2004/"]
ByteStreamInputSvc.FilePrefix += ["daq_ROS-41_LargCalib"]
ByteStreamInputSvc.RunNumber = [2417]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibTest"]
theApp.Dlls += ["LArRecUtils"]

ToolSvc.LArBadChannelTool.geometry="H8"

theApp.TopAlg += [ "LArCablingChecker" ]
LArCablingChecker = Algorithm( "LArCablingChecker" )

LArCablingChecker.DigitKey = "HIGH"
LArCablingChecker.FileName = "CablingSummary.txt"
LArCablingChecker.PrintAllCellsAndEvents = True
LArCablingChecker.PrintDisconnectedCells = False
LArCablingChecker.PrintEventSummary = True
LArCablingChecker.ADCThreshold = 1250
LArCablingChecker.DACHighGainThreshold = 100
LArCablingChecker.DACMediumGainThreshold = 1000
LArCablingChecker.UseBadChannelTool = True

#LArCablingChecker.OutputLevel = 0

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

#theApp.EvtMax=5
MessageSvc.OutputLevel=INFO

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
