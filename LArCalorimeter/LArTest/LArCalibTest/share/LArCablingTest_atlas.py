include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

LArCondCnvDbServer="atlobk02.cern.ch"

#LArCondCnvDbServer="atlmysql01.cern.ch"
include("LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include("LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py")
#include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
#include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )

theApp.Dlls += [ "IOVDbSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

# Getting calib info from file
theApp.Dlls += [ "LArCalibUtils" ]
theApp.TopAlg += ["LArCalibDigitMaker"]
LArCalibDigitMaker = Algorithm("LArCalibDigitMaker")
LArCalibDigitMaker.KeyList += ["HIGH"]
LArCalibDigitMaker.KeyList += ["MEDIUM"]
include("daq_ROS-1_calib_pattern_10157.py")
#LArCalibDigitMaker.BoardIDs += [1007091712]
LArCalibDigitMaker.BoardIDs += [1018626048]
LArCalibDigitMaker.DontRun = True

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
EventSelector.Input += ["/castor/cern.ch/atlas/LargFec/Installation/daq_ROS-1_HIGH_InstCalib_0010157_file01.data"]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
#include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
# In new versions, but not 10.0.4 :
#include("LArDetDescr/LArDetDescr_joboptions.py")
#include("LArDetDescr/LArDetDescr_H8_joboptions.py")

include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py")


DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DoIdChecks

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE

theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibTest"]
theApp.Dlls += ["LArRecUtils"]

#ToolSvc.LArBadChannelTool.geometry="H8"

theApp.TopAlg += [ "LArCablingChecker" ]
LArCablingChecker = Algorithm( "LArCablingChecker" )

LArCablingChecker.DigitKey = "HIGH"
LArCablingChecker.FileName = "CablingSummary.txt"
LArCablingChecker.PrintAllCellsAndEvents = False
LArCablingChecker.PrintDisconnectedCells = False
LArCablingChecker.PrintEventSummary = False
LArCablingChecker.ADCThreshold = 1300
LArCablingChecker.DACHighGainThreshold = 100
LArCablingChecker.DACMediumGainThreshold = 1000
LArCablingChecker.UseBadChannelTool = False

#LArCablingChecker.OutputLevel = 0

#theApp.EvtMax=5
MessageSvc.OutputLevel=INFO

#StoreGateSvc=Service("StoreGateSvc");
#StoreGateSvc.dump=TRUE

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
